#include "messagethread.h"

#include <QDebug>
#include <QThread>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QBuffer>
#include <QImage>
#include <QFileInfo>
#include <QPixmap>
#include <QFile>
#include <QMutex>
#include <QWaitCondition>
#include <QDataStream>

#pragma execution_character_set("utf-8")

MessageThread::MessageThread()
{
    socket = new QWebSocket();
    connect(socket, &QWebSocket::connected, this,&MessageThread::socket_Connected,Qt::DirectConnection);
    connect(socket, &QWebSocket::disconnected, this, &MessageThread::socket_Disconnected,Qt::DirectConnection);
    socket->open(QUrl("ws://localhost:8964"));
    qDebug() << "Connected" << QThread::currentThreadId();
}
void MessageThread::Start(){
    qDebug()<< "start listen loop";
}

void MessageThread::socket_Connected(){
    qDebug() << "Connect successfully!";
    connect(socket, &QWebSocket::textMessageReceived,
                this, &MessageThread::Receive,Qt::DirectConnection);
    connect(socket, &QWebSocket::binaryMessageReceived,
                this, &MessageThread::ReceiveData,Qt::DirectConnection);
    //生成JSON字符串
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","auth");
    msgData.insert("token",Token);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    //发送数据
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void MessageThread::Receive(QString data){
    qDebug()<<data;
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(data.toUtf8(), &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
        if (doucment.isObject()) { // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("action")) {  // 包含指定的 key
                QJsonValue value = object.value("action");
                QJsonObject obj = object.value("data").toObject();
                if(value.toString()=="updateList"){
                    qDebug()<<"updatList";
                    emit updateList(obj);
                }else if(value.toString()=="newTextMsg"){
                    qDebug()<<"newTextMsg";
                    msgHandler(obj);
                }else if(value.toString()=="auth"){
                    //addNewUser(obj,pClient);
                }else if(value.toString()=="newImg"){
                    qDebug()<<"newImgMsg";
                    imgHandler(obj);
                }else if(value.toString()=="broadcast"){
                    broadcastHandler(obj);
                }else if(value.toString()=="newFile"){
                    fileHandler(obj);
                }else if(value.toString()=="newGroup"){
                    newGroupHandler(obj);
                }else if(value.toString()=="withDraw"){
                    emit backMsg(obj.value("mid").toString(),obj.value("from").toString());
                }else if(value.toString()=="newShake"){
                    emit doShake(obj.value("from").toString());
                }else if(value.toString()=="newRedPack"){
                    Message* msg = new Message("redpacknormal","",obj.value("from").toString(),userName,0);
                    msg->avatarName = obj.value("avatarname").toString();
                    msg->setTime(obj.value("time").toString());
                    msg->mid=obj.value("mid").toString();
                    emit newMsg(msg);
                }else if(value.toString()=="garbResult"){
                    if(obj.value("status").toString()=="1"){
                        addGrade(obj.value("garbed").toInt());
                    }
                    emit showResult(obj);
                }else if(value.toString()=="newSpeak"){
                    speakHandler(obj);
                }
            }
        }
    }else{

       qDebug()<<"无法解析请求"+data;
    }
}

void MessageThread::newGroupHandler(QJsonObject data){
    emit newJoinedGroup(data.value("groupname").toString());
}

void MessageThread::fileHandler(QJsonObject data){
    Message* msg = new Message("file",data.value("realname").toString(),data.value("from").toString(),userName,0);
    msg->total = data.value("size").toInt();
    msg->fileName = data.value("filename").toString();
    msg->mid = data.value("mid").toString();
    msg->avatarName = data.value("avatarname").toString();
    msg->setTime(data.value("time").toString());
    emit newMsg(msg);
}

void MessageThread::broadcastHandler(QJsonObject data){
    emit updateStatus(data.value("status").toInt(),data.value("username").toString());
}

void MessageThread::msgHandler(QJsonObject data){
    Message* msg = new Message("text",data.value("body").toString(),data.value("from").toString(),userName,0);
    msg->avatarName = data.value("avatarname").toString();
    msg->setTime(data.value("time").toString());
    msg->mid=data.value("mid").toString();
    emit newMsg(msg);
}

void MessageThread::speakHandler(QJsonObject data){
    QByteArray content = data.value("body").toString().toUtf8();
    QByteArray realContent = QByteArray::fromBase64(content);
    QString fname = QString::number(QDateTime::currentDateTime().toTime_t());
    QFile file("tmp/tg_record_"+fname+"s.wav");
    qDebug()<<"filesave  tmp/tg_record_"+fname+"s.wav";
    file.open(QIODevice::WriteOnly);
    file.write(realContent);
    file.close();
    Message* msg = new Message("speak",fname+"s",data.value("from").toString(),userName,0);
    msg->avatarName = data.value("avatarname").toString();
    msg->setTime(data.value("time").toString());
    msg->total = data.value("sec").toInt();
    msg->mid=data.value("mid").toString();
    emit newMsg(msg);
}

void MessageThread::imgHandler(QJsonObject data){
    Message* msg = new Message("img",data.value("body").toString(),data.value("from").toString(),userName,0);
    msg->avatarName = data.value("avatarname").toString();
    msg->mid = data.value("mid").toString();
    msg->setTime(data.value("time").toString());
    emit newMsg(msg);
}

void MessageThread::Download(QString realname, QString filename, qint64 size){
    realName = realname;
    totalSize = size;
    nowFileName = filename;
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","download");
    msgData.insert("name",realname);
    msgData.insert("receiver",userName);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    file = new QFile("tmp/"+filename);
    file->open(QIODevice::WriteOnly);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void MessageThread::ReceiveData(QByteArray data){
    if(data == QString("ok,finished52121225").toUtf8()){
        file->close();
        delete file;
    }else{
        if(!file->isOpen()){
            file->open(QIODevice::WriteOnly);
        }
        file->write(data);
        emit setProgress(realName,file->size(),totalSize);
    }

}

void MessageThread::sendMsg(QString to, QString body, QString type, QString from,QString mid){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","sendTextMsg");
    msgData.insert("to",to);
    msgData.insert("from",from);
    msgData.insert("body",body);
    msgData.insert("mid",mid);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

QString MessageThread::randomString(){
        int max = 8;
        QString tmp = QString("0123456789ABCDEFGHIJKLMNOPQRSTUVWZYZ");
        QString str = QString();
        QTime t;
        t= QTime::currentTime();
        qsrand(t.msec()+t.second()*1000);
        for(int i=0;i<max;i++) {
            int ir = qrand()%tmp.length();
            str[i] = tmp.at(ir);
        }
        return str;
}

void MessageThread::sendImg(QString to,QString path,QString from,QString selfAvatar){
    QFileInfo fileinfo = QFileInfo(path);
    QImage image(path);
    QByteArray ba;
    QBuffer buf(&ba);
    image.save(&buf,fileinfo.suffix().toLatin1());
    QByteArray hexed = ba.toBase64();
    buf.close();
    Message *msg = new Message("img",QString(hexed),from,to,1);
    msg->setTime(QTime::currentTime().toString("hh:mm"));
    msg->mid =randomString();
    msg->avatar = selfAvatar;
    emit newMsg(msg);
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","sendImg");
    msgData.insert("to",to);
    msgData.insert("from",from);
    msgData.insert("mid",msg->mid);
    msgData.insert("body",QString(hexed));
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void MessageThread::sendFile(QString to,QString path,QString from,QString selfAvatar){
    QFileInfo fileinfo = QFileInfo(path);

    //发送文件头
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","sendFile");
    msgData.insert("to",to);
    msgData.insert("from",from);
    QString mid = randomString();
    msgData.insert("mid",mid);
    msgData.insert("filename",fileinfo.fileName());
    msgData.insert("size",fileinfo.size());
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
    QString fileId = QString::number(QDateTime::currentDateTime().toTime_t());
    emit newSendFile(fileinfo.fileName(),fileinfo.size(),fileId,mid);
    QByteArray outBlock;
    QFile fileObj(path);
    fileObj.open(QFile::ReadOnly);
    QByteArray pack;
    qint64 readed=0;
    while(readed<fileinfo.size()){
        if((fileinfo.size()-readed)<512*1024){
            pack = fileObj.read(fileinfo.size()-readed);
        }else{
            pack = fileObj.read(512*1024);
        }

        readed +=socket->sendBinaryMessage(pack);
        socket->flush();
        qDebug()<<QString::number(readed);
        emit setProgress(fileId,readed,fileinfo.size());
        QMutex mutex;
        QWaitCondition sleep;
        mutex.lock();
        sleep.wait(&mutex, 500);
        mutex.unlock();
    }
    qDebug()<<socket->errorString();
    socket->sendBinaryMessage(QString("ok,finished52121225").toUtf8());
    socket->flush();

    //emit fileSended();

}

void MessageThread::createGroup(QString groupname, QString currentUser, QList<QString> users){
    qDebug()<<"newGroup2";
    QJsonObject newMessage;
    QJsonObject msgData;
    QJsonArray userList;
    newMessage.insert("action","createGroup");
    msgData.insert("groupname",groupname);
    userList.append(currentUser);
    QListIterator<QString> i(users);
    qDebug()<<"newGroup4";
    while (i.hasNext()) {
        userList.append(i.next());
    }
    qDebug()<<"newGroup5";
    msgData.insert("user",userList);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
    qDebug()<<"newGroup3";
}

void MessageThread::socket_Disconnected(){
    emit offLine();
}

void MessageThread::changeProfile(QString um, QString pwd){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","changeProfile");
    msgData.insert("origin",userName);
    msgData.insert("new",um);
    msgData.insert("pwd",pwd);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void MessageThread::withDraw(QString mid){
    if(Grade<1){
        emit inform("积分不足,无法撤回");
        return;
    }
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","withDraw");
    msgData.insert("mid",mid);
    msgData.insert("from",userName);
    msgData.insert("to",currentUm);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();

    emit backMsg(mid,"current");
    deaddGrade(1);
}

void MessageThread::sendRedPack(QString mid, int packNum, int packTotal, QString to, QString avatar){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","sendRedPack");
    msgData.insert("mid",mid);
    msgData.insert("from",userName);
    msgData.insert("packNum",packNum);
    msgData.insert("packTotal",packTotal);
    msgData.insert("to",to);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();

    deaddGrade(packTotal);
}

void MessageThread::deaddGrade(int g){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","decreaseGrade");
    msgData.insert("from",userName);
    msgData.insert("grade",g);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
    emit decreaseGrade(g);
}

void MessageThread::addGrade(int g){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","addGrade");
    msgData.insert("from",userName);
    msgData.insert("grade",g);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
    emit plusGrade(g);
}
void MessageThread::sendShake(QString to){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","sendShanke");
    msgData.insert("to",to);
    msgData.insert("from",userName);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void MessageThread::Garb(QString mid){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","garbRedPack");
    msgData.insert("from",userName);
    msgData.insert("mid",mid);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void MessageThread::startRecord(){

}

void MessageThread::stopRecord(QString sec,QString to,QString from,QString selfAvatar){
    qDebug()<<"stop"+sec;
    QFileInfo fileinfo = QFileInfo("tmp/tg_record.wav");
    QFile sourceFile("tmp/tg_record.wav");
    sourceFile.open(QFile::ReadWrite);

    QByteArray base64Encoded  = sourceFile.readAll().toBase64();
    sourceFile.close();
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","sendSpeak");
    msgData.insert("to",to);
    msgData.insert("from",from);
    msgData.insert("mid",randomString());
    msgData.insert("sec",sec.toInt());
    msgData.insert("body",QString(base64Encoded));
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();

}
