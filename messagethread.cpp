#include "messagethread.h"

#include <QDebug>
#include <QThread>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

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
                }
            }
        }
    }else{
       qDebug()<<"无法解析请求"+data;
    }
}

void MessageThread::msgHandler(QJsonObject data){
    Message* msg = new Message("text",data.value("body").toString(),data.value("from").toString(),userName,0);
    emit newMsg(msg);
}

void MessageThread::sendMsg(QString to, QString body, QString type, QString from){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","sendTextMsg");
    msgData.insert("to",to);
    msgData.insert("from",from);
    msgData.insert("body",body);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void MessageThread::socket_Disconnected(){

}
