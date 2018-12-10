#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "logindialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent, LoginDialog& ld, QApplication& app);

   ~MainWindow();

protected:
   void closeEvent(QCloseEvent *ce);

private slots:
   void loadBalance();

   void on_actionInfo_triggered();
   void on_actionHome_triggered();
   void on_actionLogout_triggered();
   void on_actionExit_triggered();
   void on_actionAbout_triggered();
   void on_actionAboutQt_triggered();
   void on_pushButtonChangePwd_clicked();
   void on_actionBlueMD_triggered();
   void on_actionBlue_triggered();
   void on_actionDarkVS_triggered();
   void on_actionDark_triggered();
   void on_actionGray_triggered();
   void on_pushButtonDeposit_clicked();
   void on_pushButtonWithdraw_clicked();
   void on_pushButtonChangePwdPay_clicked();

   void on_actionTransfer_triggered();

private:
   Ui::MainWindow *ui;

   LoginDialog& rLoginDialog;

   QApplication& rApp;
};

#endif // MAINWINDOW_H
