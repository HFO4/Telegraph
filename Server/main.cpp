#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QFont font("微软雅黑",9);
    QApplication a(argc, argv);
    a.setFont(font);
    MainWindow w;
    w.show();

    return a.exec();
}
