#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QTime>

class Message
{
public:
    Message(QString yt,QString b,QString f,QString t,bool self){
        type=yt;
        body=b;
        from=f;
        to=t;
        selfsend =self;
    }
    QString type;
    QString avatar;
    QString body;
    QString from;
    QString timeString;
    QString to;
    qint64 sended;
    qint64 total;
    QString fileName;
    QTime time;
    bool selfsend;
    void setTime(QString);
};

#endif // MESSAGE_H
