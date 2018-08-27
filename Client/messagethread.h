#ifndef MESSAGETHREAD_H
#define MESSAGETHREAD_H

#include "message.h"

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>
#include <QFile>
#include <QAudioRecorder>
#include <QAudioEncoderSettings>

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
    int Grade;
    QFile* file;
private:
    QWebSocket *socket;
    QAudioRecorder* audioRecorder;
    void socket_Connected();
    void socket_Disconnected();
    void Receive(QString data);
    void ReceiveData(QByteArray data);
    void msgHandler(QJsonObject data);
    void imgHandler(QJsonObject data);
    void speakHandler(QJsonObject data);
    void fileHandler(QJsonObject data);
    void deaddGrade(int);
    void broadcastHandler(QJsonObject data);
    QString randomString();
    void newGroupHandler(QJsonObject data);
signals:
    void updateList(QJsonObject);
    void newMsg(Message*);
    void newRedPack(Message*);
    void updateStatus(int,QString);
    void newSendFile(QString,qint64,QString,QString);
    void setProgress(QString,qint64,qint64);
    void newJoinedGroup(QString);
    void offLine();
    void backMsg(QString,QString);
    void plusGrade(int);
    void inform(QString);
    void decreaseGrade(int);
    void doShake(QString);
    void showResult(QJsonObject);
public slots:
    void sendMsg(QString to,QString body,QString type,QString from,QString mid);
    void sendImg(QString to,QString,QString from,QString);
    void sendFile(QString to,QString,QString from,QString);
    void createGroup(QString,QString,QList<QString>);
    void Download(QString,QString,qint64);
    void sendShake(QString);
    void changeProfile(QString,QString);
    void withDraw(QString);
    void addGrade(int);
    void sendRedPack(QString mid,int packNum,int packTotal,QString to,QString avatar);
    void Garb(QString mid);
    void startRecord();
    void stopRecord(QString sec,QString to,QString from,QString selfAvatar);
public slots:
};

#endif // MESSAGETHREAD_H
