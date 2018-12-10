#ifndef TRANSFERDIALOG_H
#define TRANSFERDIALOG_H

#include <QDialog>

#include "account.h"

namespace Ui {
class TransferDialog;
}

class TransferDialog : public QDialog
{
   Q_OBJECT

public:
   TransferDialog(QWidget *parent, Account _acc);
   ~TransferDialog();

private slots:
   void on_lineEditRCard_textEdited(const QString& arg1);

   void on_pushButtonYes_clicked();

   void on_pushButtonNo_clicked();

private:
   Ui::TransferDialog *ui;

   Account acc;
};

#endif // TRANSFERDIALOG_H
