#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "user.h"

#include <QMainWindow>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QHash>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Log(QString s);
    void handleReq(QString data,QWebSocket* pClient);
    void registerNewUser(QJsonObject userData);
    void addNewUser(QJsonObject userData,QWebSocket* pClient);
    void Login(QJsonObject userData);
    void sendMsg(QJsonObject msgData);
    void sendImg(QJsonObject msgData);
    void sendFile(QJsonObject msgData);
    void changeProfile(QJsonObject msgData);
    void createGroup(QJsonObject msgData);
    void withDraw(QJsonObject msgData);
    void Download(QJsonObject msgData);
    void addGrade(QJsonObject msgData);
    void sendRedPack(QJsonObject msgData);
    void sendSpeak(QJsonObject msgData);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    QWebSocketServer* server;
    void server_New_Connect();
    void socket_Read_Data(QString message);
    void socket_Read_File(QByteArray message);
    void socket_Disconnected();
    QString errorMsg;
    QJsonObject returnData;
    QList<QWebSocket*> connectionList;
    QHash<QString, QList<QString> > groupList;
    QSqlDatabase db;
    QHash<QString,QJsonObject> line;
    QHash<QString, User> onlineList;
    int wiiRepaly=1;
signals:
    void startThread();
};

#endif // MAINWINDOW_H
