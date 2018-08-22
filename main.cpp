#include "mainwindow.h"
#include <QApplication>
#pragma execution_character_set("utf-8")
int main(int argc, char *argv[])
{
    QFont font("微软雅黑",9);
    QApplication a(argc, argv);
    a.setFont(font);
    MainWindow w;
    w.show();
    return a.exec();
}
