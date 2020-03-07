#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //! set debug message pattern
    qSetMessagePattern("[%{type}] %{appname} (%{file}:%{line}) - %{message}");
    MainWindow w;
    w.show();

    return a.exec();
}
