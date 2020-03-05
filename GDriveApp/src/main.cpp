#include "mainwindow.h"
#include <QApplication>
#include <QPlainTextEdit>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qSetMessagePattern("[%{type}] %{appname} (%{file}:%{line}) - %{message}");
    MainWindow w;
    w.show();

    return a.exec();
}
