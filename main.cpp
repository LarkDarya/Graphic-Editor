#include "mainwindow.h"

#include <QApplication>

//проверка на коммит из QT

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
