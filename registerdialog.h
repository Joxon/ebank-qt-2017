#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

#include "logindialog.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
   Q_OBJECT

public:
   RegisterDialog(QWidget *parent, LoginDialog& ld, QString& cardid);
   ~RegisterDialog();

   QString generateNewCardID(const QString bankname);

private slots:
   void on_pushButtonRegister_clicked();

   void on_checkBoxShowPwd_clicked(bool checked);

   void on_pushButtonRandID_clicked();

   void on_pushButtonRandPhone_clicked();

   void on_checkBoxShowPwdPay_clicked(bool checked);

private:
   Ui::RegisterDialog *ui;

   LoginDialog& rLd;

   QString& rCardID;
};

#endif // REGISTERDIALOG_H
