#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "user.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QJsonArray>
#include <QFile>

#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug()<<"write";

}

void MainWindow::server_New_Connect(){
    QWebSocket* socketCon = server->nextPendingConnection();
    connect(socketCon, &QWebSocket::textMessageReceived, this, &MainWindow::socket_Read_Data);
    connect(socketCon, &QWebSocket::binaryMessageReceived, this, &MainWindow::socket_Read_File);
    connect(socketCon, &QWebSocket::disconnected, this, &MainWindow::socket_Disconnected);
    connectionList.append(socketCon);
    Log("A Client connect!");

}

void MainWindow::socket_Read_File(QByteArray message){
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QHash<QString, User>::const_iterator i;
    QString senderName;
    Log("write");
   for(i=onlineList.constBegin(); i!=onlineList.constEnd(); ++i){
        if(i.value().socket == pClient){
            senderName = i.key();
            break;
        }
    }
    if(line.contains(senderName)){
        if(message==QString("ok,finished52121225").toUtf8()){
             onlineList[senderName].file->close();
             if(line[senderName].value("to").toString().contains("[群聊]")){
                 QList<QString> users = groupList[line[senderName].value("to").toString().replace("[群聊]","")];
                 QListIterator<QString> ii(users);
                 while (ii.hasNext()) {
                     QString user = ii.next();
                     if(onlineList.contains(user)){
                         if(user==line[senderName].value("from").toString()){
                             continue;
                         }
                          onlineList[user].sendFile(onlineList[senderName].file->fileName(),line[senderName].value("filename").toString(),line[senderName].value("to").toString(),line[senderName].value("size").toInt(),line[senderName].value("mid").toString(),line[senderName].value("from").toString());
                     }
                 }
             }else{
                QHash<QString, User>::iterator i = onlineList.find(line[senderName].value("to").toString());
                if(i != onlineList.end()){
                    Log("writeFound");

                 i.value().sendFile(onlineList[senderName].file->fileName(),line[senderName].value("filename").toString(),line[senderName].value("from").toString(),line[senderName].value("size").toInt(),line[senderName].value("mid").toString());

                }else{
                    onlineList[line[senderName].value("from").toString()].saveFileMsg(line[senderName]);
                }
            }
            delete onlineList[senderName].file;
        }else{
            onlineList[senderName].file->write(message);
        }
    }
}

void MainWindow::socket_Read_Data(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(!message.isEmpty())
    {
        if(message.length()<=5000){
            Log(message);
        }

        handleReq(message,pClient);

    }
    QJsonObject newMessage;
    if(!errorMsg.isEmpty()){
        newMessage.insert("error",1);
        newMessage.insert("msg",errorMsg);
        errorMsg="";
    }else{
        newMessage.insert("error",0);
    }
    if(wiiRepaly == 0){

    }else{
        QJsonDocument document;
        document.setObject(newMessage);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        QString strJson(byteArray);
        pClient->sendTextMessage(strJson.toUtf8());
    }
    wiiRepaly=1;

}

void MainWindow::handleReq(QString data,QWebSocket *pClient){
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(data.toUtf8(), &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
        if (doucment.isObject()) { // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("action")) {  // 包含指定的 key
                QJsonValue value = object.value("action");
                QJsonObject obj = object.value("data").toObject();
                if(value.toString()=="register"){
                    registerNewUser(obj);
                }else if(value.toString()=="login"){
                    Login(obj);
                }else if(value.toString()=="auth"){
                    addNewUser(obj,pClient);
                    wiiRepaly=0;
                }else if(value.toString()=="sendTextMsg"){
                    sendMsg(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="sendImg"){
                    sendImg(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="sendFile"){
                    sendFile(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="download"){
                    Download(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="createGroup"){
                    createGroup(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="changeProfile"){
                    changeProfile(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="withDraw"){
                    withDraw(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="addGrade"){
                    addGrade(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="decreaseGrade"){
                    onlineList[obj.value("from").toString()].decreaseGrade(obj.value("grade").toInt());
                    wiiRepaly=0;
                }else if(value.toString()=="sendShanke"){
                    if(onlineList.contains(obj.value("to").toString())){
                        onlineList[obj.value("to").toString()].sendShake(obj.value("from").toString());
                    }
                    wiiRepaly=0;
                }else if(value.toString()=="sendRedPack"){
                    sendRedPack(obj);
                    wiiRepaly=0;
                }else if(value.toString()=="garbRedPack"){
                    onlineList[obj.value("from").toString()].garbRedPack(obj.value("mid").toString());
                    wiiRepaly=0;
                }else if(value.toString()=="sendSpeak"){
                    sendSpeak(obj);
                    wiiRepaly=0;
                }


            }
        }
    }else{
        errorMsg = "无法解析请求";
    }
}

void MainWindow::sendSpeak(QJsonObject msgData){
    if(msgData.value("to").toString().contains("[群聊]")){
        QList<QString> users = groupList[msgData.value("to").toString().replace("[群聊]","")];
        QListIterator<QString> i(users);
        while (i.hasNext()) {
            QString user = i.next();
            if(onlineList.contains(user)){
                if(user==msgData.value("from").toString()){
                    continue;
                }
                onlineList[user].sendSpeak(msgData.value("sec").toInt(),msgData.value("body").toString(),msgData.value("to").toString(),msgData.value("mid").toString(),msgData.value("from").toString());
            }
        }
        return;
    }
    QHash<QString, User>::iterator i = onlineList.find(msgData.value("to").toString());
    if(i != onlineList.end()){
        i.value().sendSpeak(msgData.value("sec").toInt(),msgData.value("body").toString(),msgData.value("from").toString(),msgData.value("mid").toString());
    }else{
       //onlineList[msgData.value("from").toString()].saveSpeakMsg(msgData);
    }
}

void MainWindow::sendRedPack(QJsonObject msgData){
    onlineList[msgData.value("from").toString()].initRedPack(msgData);
    if(msgData.value("to").toString().contains("[群聊]")){
        QList<QString> users = groupList[msgData.value("to").toString().replace("[群聊]","")];
        QListIterator<QString> i(users);
        while (i.hasNext()) {
            QString user = i.next();
            if(onlineList.contains(user)){
                if(user==msgData.value("from").toString()){
                    continue;
                }
                onlineList[user].sendRedPack(msgData.value("mid").toString(),msgData.value("to").toString(),msgData.value("from").toString());
            }
        }
        return;
    }
    QHash<QString, User>::iterator i = onlineList.find(msgData.value("to").toString());
    if(i != onlineList.end()){
        i.value().sendRedPack(msgData.value("mid").toString(),msgData.value("from").toString());
    }else{
        onlineList[msgData.value("from").toString()].saveRedPackMsg(msgData);
    }
}

void MainWindow::addGrade(QJsonObject msgData){
    onlineList[msgData.value("from").toString()].addGrade(msgData.value("grade").toInt());
}

void MainWindow::withDraw(QJsonObject msgData){
    if(msgData.value("to").toString().contains("[群聊]")){
        QList<QString> users = groupList[msgData.value("to").toString().replace("[群聊]","")];
        QListIterator<QString> i(users);
        while (i.hasNext()) {
            QString user = i.next();
            if(onlineList.contains(user)){
                if(user==msgData.value("from").toString()){
                    continue;
                }
                onlineList[user].withDraw(msgData.value("mid").toString(),msgData.value("to").toString());
            }
        }
        return;
    }
    QHash<QString, User>::iterator i = onlineList.find(msgData.value("to").toString());
    if(i != onlineList.end()){
        i.value().withDraw(msgData.value("mid").toString(),msgData.value("from").toString());
    }else{
        if(onlineList.contains(msgData.value("from").toString())){
            onlineList[msgData.value("from").toString()].withDrawOfflineMsg(msgData.value("mid").toString());
        }

    }
}

void MainWindow::changeProfile(QJsonObject msgData){
    onlineList[msgData.value("origin").toString()].changeProfile(msgData);
    onlineList[msgData.value("origin").toString()].socket->close();
}

void MainWindow::createGroup(QJsonObject msgData){
   QJsonArray array = msgData.value("user").toArray();
   QList<QString> users;
   int nSize = array.size();
   for (int i = 0; i < nSize; ++i) {
        users.append(array.at(i).toString());
   }
   groupList.insert(msgData.value("groupname").toString(),users);
   for (int i = 0; i < nSize; ++i) {
        //users.append(array.at(i).toString());
       if(onlineList.contains(array.at(i).toString())){
            onlineList[array.at(i).toString()].newGroup(msgData.value("groupname").toString());
       }
   }
}

void MainWindow::Download(QJsonObject msgData){
    onlineList[msgData.value("receiver").toString()].sendFileData(msgData.value("name").toString());
}

void MainWindow::sendFile(QJsonObject msgData){
    line.insert(msgData.value("from").toString(),msgData);
    onlineList[msgData.value("from").toString()].file = new QFile("tmp/"+QString::number(QDateTime::currentDateTime().toTime_t()));
    onlineList[msgData.value("from").toString()].file->open(QIODevice::WriteOnly);
}

void MainWindow::sendMsg(QJsonObject msgData){
    if(msgData.value("to").toString().contains("[群聊]")){
        QList<QString> users = groupList[msgData.value("to").toString().replace("[群聊]","")];
        QListIterator<QString> i(users);
        while (i.hasNext()) {
            QString user = i.next();
            if(onlineList.contains(user)){
                if(user==msgData.value("from").toString()){
                    continue;
                }
                onlineList[user].sendTextMsg(msgData.value("body").toString(),msgData.value("to").toString(),msgData.value("mid").toString(),msgData.value("from").toString());
            }
        }
        return;
    }
    QHash<QString, User>::iterator i = onlineList.find(msgData.value("to").toString());
    if(i != onlineList.end()){
        i.value().sendTextMsg(msgData.value("body").toString(),msgData.value("from").toString(),msgData.value("mid").toString());
    }else{
        onlineList[msgData.value("from").toString()].saveTextMsg(msgData);
    }
}

void MainWindow::sendImg(QJsonObject msgData){
    if(msgData.value("to").toString().contains("[群聊]")){
        QList<QString> users = groupList[msgData.value("to").toString().replace("[群聊]","")];
        QListIterator<QString> i(users);
        while (i.hasNext()) {
            QString user = i.next();
            if(onlineList.contains(user)){
                if(user==msgData.value("from").toString()){
                    continue;
                }
                onlineList[user].sendImg(msgData.value("body").toString(),msgData.value("to").toString(),msgData.value("mid").toString(),msgData.value("from").toString());
            }
        }
        return;
    }
    QHash<QString, User>::iterator i = onlineList.find(msgData.value("to").toString());
    if(i != onlineList.end()){
        i.value().sendImg(msgData.value("body").toString(),msgData.value("from").toString(),msgData.value("mid").toString());
    }else{
       onlineList[msgData.value("from").toString()].saveImgMsg(msgData);
    }
}

void MainWindow::registerNewUser(QJsonObject userData){
    User newUser;
    newUser.db = &db;
    newUser.setUserInfo(userData.value("username").toString(),userData.value("password").toString(),userData.value("sex").toString(),userData.value("email").toString());
    if(!newUser.Register()){
        errorMsg = newUser.errMsg;
    }else{
         Log("New User "+userData.value("username").toString() +" Registered.");
    }
}

void MainWindow::Login(QJsonObject userData){
    User newUser;
    newUser.db = &db;
    newUser.setUserInfo(userData.value("username").toString(),userData.value("password").toString());
    if(!newUser.Login()){
        errorMsg = newUser.errMsg;
    }else{
         Log("User"+userData.value("username").toString() +" Log in.");
    }
}

void MainWindow::addNewUser(QJsonObject userData,QWebSocket* pClient){
    User newUser;
    newUser.db = &db;
    if(!newUser.LoginByToken(userData.value("token").toString() )){
        Log(userData.value("token").toString()+" auth failed.");
        return;
    }
    newUser.socket = pClient;
    onlineList.insert(newUser.userName,newUser);
    //获取在线用户列表
    QHash<QString, User>::const_iterator i;
    QJsonArray onlineUser;
    for(i=onlineList.constBegin(); i!=onlineList.constEnd(); ++i){
        QJsonObject userInfo;
        userInfo.insert("uid",i.value().uid);
        userInfo.insert("username",i.value().userName);
        userInfo.insert("email",i.value().Email);
        userInfo.insert("avatar",i.value().Avatar);
        onlineUser.append(userInfo);
        onlineList[i.value().userName].sendBroadcast(1,newUser.userName);
    }
    onlineList[newUser.userName].Init(onlineUser);
}

void MainWindow::socket_Disconnected()
{
    QString logOffUser;
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QHash<QString, User>::const_iterator i;
    for(i=onlineList.constBegin(); i!=onlineList.constEnd(); ++i){
        if(i.value().socket == pClient){
            logOffUser = i.key();
            break;
        }
    }
    Log("Disconnected!");
    if(!logOffUser.isEmpty()){
        onlineList.remove(logOffUser);
        Log("User "+logOffUser+" Log off!");
        QHash<QString, User>::const_iterator i2;
        for(i2=onlineList.constBegin(); i2!=onlineList.constEnd(); ++i2){
            onlineList[i2.value().userName].sendBroadcast(0,logOffUser);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    server->close();
    qDeleteAll(connectionList.begin(), connectionList.end());
}

void MainWindow::Log(QString s){
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("[hh:mm:ss] ");
    ui->textEdit->setText(ui->textEdit->toPlainText()+str+s+"\n");
}

void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setEnabled(false);
    server = new QWebSocketServer(QStringLiteral("Echo Server"),
                                  QWebSocketServer::NonSecureMode, this);
    qint16 port = ui->lineEdit->text().toInt();
    if(!server->listen(QHostAddress::Any, port)){
        Log("Failed to start WS server.");
        ui->pushButton->setEnabled(true);
        return;

    }
    connect(server, &QWebSocketServer::newConnection,
                    this, &MainWindow::server_New_Connect);
    Log("Listen succeessfully!(Port: "+ui->lineEdit->text()+")");
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");   						//127.0.0.1也是一样的,属于本地地址
    db.setUserName("root");       							//登陆MYSQL的用户名
    db.setPassword("root");    							//你自己登陆的密码
    db.setDatabaseName("telegraph");  						//登陆数据库的名称
    db.open();
    if(!db.open()){
        Log("Failed to connect to MySQL.");
        ui->pushButton_2->setEnabled(true);
        return;
    }
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_pushButton_2_clicked()
{
    server->close();
    qDeleteAll(connectionList.begin(), connectionList.end());
    db.close();
    Log("Server stoped.");
    delete server;
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->textEdit->setText("");
}
