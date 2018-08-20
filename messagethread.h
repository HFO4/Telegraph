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
signals:
    void updateList(QJsonObject);
    void newMsg(Message*);
public slots:
    void sendMsg(QString to,QString body,QString type,QString from);
public slots:
};

#endif // MESSAGETHREAD_H
