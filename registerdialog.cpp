#pragma execution_character_set("utf-8")

#include "registerdialog.h"
#include "ui_registerdialog.h"

#include <QtSql>
#include <QtCore>
#include <QtWidgets>

#include "randomidgenerator.h"

RegisterDialog::RegisterDialog(QWidget *parent, LoginDialog& ld, QString& cardid)
   : QDialog(parent)
   , ui(new Ui::RegisterDialog)
   , rLd(ld)
   , rCardID(cardid)
{
   ui->setupUi(this);
   setWindowFlags(Qt::WindowMinimizeButtonHint
                  | Qt::WindowCloseButtonHint);

   QStringList           bankNames;
   QVectorIterator<Bank> i(rLd.getBankList());
   while (i.hasNext())
   {
      bankNames.append(i.next().getName());
   }
   ui->comboBoxBanks->addItems(bankNames);
   ui->comboBoxBanks->setCurrentIndex(0);
}


RegisterDialog::~RegisterDialog()
{
   delete ui;
}


QString RegisterDialog::generateNewCardID(const QString bankname)
{
   Bank currBank;

   QVectorIterator<Bank> i(rLd.getBankList());
   while (i.hasNext())
   {
      currBank = i.next();
      if (currBank.getName() == bankname)
      {
         break;
      }
   }

   rCardID = currBank.getPrefix() // 6 + 13 digits
             + QString::number(currBank.getCount()).rightJustified(13, '0', true);

   currBank.setCount(currBank.getCount() + 1);
   QSqlQuery q("update banks" // watch for spaces!
               " set count = " + QString::number(currBank.getCount()) +
               " where prefix = " + currBank.getPrefix());
   if (!q.exec())
   {
      qDebug() << "RegisterDialog::generateNewCardID: update failed";
   }


   return rCardID;
}


void RegisterDialog::on_pushButtonRegister_clicked()
{
   QString name    = ui->lineEditUserName->text().trimmed();
   QString id      = ui->lineEditID->text().trimmed();
   QString bank    = ui->comboBoxBanks->currentText().trimmed();
   QString cardid  = generateNewCardID(bank);
   QString pwd     = ui->lineEditPassword->text().trimmed();
   QString pwdpay  = ui->lineEditPasswordPay->text().trimmed();
   QString phone   = ui->lineEditPhone->text().trimmed();
   float   balance = 0.0;

   QRegularExpression pwdReg("^(?=.*\\d)(?=.*[a-z])(?=.*[A-Z]).{6,18}$");

   if (!pwd.contains(pwdReg))
   {
      QMessageBox::warning(this, tr("注册"), tr("登录密码6-18位，必须包含大小写字母和数字。"));
      ui->lineEditPassword->setFocus();
      return;
   }

   QRegularExpression pwdpayReg("^\\d{6}$");

   if (!pwdpay.contains(pwdpayReg))
   {
      QMessageBox::warning(this, tr("注册"), tr("支付密码为6位数字！"));
      ui->lineEditPasswordPay->setFocus();
      return;
   }

   QRegularExpression nameReg("^[\u4e00-\u9fa5]+(·[\u4e00-\u9fa5]+)*$");

   if (!name.contains(nameReg))
   {
      QMessageBox::warning(this, tr("注册"), tr("请输入合法中文姓名！"));
      ui->lineEditUserName->setFocus();
      return;
   }

   QRegularExpression idReg("^[1-9]\\d{5}(18|19|([23]\\d))"                         // city code
                            "\\d{2}((0[1-9])|(10|11|12))(([0-2][1-9])|10|20|30|31)" // birthday
                            "\\d{3}[0-9Xx]$");                                      // vericode

   if (!id.contains(idReg))
   {
      QMessageBox::warning(this, tr("注册"), tr("请输入18位合法身份证号！"));
      ui->lineEditID->setFocus();
      return;
   }

   QRegularExpression phoneReg("^((13[0-9])"
                               "|(14[5|7])"
                               "|(15([0-3]|[5-9]))"
                               "|(17[6-8])"
                               "|(18([0-3]|[5-9])))" // carrier code
                               "\\d{8}$");           // personal code

   if (!phone.contains(phoneReg))
   {
      QMessageBox::warning(this, tr("注册"), tr("请输入11位合法手机号！"));
      ui->lineEditPhone->setFocus();
      return;
   }

   QSqlQuery query;

   query.prepare("insert into"
                 " accounts (Name, ID, Bank, CardID, Password, PasswordPay, Balance, Phone)"
                 " values (:Name, :ID, :Bank, :CardID, :Password, :PasswordPay, :Balance, :Phone)");
   query.bindValue(":Name", name);
   query.bindValue(":ID", id);
   query.bindValue(":Bank", bank);
   query.bindValue(":CardID", cardid);
   query.bindValue(":Password", pwd);
   query.bindValue(":PasswordPay", pwdpay);
   query.bindValue(":Balance", balance);
   query.bindValue(":Phone", phone);
   query.exec();

   accept();
}


void RegisterDialog::on_checkBoxShowPwd_clicked(bool checked)
{
   if (checked)
   {
      ui->lineEditPassword->setEchoMode(QLineEdit::Normal);
   }
   else
   {
      ui->lineEditPassword->setEchoMode(QLineEdit::Password);
   }
}


void RegisterDialog::on_pushButtonRandID_clicked()
{
   RandomIDGenerator gen;

   ui->lineEditID->setText(gen.generateID());
}


void RegisterDialog::on_pushButtonRandPhone_clicked()
{
   RandomIDGenerator gen;

   ui->lineEditPhone->setText(gen.generatePhone());
}


void RegisterDialog::on_checkBoxShowPwdPay_clicked(bool checked)
{
   if (checked)
   {
      ui->lineEditPasswordPay->setEchoMode(QLineEdit::Normal);
   }
   else
   {
      ui->lineEditPasswordPay->setEchoMode(QLineEdit::Password);
   }
}
