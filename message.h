#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>

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
    QString to;
    bool selfsend;
};

#endif // MESSAGE_H
