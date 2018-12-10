#pragma execution_character_set("utf-8")

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QtCore>
#include <QtSql>

#include "aboutdialog.h"
#include "transferdialog.h"

MainWindow::MainWindow(QWidget *parent, LoginDialog& ld, QApplication& app)
   : QMainWindow(parent)
   , ui(new Ui::MainWindow)
   , rLoginDialog(ld)
   , rApp(app)
{
   ui->setupUi(this);

   rLoginDialog.setShowsAgain(false);

   Account acc = rLoginDialog.getCurrentAccount();
   ui->labelBalance->setText(QString::number(acc.getBalance(), 'f', 2));
   ui->labelCardID->setText(acc.getCardid());
   ui->labelID->setText(acc.getId());
   ui->labelName->setText(acc.getName());
   ui->labelPhone->setText(acc.getPhone());
   ui->labelHello->setText(tr("你好，") + acc.getName());

   // live balance
   QTimer *balanceTimer = new QTimer(this);
   connect(balanceTimer, SIGNAL(timeout()), this, SLOT(loadBalance()));
   balanceTimer->start(1000);
}


MainWindow::~MainWindow()
{
   rLoginDialog.resetUI();
   delete ui;
}


void MainWindow::on_actionInfo_triggered()
{
   ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_actionHome_triggered()
{
   ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_actionLogout_triggered()
{
   QMessageBox::StandardButton clickedButton;
   clickedButton = QMessageBox::question(this, tr("注销"), tr("您确定要注销吗？")
                                         , QMessageBox::Yes | QMessageBox::No
                                         , QMessageBox::Yes);
   if (clickedButton == QMessageBox::Yes)
   {
      rLoginDialog.setShowsAgain(true);
      close();
   }
}


void MainWindow::on_actionExit_triggered()
{
   rLoginDialog.setShowsAgain(false);
   close();
}


void MainWindow::closeEvent(QCloseEvent *ce)
{
   if (rLoginDialog.showsAgain())
   {
      ce->accept();
   }
   else
   {
      QMessageBox::StandardButton clickedButton;
      clickedButton = QMessageBox::question(this, tr("退出"), tr("您确定要退出吗？")
                                            , QMessageBox::Yes | QMessageBox::No
                                            , QMessageBox::Yes);
      if (clickedButton == QMessageBox::Yes)
      {
         ce->accept();
      }
      else
      {
         ce->ignore();
      }
   }
}


void MainWindow::loadBalance()
{
   QSqlQuery q;

   q.prepare("select * from accounts where CardID = :id");
   q.bindValue(":id", rLoginDialog.getCurrentAccountPtr()->getCardid());
   q.exec();
   q.next();

   double b = q.value("Balance").toDouble();
   rLoginDialog.getCurrentAccountPtr()->setBalance(b);
   ui->labelBalance->setText(QString::number(b, 'f', 2));
}


void MainWindow::on_actionAbout_triggered()
{
   AboutDialog ad;

   ad.exec();
}


void MainWindow::on_actionAboutQt_triggered()
{
   QApplication::aboutQt();
}


void MainWindow::on_pushButtonChangePwd_clicked()
{
   Account *pacc  = rLoginDialog.getCurrentAccountPtr();
   QString oldPwd = ui->lineEditOldPwd->text().trimmed();
   QString newPwd = ui->lineEditNewPwd->text().trimmed();

   QRegExp pwdReg("^(?=.*\\d)(?=.*[a-z])(?=.*[A-Z]).{6,18}$");

   if (!newPwd.contains(pwdReg))
   {
      ui->lineEditNewPwd->clear();
      QMessageBox::warning(this, tr("修改密码"), tr("新登录密码6-18位，必须包含大小写字母和数字。"),
                           QMessageBox::Ok);
      return;
   }

   if (oldPwd == pacc->getPwd())
   {
      pacc->setPwd(newPwd);
      QSqlQuery q("update accounts"
                  " set Password = '" + pacc->getPwd() +
                  "' where CardID = " + pacc->getCardid());
      if (!q.exec())
      {
         qDebug() << "MainWindow::on_pushButtonChangePwd_clicked(): update failed";
         qDebug() << q.lastError();
         QMessageBox::critical(this, tr("修改密码"), tr("数据库异常！"),
                               QMessageBox::Ok);
      }
      else
      {
         ui->lineEditNewPwd->clear();
         ui->lineEditOldPwd->clear();
         QMessageBox::information(this, tr("修改密码"), tr("修改密码成功！"),
                                  QMessageBox::Ok);
      }
   }
   else
   {
      ui->lineEditOldPwd->clear();
      QMessageBox::warning(this, tr("修改密码"), tr("原密码不匹配！"),
                           QMessageBox::Ok);
   }
}


void MainWindow::on_pushButtonChangePwdPay_clicked()
{
   Account *pacc  = rLoginDialog.getCurrentAccountPtr();
   QString oldPwd = ui->lineEditOldPwdPay->text().trimmed();
   QString newPwd = ui->lineEditNewPwdPay->text().trimmed();

   QRegExp pwdpayReg("^\\d{6}$");

   if (!newPwd.contains(pwdpayReg))
   {
      ui->lineEditNewPwd->clear();
      QMessageBox::warning(this, tr("修改密码"), tr("新支付密码为6位数字，请重试"),
                           QMessageBox::Ok);
      return;
   }

   if (oldPwd == pacc->getPwdpay())
   {
      pacc->setPwdpay(newPwd);
      QSqlQuery q("update accounts"
                  " set PasswordPay = '" + pacc->getPwdpay() +
                  "' where CardID=" + pacc->getCardid());
      if (!q.exec())
      {
         qDebug() << "MainWindow::on_pushButtonChangePwd_clicked(): update failed";
         qDebug() << q.lastError();
         QMessageBox::critical(this, tr("修改密码"), tr("数据库异常！"),
                               QMessageBox::Ok);
      }
      else
      {
         ui->lineEditNewPwdPay->clear();
         ui->lineEditOldPwdPay->clear();
         QMessageBox::information(this, tr("修改密码"), tr("修改密码成功！"),
                                  QMessageBox::Ok);
      }
   }
   else
   {
      ui->lineEditOldPwdPay->clear();
      QMessageBox::warning(this, tr("修改密码"), tr("原密码不匹配！"),
                           QMessageBox::Ok);
   }
}


void MainWindow::on_actionBlueMD_triggered()
{
   QFile f(":/qss/blue-md.qss");

   if (f.open(QFile::ReadOnly | QFile::Text))
   {
      QTextStream ts(&f);
      rApp.setStyleSheet(ts.readAll());
   }

   QSqlQuery q("update appsettings set lastTheme = 'blue-md.qss'");
   if (!q.exec())
   {
      qDebug() << "MainWindow::on_actionBlue_triggered(): update appsettings failed";
   }
}


void MainWindow::on_actionBlue_triggered()
{
   QFile f(":/qss/blue.qss");

   if (f.open(QFile::ReadOnly | QFile::Text))
   {
      QTextStream ts(&f);
      rApp.setStyleSheet(ts.readAll());
   }

   QSqlQuery q("update appsettings set lastTheme = 'blue.qss'");
   if (!q.exec())
   {
      qDebug() << "MainWindow::on_actionBlue_triggered(): update appsettings failed";
   }
}


void MainWindow::on_actionDarkVS_triggered()
{
   QFile f(":/qss/dark-vs.qss");

   if (f.open(QFile::ReadOnly | QFile::Text))
   {
      QTextStream ts(&f);
      rApp.setStyleSheet(ts.readAll());
   }

   QSqlQuery q("update appsettings set lastTheme = 'dark-vs.qss'");
   if (!q.exec())
   {
      qDebug() << "MainWindow::on_actionBlue_triggered(): update appsettings failed";
   }
}


void MainWindow::on_actionDark_triggered()
{
   QFile f(":/qss/dark.qss");

   if (f.open(QFile::ReadOnly | QFile::Text))
   {
      QTextStream ts(&f);
      rApp.setStyleSheet(ts.readAll());
   }

   QSqlQuery q("update appsettings set lastTheme = 'dark.qss'");
   if (!q.exec())
   {
      qDebug() << "MainWindow::on_actionBlue_triggered(): update appsettings failed";
   }
}


void MainWindow::on_actionGray_triggered()
{
   QFile f(":/qss/gray.qss");

   if (f.open(QFile::ReadOnly | QFile::Text))
   {
      QTextStream ts(&f);
      rApp.setStyleSheet(ts.readAll());
   }

   QSqlQuery q("update appsettings set lastTheme = 'gray.qss'");
   if (!q.exec())
   {
      qDebug() << "MainWindow::on_actionBlue_triggered(): update appsettings failed";
   }
}


void MainWindow::on_actionTransfer_triggered()
{
   TransferDialog td(nullptr, rLoginDialog.getCurrentAccount());

   td.exec();
}


void MainWindow::on_pushButtonDeposit_clicked()
{
   bool   confirm = false;
   double amount  = QInputDialog::getDouble(this, tr("存款"), tr("请输入您存款的数目：（单笔不超过20万）")
                                            , 0, 0, 2e5, 2, &confirm
                                            , Qt::WindowCloseButtonHint);

   if (!confirm)
   {
      return;
   }
   Account *pcurrAccount = rLoginDialog.getCurrentAccountPtr();
   double  currBalance   = pcurrAccount->getBalance() + amount;
   QString currCardID    = pcurrAccount->getCardid();

   pcurrAccount->setBalance(currBalance);
   ui->labelBalance->setText(QString::number(currBalance, 'f', 2));

   QSqlQuery q;
   q.prepare("update accounts set Balance = :b where CardID = :c");
   q.bindValue(":b", QString::number(currBalance, 'f', 2));
   q.bindValue(":c", currCardID);
   if (!q.exec())
   {
      qDebug() << "MainWindow::on_pushButtonDeposit_clicked(): update accounts failed";
   }
}


void MainWindow::on_pushButtonWithdraw_clicked()
{
   bool   confirm     = false;
   double lastBalance = rLoginDialog.getCurrentAccount().getBalance();
   double amount      = QInputDialog::getDouble(this, tr("取款")
                                                , tr("请输入您取款的数目：（不大于")
                                                + QString::number(lastBalance, 'f', 2) + tr("元）")
                                                , 0, 0, lastBalance, 2, &confirm
                                                , Qt::WindowCloseButtonHint);

   if (!confirm)
   {
      return;
   }
   Account *pcurrAccount = rLoginDialog.getCurrentAccountPtr();
   double  currBalance   = pcurrAccount->getBalance() - amount;
   QString currCardID    = pcurrAccount->getCardid();

   pcurrAccount->setBalance(currBalance);
   ui->labelBalance->setText(QString::number(currBalance, 'f', 2));

   QSqlQuery q;
   q.prepare("update accounts set Balance = :b where CardID = :c");
   q.bindValue(":b", QString::number(currBalance, 'f', 2));
   q.bindValue(":c", currCardID);
   if (!q.exec())
   {
      qDebug() << "MainWindow::on_pushButtonWithdraw_clicked(): update accounts failed";
   }
}
