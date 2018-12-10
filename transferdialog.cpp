#pragma execution_character_set("utf-8")

#include "transferdialog.h"
#include "ui_transferdialog.h"

#include <QtSql>
#include <QMessageBox>

TransferDialog::TransferDialog(QWidget *parent, Account _acc) :
   QDialog(parent),
   ui(new Ui::TransferDialog),
   acc(_acc)
{
   ui->setupUi(this);

   ui->doubleSpinBoxMoney->setMinimum(0);
   ui->doubleSpinBoxMoney->setMaximum(acc.getBalance());
}


TransferDialog::~TransferDialog()
{
   delete ui;
}


void TransferDialog::on_lineEditRCard_textEdited(const QString& arg1)
{
   if (arg1.length() == 19)
   {
      QSqlQuery q;
      q.prepare("select * from accounts where CardID = :i");
      q.bindValue(":i", arg1);
      q.exec();
      if (q.next())
      {
         ui->labelRBank->setText(q.value("Bank").toString());
         ui->labelRName->setText(q.value("Name").toString());
      }
      else
      {
         ui->labelRBank->setText(tr("无此卡号"));
         ui->labelRName->setText(tr("无此卡号"));
      }
   }
   else
   {
      ui->labelRBank->setText(tr("等待输入卡号..."));
      ui->labelRName->setText(tr("等待输入卡号..."));
   }
}


void TransferDialog::on_pushButtonYes_clicked()
{
   double sval = ui->doubleSpinBoxMoney->value();

   if (sval <= 0)
   {
      QMessageBox::warning(this, tr("转账"), tr("转账金额有误！"));
      return;
   }

   QString rid = ui->lineEditRCard->text();
   QString sid = acc.getCardid();
   if (rid == sid)
   {
      QMessageBox::warning(this, tr("转账"), tr("不能给自己转账！"));
      return;
   }

   QSqlQuery q;

   q.prepare("select * from accounts where CardID = :i");
   q.bindValue(":i", rid);
   q.exec();
   q.next();
   double rblc = q.value("Balance").toDouble();
   double sblc = acc.getBalance();

   rblc += sval;
   sblc -= sval;

   q.prepare("update accounts set Balance = :b where CardID = :i");
   q.bindValue(":b", QString::number(rblc, 'f', 2));
   q.bindValue(":i", rid);
   q.exec();

   q.prepare("update accounts set Balance = :b where CardID = :i");
   q.bindValue(":b", QString::number(sblc, 'f', 2));
   q.bindValue(":i", sid);
   q.exec();

   QMessageBox::information(this, tr("转账"), tr("转账成功！"));
   accept();
}


void TransferDialog::on_pushButtonNo_clicked()
{
   reject();
}
