#include "loginthread.h"

#include <QWebSocket>
#include <QByteArray>
#include <QCryptographicHash>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

LoginThread::LoginThread(){
}
void LoginThread::startConnect(){
    socket = new QWebSocket();
    connect(socket, &QWebSocket::connected, this,&LoginThread::socket_Connected,Qt::DirectConnection);
    connect(socket, &QWebSocket::disconnected, this, &LoginThread::socket_Disconnected,Qt::DirectConnection);
    socket->open(QUrl("ws://localhost:8964"));
    qDebug() << "Connected" << QThread::currentThreadId();
}
void LoginThread::socket_Connected(){
    qDebug() << "Connect successfully!";
    QString md5;
    QByteArray bytePwd = password.toLatin1();
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    md5.append(bytePwdMd5.toHex());
    connect(socket, &QWebSocket::textMessageReceived,
                this, &LoginThread::socket_Read_Data,Qt::DirectConnection);
    //生成JSON字符串
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","login");
    msgData.insert("username",username);
    msgData.insert("password",md5);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    //发送数据
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void LoginThread::socket_Read_Data(QString buffer){
    if(!buffer.isEmpty())
    {
        QJsonParseError jsonError;
        QJsonDocument doucment = QJsonDocument::fromJson(buffer.toUtf8(), &jsonError);  // 转化为 JSON 文档
        if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
            if (doucment.isObject()) { // JSON 文档为对象
                QJsonObject object = doucment.object();  // 转化为对象
                if (object.contains("error")) {  // 包含指定的 key
                    QJsonValue value = object.value("error");
                    if(!value.toInt()){
                       socket->close();
                       //this->close();
                       emit loginSuccess();
                    }else{
                       errMsg = object.value("msg").toString();
                       socket->close();
                       emit loginFailed();
                       qDebug()<<buffer;
                    }

                }
            }
        }
    }
}

void LoginThread::socket_Disconnected(){

}
