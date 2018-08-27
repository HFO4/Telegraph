#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "regwindow.h"
#include "loginthread.h"
#include "home.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    RegWindow* reg;
    void loginFailed();
    void loginSuccess();
    LoginThread* loginThread;
    QThread* thread;
    Home* home;
signals:
    void startThread();
};

#endif // MAINWINDOW_H
