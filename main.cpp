#include "logindialog.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   LoginDialog ld(nullptr, app);

   ld.setShowsAgain(true);
   while (ld.showsAgain())
   {
      if (ld.exec() == QDialog::Accepted)
      {
         MainWindow mw(nullptr, ld, app);
         mw.show();
         app.exec();
      }
   }

   return 0;
}
