#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QMessageBox msg;

         msg.setText("你点击了登录");

         msg.exec();
}

void MainWindow::on_pushButton_2_clicked()
{
    MainWindow reg;
    reg.show();
}
