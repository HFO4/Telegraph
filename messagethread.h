#ifndef MESSAGETHREAD_H
#define MESSAGETHREAD_H

#include "message.h"

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>

class MessageThread : public QObject
{
    Q_OBJECT
public:
    MessageThread();
    void Start();
    QString Token;
    QString userName;
private:
    QWebSocket *socket;
    void socket_Connected();
    void socket_Disconnected();
    void Receive(QString data);
    void msgHandler(QJsonObject data);
    void imgHandler(QJsonObject data);
    void broadcastHandler(QJsonObject data);
signals:
    void updateList(QJsonObject);
    void newMsg(Message*);
    void updateStatus(int,QString);
public slots:
    void sendMsg(QString to,QString body,QString type,QString from);
    void sendImg(QString to,QString,QString from,QString);
public slots:
};

#endif // MESSAGETHREAD_H
