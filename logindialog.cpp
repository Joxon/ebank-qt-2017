#pragma execution_character_set("utf-8")

#include "logindialog.h"
#include "ui_logindialog.h"

#include <QtCore>
#include <QtWidgets>

#include "registerdialog.h"
#include "randomidgenerator.h"

LoginDialog::LoginDialog(QWidget *parent, QApplication& app)
   : QDialog(parent)
   , ui(new Ui::LoginDialog)
   , rApp(app)
{
   ui->setupUi(this);
   setWindowFlags(Qt::WindowMinimizeButtonHint
                  | Qt::WindowCloseButtonHint);


   openDB(QApplication::applicationDirPath() + "/ebank.db");

   // load appsettings
   QSqlQuery q;
   q.exec("select * from appsettings");
   q.next();
   if (q.value("remUid").toBool())
   {
      ui->usernameEdit->setText(q.value("lastUid").toString().trimmed());
      ui->checkBoxRemUid->setChecked(true);
   }
   if (q.value("remPwd").toBool())
   {
      ui->passwordEdit->setText(q.value("lastPwd").toString().trimmed());
      ui->checkBoxRemPwd->setChecked(true);
   }

   QFile f(":/qss/" + q.value("lastTheme").toString());

   if (f.open(QFile::ReadOnly | QFile::Text))
   {
      QTextStream ts(&f);
      rApp.setStyleSheet(ts.readAll());
   }
   else
   {
      qDebug() << "LoginDialog::LoginDialog(): qss open failed";
   }

   // dynamic banner
   QTimer *changeBannerTimer = new QTimer(this);
   int    changeIntervalMs   = 2000;
   connect(changeBannerTimer, SIGNAL(timeout()),
           this, SLOT(changeBanner()));
   changeBannerTimer->start(changeIntervalMs);

   // tcp server
   server = new QTcpServer(this);
   if (!server->listen(QHostAddress::LocalHost, 10086))
   {
      QMessageBox::critical(this, tr("电子银行门户服务器"),
                            tr("无法启动服务器: %1.")
                               .arg(server->errorString()));
      ui->labelServer->setText(tr("服务器异常"));
      return;
   }

   connect(server, SIGNAL(newConnection()), this, SLOT(acceptRequest()));

   ui->labelServer->setText(tr("服务器已启动"));
}


void LoginDialog::changeBanner()
{
   RandomIDGenerator gen;
   int               bannerNo = gen.genRandInt(1, 3);

   ui->widgetBanner->setStyleSheet("border-image: url(:/pics/banner"
                                   + QString::number(bannerNo) + ".jpg);");
}


void LoginDialog::acceptRequest()
{
   ui->labelServer->setText(tr("服务器已接受请求"));
   socket = server->nextPendingConnection();
   connect(socket, SIGNAL(readyRead()), this, SLOT(sendCardInfo()));
}


void LoginDialog::sendCardInfo()
{
   ui->labelServer->setText(tr("服务器正在响应"));

   QByteArray  bin    = socket->readAll();
   QStringList cmd    = QString(bin).split(";");
   QString     opcode = cmd.at(0);

   QByteArray bout;

   // bind a card
   // cmd = 00;#cardid;#phone;#paypwd
   // out = 0|1 (0=success, 1=fail)
   if (opcode == "00")
   {
      if (cmd.size() != 4)
      {
         bout = "1";
      }
      else
      {
         QString   cardid = cmd.at(1);
         QString   phone  = cmd.at(2);
         QString   paypwd = cmd.at(3);
         QSqlQuery q;
         q.prepare("select * from accounts "
                   "where CardID = :c and Phone = :p and PasswordPay = :pp");
         q.bindValue(":c", cardid);
         q.bindValue(":p", phone);
         q.bindValue(":pp", paypwd);
         q.exec();
         if (q.next())
         {
            bout = "0";
         }
         else
         {
            bout = "1";
         }
      }
   }
   // query a card
   // cmd = 01;#cardid;#paypwd
   // out = 1|#bankname;#balance
   else if (opcode == "01")
   {
      if (cmd.size() != 3)
      {
         bout = "1";
      }
      else
      {
         QString   cardid = cmd.at(1);
         QString   paypwd = cmd.at(2);
         QSqlQuery q;
         q.prepare("select * from accounts "
                   "where CardID = :c and PasswordPay = :pp");
         q.bindValue(":c", cardid);
         q.bindValue(":pp", paypwd);
         q.exec();
         if (q.next())
         {
            QStringList l;
            l << q.value("Bank").toString()
              << q.value("Balance").toString();
            bout = l.join(';').toUtf8();
         }
         else
         {
            bout = "1";
         }
      }
   }
   // pay with a card
   // cmd = 02;#cardid;#paypwd;#value
   // out = 0|1
   else if (opcode == "02")
   {
      if (cmd.size() == 4)
      {
         QString   cardid = cmd.at(1);
         QString   paypwd = cmd.at(2);
         QSqlQuery q;
         q.prepare("select * from accounts "
                   "where CardID = :c and PasswordPay = :pp");
         q.bindValue(":c", cardid);
         q.bindValue(":pp", paypwd);
         q.exec();
         if (q.next())
         {
            double value   = cmd.at(3).toDouble();
            double balance = q.value("Balance").toDouble();
            if (balance >= value)
            {
               balance -= value;
               q.prepare("update accounts "
                         "set Balance = :b "
                         "where CardID = :c and PasswordPay = :pp");
               q.bindValue(":c", cardid);
               q.bindValue(":pp", paypwd);
               q.bindValue(":b", QString::number(balance, 'f', 2));
               q.exec();

               bout = "0"; // success
            }
            else
            {
               bout = "3"; // insufficient balance
            }
         }
         else
         {
            bout = "2"; //  no record error
         }
      }
      else
      {
         bout = "1"; // cmd error
      }
   }
   // unknown opcode
   else
   {
      bout = "-1";
   }

   connect(socket, SIGNAL(disconnected()),
           socket, SLOT(deleteLater()));

   socket->write(bout);
   socket->disconnectFromHost();

   ui->labelServer->setText(tr("服务器发送信息完毕"));
}


bool LoginDialog::openDB(const QString dbname)
{
   if (database.isOpen())
   {
      qDebug() << "LoginDialog::openDB(): db is already open";
      return false;
   }

   database = QSqlDatabase::addDatabase("QSQLITE");
   database.setDatabaseName(dbname);

   if (!database.open())
   {
      qDebug() << "LoginDialog::openDB(): db.open() failed";
      QMessageBox::critical(this, tr("错误"), tr("无法打开数据库！"));
      return false;
   }

   QSqlQuery q(database);
   q.exec("select * from banks");
   while (q.next())
   {
      Bank bank;
      bank.setName(q.value("name").toString());
      bank.setPrefix(q.value("prefix").toString());
      bank.setCount(q.value("count").toInt());
      bankList.append(bank);
   }

   return true;
}


void LoginDialog::closeDB()
{
   if (database.isOpen())
   {
      database.close();
   }
   else
   {
      qDebug() << "LoginDialog::closeDB(): db is not open";
   }
}


LoginDialog::~LoginDialog()
{
   closeDB();
   delete ui;
}


void LoginDialog::setShowsAgain(bool b)
{
   bShowsAgain = b;
}


bool LoginDialog::showsAgain()
{
   return bShowsAgain;
}


void LoginDialog::closeEvent(QCloseEvent *ce)
{
   this->setShowsAgain(false);
   ce->accept();
}


void LoginDialog::on_usernameEdit_returnPressed()
{
   on_loginButton_clicked();
}


void LoginDialog::on_passwordEdit_returnPressed()
{
   on_loginButton_clicked();
}


void LoginDialog::on_loginButton_clicked()
{
   QString uid = ui->usernameEdit->text().trimmed();
   QString pwd = ui->passwordEdit->text().trimmed();

   if (database.isOpen())
   {
      QSqlQuery query;
      query.exec("select * from accounts where cardid='" + uid + "' and password='" + pwd + "'");
      if (query.next())
      {
         currentAccount.setPwd(query.value("Password").toString());
         currentAccount.setPwdpay(query.value("PasswordPay").toString());
         currentAccount.setPhone(query.value("Phone").toString());
         currentAccount.setName(query.value("Name").toString());
         currentAccount.setId(query.value("ID").toString());
         currentAccount.setCardid(query.value("CardID").toString());
         currentAccount.setBank(query.value("Bank").toString());
         currentAccount.setBalance(query.value("Balance").toDouble());

         if (ui->checkBoxRemUid->isChecked())
         {
            query.prepare("update appsettings set lastUid = :cardid");
            query.bindValue(":cardid", currentAccount.getCardid());
            query.exec();
         }

         if (ui->checkBoxRemPwd->isChecked())
         {
            query.prepare("update appsettings set lastPwd = :pwd");
            query.bindValue(":pwd", currentAccount.getPwd());
            query.exec();
         }

         accept();
      }
      else
      {
         ui->labelNotif->setText(tr("登录信息有误，请重试"));
      }
   }
}


void LoginDialog::on_registerButton_clicked()
{
   QString        cardid;
   RegisterDialog rd(nullptr, *this, cardid);

   if (rd.exec() == QDialog::Accepted)
   {
      ui->labelNotif->setText("注册成功！您的卡号为：" + cardid);
      ui->usernameEdit->setText(cardid);
      ui->passwordEdit->setFocus();
   }
   else
   {
      ui->labelNotif->setText("取消注册。");
   }
}


QVector<Bank> LoginDialog::getBankList() const
{
   return bankList;
}


void LoginDialog::resetUI()
{
   QSqlQuery q;

   q.exec("select * from appsettings");
   q.next();
   if (!q.value("remUid").toBool())
   {
      ui->usernameEdit->clear();
      ui->usernameEdit->setFocus();
   }
   if (!q.value("remPwd").toBool())
   {
      ui->passwordEdit->clear();
      ui->passwordEdit->setFocus();
   }
   ui->labelNotif->setText(tr("欢迎使用电子银行系统！"));
}


Account LoginDialog::getCurrentAccount() const
{
   return currentAccount;
}


Account *LoginDialog::getCurrentAccountPtr()
{
   return &currentAccount;
}


QSqlDatabase LoginDialog::getDatabase() const
{
   return database;
}


void LoginDialog::on_checkBoxRemUid_toggled(bool checked)
{
   QSqlQuery q;

   if (checked)
   {
      if (!q.exec("update appsettings set remUid = 1"))
      {
         qDebug() << "LoginDialog::on_checkBoxRemUid_toggled(): update failed";
      }
   }
   else
   {
      if (!q.exec("update appsettings set remUid = 0, lastUid = ''"))
      {
         qDebug() << "LoginDialog::on_checkBoxRemUid_toggled(): update failed";
      }
   }
}


void LoginDialog::on_checkBoxRemPwd_clicked(bool checked)
{
   QSqlQuery q;

   if (checked)
   {
      QMessageBox::StandardButton btn;
      btn = QMessageBox::question(this, tr("警告")
                                  , tr("记住密码会大大降低本程序的安全性，确定使用吗？"));
      if (btn == QMessageBox::No)
      {
         ui->checkBoxRemPwd->setChecked(false);
         if (!q.exec("update appsettings set remPwd = 0, lastPwd = ''"))
         {
            qDebug() << "LoginDialog::on_checkBoxRemPwd_toggled(): update failed";
         }
      }
      else
      {
         ui->checkBoxRemUid->setChecked(true);
         if (!q.exec("update appsettings set remPwd = 1"))
         {
            qDebug() << "LoginDialog::on_checkBoxRemPwd_toggled(): update failed";
         }
      }
   }
   else
   {
      if (!q.exec("update appsettings set remPwd = 0, lastPwd = ''"))
      {
         qDebug() << "LoginDialog::on_checkBoxRemPwd_toggled(): update failed";
      }
   }
}
