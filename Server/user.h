#ifndef USER_H
#define USER_H

#include <QString>
#include <QSqlDatabase>
#include <QWebSocket>
#include <QFile>
#include <QJsonArray>

class User
{
public:
    User();
    QString errMsg;
    void setUserInfo(QString username="",QString password="",QString sex="",QString email="");
    int Register();
    int Login();
    int LoginByToken(QString token);
    void sendTextMsg(QString msg,QString from,QString mid,QString avatarUser="self",QString stime="no");
    void sendImg(QString msg,QString from,QString mid,QString avatarUser="self",QString stime="no");
    void sendSpeak(int sec,QString msg,QString from,QString mid,QString avatarUser="self",QString stime="no");
    void sendFile(QString,QString,QString,qint64,QString mid,QString avatarUser="self",QString stime="no");
    void sendRedPack(QString mid,QString from,QString avatarUser="self",QString stime="no");
    void sendFileData(QString);
    void sendBroadcast(int,QString);
    void newGroup(QString);
    void garbRedPack(QString mid);
    void saveTextMsg(QJsonObject);
    void saveFileMsg(QJsonObject);
    void saveImgMsg(QJsonObject);
    void saveRedPackMsg(QJsonObject);
    void changeProfile(QJsonObject);
    void initRedPack(QJsonObject);
    void withDraw(QString mid,QString um);
    void withDrawOfflineMsg(QString mid);
    void addGrade(int g);
    void sendShake(QString from);
    void decreaseGrade(int g);
    QSqlDatabase* db;
    QString userName;
    QString passWord;
    bool isGroup;
    QString Sex;
    QString Email;
    int grade;
    int uid;
    QFile* file;
    int Avatar;
    QWebSocket* socket;
    void Init(QJsonArray onlineUser);
};

#endif // USER_H
