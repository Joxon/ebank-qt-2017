#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QtNetwork>

#include "account.h"
#include "bank.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
   Q_OBJECT

public:
   LoginDialog(QWidget *parent, QApplication& app);
   ~LoginDialog();

   void setShowsAgain(bool b);
   bool showsAgain();

   QSqlDatabase getDatabase() const;

   Account getCurrentAccount() const;
   Account *getCurrentAccountPtr();

   QVector<Bank> getBankList() const;

   void resetUI();

protected:
   void closeEvent(QCloseEvent *ce);

private slots:
   void changeBanner();
   void acceptRequest();
   void sendCardInfo();

   void on_usernameEdit_returnPressed();
   void on_passwordEdit_returnPressed();
   void on_loginButton_clicked();
   void on_registerButton_clicked();
   void on_checkBoxRemUid_toggled(bool checked);
   void on_checkBoxRemPwd_clicked(bool checked);

private:
   Ui::LoginDialog *ui;
   QApplication& rApp;
   bool bShowsAgain;

   QSqlDatabase database;

   QTcpServer *server;
   QTcpSocket *socket;

   Account currentAccount;
   QVector<Bank> bankList;

   bool openDB(const QString dbname);
   void closeDB();
};

#endif  // LOGINDIALOG_H
