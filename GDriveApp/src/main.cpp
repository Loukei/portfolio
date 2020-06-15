#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //! set Application
    QCoreApplication::setOrganizationName("Loukei Soft");
    QCoreApplication::setApplicationName("GDriveApp");
    QCoreApplication::setApplicationVersion("1.0.1");
    //! set debug message pattern
    qSetMessagePattern("[%{type}] %{appname} (%{file}:%{line}) - %{message}");
    MainWindow w;
    w.show();

    return a.exec();
}
