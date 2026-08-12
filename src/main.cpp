#include "mainwindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setFont(QFont("Microsoft JhengHei UI", 10));

    MainWindow w;
    w.show();
    return a.exec();
}
