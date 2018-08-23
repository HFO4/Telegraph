#ifndef MESSAGETHREAD_H
#define MESSAGETHREAD_H

#include "message.h"

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>
#include <QFile>

class MessageThread : public QObject
{
    Q_OBJECT
public:
    MessageThread();
    void Start();
    QString Token;
    QString userName;
    QString realName;
    QString nowFileName;
    QString currentUm;
    qint64 totalSize;
    QFile* file;
private:
    QWebSocket *socket;
    void socket_Connected();
    void socket_Disconnected();
    void Receive(QString data);
    void ReceiveData(QByteArray data);
    void msgHandler(QJsonObject data);
    void imgHandler(QJsonObject data);
    void fileHandler(QJsonObject data);
    void broadcastHandler(QJsonObject data);
    QString randomString();
    void newGroupHandler(QJsonObject data);
signals:
    void updateList(QJsonObject);
    void newMsg(Message*);
    void updateStatus(int,QString);
    void newSendFile(QString,qint64,QString,QString);
    void setProgress(QString,qint64,qint64);
    void newJoinedGroup(QString);
    void offLine();
    void backMsg(QString,QString);
public slots:
    void sendMsg(QString to,QString body,QString type,QString from,QString mid);
    void sendImg(QString to,QString,QString from,QString);
    void sendFile(QString to,QString,QString from,QString);
    void createGroup(QString,QString,QList<QString>);
    void Download(QString,QString,qint64);
    void changeProfile(QString,QString);
    void withDraw(QString);
public slots:
};

#endif // MESSAGETHREAD_H
