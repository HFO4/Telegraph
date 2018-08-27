#ifndef REGWINDOW_H
#define REGWINDOW_H

#include <QMainWindow>
#include <QWebSocket>

namespace Ui {
class RegWindow;
}

class RegWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegWindow(QWidget *parent = 0);
    ~RegWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::RegWindow *ui;
    QWebSocket *socket;
    void socket_Read_Data(QString buffer);
    void socket_Disconnected();
    void socket_Connected();
};

#endif // REGWINDOW_H
