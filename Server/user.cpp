#include "user.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QCryptographicHash>
#include <QDebug>
#include <QTime>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QWaitCondition>
#include <QJsonObject>
#include <QMutex>
#include <QFileInfo>

#pragma execution_character_set("utf-8")
User::User()
{

}

void User::setUserInfo(QString username, QString password, QString sex, QString email){
    userName = username;
    passWord = password;
    Sex = sex;
    Email = email;
}

int User::Login(){
        QSqlQuery query(*db);
        query.exec( "select * from users where `username`='"+userName+"' ");
        if(query.next()){
            int fieldNo = query.record().indexOf("password");
            QString pwd = query.value(fieldNo).toString();
            if(pwd == passWord){
                return 1;
            }else{
                errMsg = "用户名或密码错误";
                return 0;
            }
        }
        errMsg = "用户不存在";
        return 0;
}

int User::Register(){
    QString md5;
    QString sign = passWord+userName;
    QByteArray bytePwd = sign.toLatin1();
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    md5.append(bytePwdMd5.toHex());
    QSqlQuery query(*db);
    QString savesql = QString("INSERT INTO users(id,username,password,sex,email,sign)");
    savesql += QString(" VALUES(NULL,'%1','%2','%3','%4','%5')")
            .arg(userName)
            .arg(passWord)
            .arg(Sex)
            .arg(Email)
            .arg(md5);
    bool ok=query.exec(savesql);
    if(ok){
         return 1;
    }else{
        errMsg = "注册失败";
        return 0;
    }

}

int User::LoginByToken(QString token){
    QSqlQuery query(*db);
    query.exec( "select * from users where `sign`='"+token+"' ");
    if(query.next()){
        userName = query.value(query.record().indexOf("username")).toString();
        uid = query.value(query.record().indexOf("id")).toInt();
        Email = query.value(query.record().indexOf("email")).toString();
        Sex = query.value(query.record().indexOf("sex")).toString();
        passWord = query.value(query.record().indexOf("password")).toString();
        Avatar = query.value(query.record().indexOf("avatar")).toInt();
        grade = query.value(query.record().indexOf("grade")).toInt();
        return 1;
    }else{
    }
    errMsg = "用户不存在";
    return 0;
}

void User::garbRedPack(QString mid){
    QSqlQuery query(*db);
    query.exec( "select * from redpack where `mid`='"+mid+"' ");
    qDebug()<<"select * from redpack where `mid`='"+mid+"' ";
    QHash<QString,QString> garbList;
    QString from;
    int total;
    int num;
    if(query.next()){
        from = query.value(query.record().indexOf("fromb")).toString();
        total = query.value(query.record().indexOf("total")).toInt();
        num = query.value(query.record().indexOf("num")).toInt();
        qDebug()<<from;
        garbList.insert("1",query.value(query.record().indexOf("n1")).toString());
        garbList.insert("2",query.value(query.record().indexOf("n2")).toString());
        garbList.insert("3",query.value(query.record().indexOf("n3")).toString());
        garbList.insert("4",query.value(query.record().indexOf("n4")).toString());
        garbList.insert("5",query.value(query.record().indexOf("n5")).toString());
        garbList.insert("6",query.value(query.record().indexOf("n6")).toString());
    }else{
        return;
    }
    int garbed = 0;
    bool cant = 0;
    QString garbedIndex;
    for(int i=1;i<=6;i++){
        qDebug()<<QString::number(i);
        if(garbList[QString::number(i)].contains(userName)){
            cant=1;
            break;
        }
        if(garbList[QString::number(i)]!="0"&& !garbList[QString::number(i)].contains("garbed.")){
            garbed = garbList[QString::number(i)].toInt();
            garbedIndex = "n"+QString::number(i);
            break;
        }
    }
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","garbResult");
    if(garbed!=0){
        query.exec( "UPDATE `redpack` SET `"+garbedIndex+"` = 'garbed."+userName+"' WHERE `redpack`.`mid` = '"+mid+"'");
        msgData.insert("status","1");
        msgData.insert("from",from);
        msgData.insert("garbed",garbed);
    }
    if(garbed==0&&cant){
        msgData.insert("status","2");
        msgData.insert("from",from);
    }
    if(garbed==0&&cant==0){
        msgData.insert("status","3");
        msgData.insert("from",from);
    }
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();

}

void User::Init(QJsonArray onlineUser){
    QJsonObject newMessage;
    QJsonArray allUser;
    newMessage.insert("online",onlineUser);
    QSqlQuery query(*db);
    query.exec( "select * from users");
    while(query.next()){
        QJsonObject userInfo;
        userInfo.insert("uid",query.value(query.record().indexOf("id")).toInt());
        userInfo.insert("username",query.value(query.record().indexOf("username")).toString());
        userInfo.insert("email",query.value(query.record().indexOf("email")).toString());
        userInfo.insert("avatar",query.value(query.record().indexOf("avatar")).toInt());
        allUser.append(userInfo);
    }
    newMessage.insert("all",allUser);
    newMessage.insert("grade",grade);
    QJsonObject message;
    message.insert("action","updateList");
    message.insert("data",newMessage);
    QJsonDocument document;
    document.setObject(message);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson);
    socket->flush();

    //发送未读离线消息
    QSqlQuery queryq(*db);
    queryq.exec( "select * from message where `tob` = '"+userName+"'");
    qDebug()<<"select * from message where `tob` = '"+userName+"'";
    while(queryq.next()){
        QString msgType = queryq.value(queryq.record().indexOf("type")).toString();
        if(msgType=="text"){
            sendTextMsg(queryq.value(queryq.record().indexOf("body")).toString(),queryq.value(queryq.record().indexOf("fromb")).toString(),queryq.value(queryq.record().indexOf("avatarName")).toString(),queryq.value(queryq.record().indexOf("timeString")).toString());
            query.exec("DELETE FROM `message` WHERE `id`='"+queryq.value(queryq.record().indexOf("id")).toString()+"'");
        }else if(msgType=="img"){
            sendImg(queryq.value(queryq.record().indexOf("body")).toString(),queryq.value(queryq.record().indexOf("fromb")).toString(),queryq.value(queryq.record().indexOf("avatarName")).toString(),queryq.value(queryq.record().indexOf("timeString")).toString());
            query.exec("DELETE FROM `message` WHERE `id`='"+queryq.value(queryq.record().indexOf("id")).toString()+"'");
        }else if(msgType=="file"){
            sendFile(queryq.value(queryq.record().indexOf("fileName")).toString(),queryq.value(queryq.record().indexOf("body")).toString(),queryq.value(queryq.record().indexOf("fromb")).toString(),queryq.value(queryq.record().indexOf("total")).toInt(),queryq.value(queryq.record().indexOf("avatarName")).toString(),queryq.value(queryq.record().indexOf("timeStrings")).toString());
            query.exec("DELETE FROM `message` WHERE `id`='"+queryq.value(queryq.record().indexOf("id")).toString()+"'");
        }else if(msgType=="redpacknormal"){
            sendRedPack(queryq.value(queryq.record().indexOf("mid")).toString(), queryq.value(queryq.record().indexOf("fromb")).toString(), queryq.value(queryq.record().indexOf("avatarName")).toString(), queryq.value(queryq.record().indexOf("timeStrings")).toString());
            query.exec("DELETE FROM `message` WHERE `id`='"+queryq.value(queryq.record().indexOf("id")).toString()+"'");
        }
    }
}

void User::initRedPack(QJsonObject msgData){
    QSqlQuery query(*db);
    QString sql;
    if(msgData.value("packNum").toInt()==1){
        sql = "INSERT INTO `redpack` (`id`, `mid`, `fromb`, `tob`, `total`, `num`, `n1`, `n2`, `n3`, `n4`, `n5`, `n6`) VALUES (NULL, '"+msgData.value("mid").toString()+"', '"+msgData.value("from").toString()+"', '"+msgData.value("to").toString()+"', '"+QString::number(msgData.value("packTotal").toInt())+"', '"+QString::number(msgData.value("packNum").toInt())+"', '"+QString::number(msgData.value("packTotal").toInt())+"', '0', '0', '0', '0', '0');";
    }else{
        int packNum = msgData.value("packNum").toInt();
        int total = msgData.value("packTotal").toInt();
        QHash<int,QString> numList;
        numList[1]="0";
        numList[2]="0";
        numList[3]="0";
        numList[4]="0";
        numList[5]="0";
        numList[6]="0";
        for(int i=1;i<=packNum;i++){
            QTime time;
            time= QTime::currentTime();
            qsrand(time.msec()+time.second()*1000);
            int n = (qrand() % (total-(packNum-i)-1))+1;
            total = total-n;
            numList[i] = QString::number(n);
        }
         sql = "INSERT INTO `redpack` (`id`, `mid`, `fromb`, `tob`, `total`, `num`, `n1`, `n2`, `n3`, `n4`, `n5`, `n6`) VALUES (NULL, '"+msgData.value("mid").toString()+"', '"+msgData.value("from").toString()+"', '"+msgData.value("to").toString()+"', '"+QString::number(msgData.value("packTotal").toInt())+"', '"+QString::number(msgData.value("packNum").toInt())+"', '"+numList[1]+"', '"+numList[2]+"', '"+numList[3]+"', '"+numList[4]+"', '"+numList[5]+"', '"+numList[6]+"');";
    }
    query.exec(sql);
}

void User::saveTextMsg(QJsonObject msgData){
    QSqlQuery query(*db);
    QString savesql = QString("INSERT INTO message(`id`, `type`, `avatar`, `body`, `fromb`, `tob`, `avatarName`, `timeString`, `sended`, `total`, `fileName`, `mid`)");
    savesql += QString(" VALUES(NULL,'%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11')")
            .arg("text")
            .arg("")
            .arg(msgData.value("body").toString())
            .arg(msgData.value("from").toString())
            .arg(msgData.value("to").toString())
            .arg(msgData.value("from").toString())
            .arg(QTime::currentTime().toString("hh:mm"))
            .arg(0)
            .arg(0)
            .arg("0")
            .arg(msgData.value("mid").toString());
    bool ok=query.exec(savesql);
    if(ok){
         //return 1;
    }else{
        //errMsg = "插入失败";
        //return 0;
    }
}

void User::saveImgMsg(QJsonObject msgData){
    QSqlQuery query(*db);
    QString savesql = QString("INSERT INTO message(`id`, `type`, `avatar`, `body`, `fromb`, `tob`, `avatarName`, `timeString`, `sended`, `total`, `fileName`, `mid`)");
    savesql += QString(" VALUES(NULL,'%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11')")
            .arg("img")
            .arg("")
            .arg(msgData.value("body").toString())
            .arg(msgData.value("from").toString())
            .arg(msgData.value("to").toString())
            .arg(msgData.value("from").toString())
            .arg(QTime::currentTime().toString("hh:mm"))
            .arg(0)
            .arg(0)
            .arg("0")
            .arg(msgData.value("mid").toString());
    bool ok=query.exec(savesql);
    if(ok){
         //return 1;
    }else{
        //errMsg = "插入失败";
        //return 0;
    }
}

void User::saveFileMsg(QJsonObject msgData){
    QSqlQuery query(*db);
    QString savesql = QString("INSERT INTO message(`id`, `type`, `avatar`, `body`, `fromb`, `tob`, `avatarName`, `timeString`, `sended`, `total`, `fileName`, `mid`)");
    savesql += QString(" VALUES(NULL,'%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11')")
            .arg("file")
            .arg("")
            .arg(msgData.value("filename").toString())
            .arg(msgData.value("from").toString())
            .arg(msgData.value("to").toString())
            .arg(msgData.value("from").toString())
            .arg(QTime::currentTime().toString("hh:mm"))
            .arg(0)
            .arg(msgData.value("size").toInt())
            .arg(file->fileName())
            .arg(msgData.value("mid").toString());
    bool ok=query.exec(savesql);
    if(ok){
         //return 1;
    }else{
        //errMsg = "插入失败";
        //return 0;
    }
}

void User::addGrade(int g){
    grade = grade + g;
    QSqlQuery query(*db);
    query.exec("UPDATE `users` SET `grade` = "+QString::number(grade)+" WHERE `users`.`username` = '"+userName+"'");
}

void User::decreaseGrade(int g){
    grade = grade - g;
    QSqlQuery query(*db);
    query.exec("UPDATE `users` SET `grade` = "+QString::number(grade)+" WHERE `users`.`username` = '"+userName+"'");
}

void User::changeProfile(QJsonObject msgData){
    QSqlQuery query(*db);
    query.exec("UPDATE `users` SET `username` = '"+msgData.value("new").toString()+"' WHERE `users`.`username` = '"+msgData.value("origin").toString()+"'");
    QString md5;
    QString token;
    QString sign = msgData.value("pwd").toString();
    QByteArray bytePwd = sign.toLatin1();
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    md5.append(bytePwdMd5.toHex());
    bytePwd = (md5+msgData.value("new").toString()).toLatin1();
    qDebug()<<md5+msgData.value("new").toString();
    bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    token.append(bytePwdMd5.toHex());
    query.exec("UPDATE `users` SET `sign` = '"+token+"' WHERE `users`.`username` = '"+msgData.value("new").toString()+"'");
    query.exec("UPDATE `users` SET `password` = '"+md5+"' WHERE `users`.`username` = '"+msgData.value("new").toString()+"'");

}

void User::withDrawOfflineMsg(QString mid){
    QSqlQuery query(*db);
    query.exec("DELETE FROM `message` WHERE `mid` ='"+mid+"'");
}

void User::sendTextMsg(QString msg,QString from,QString mid,QString avatarUser,QString stime){
    QJsonObject newMessage;
    QJsonObject msgData;
    QTime  time = QTime::currentTime();
    newMessage.insert("action","newTextMsg");
    msgData.insert("body",msg);
    msgData.insert("body",msg);
    msgData.insert("from",from);
    msgData.insert("mid",mid);
    msgData.insert("avatarname",avatarUser=="self"?from:avatarUser);
    msgData.insert("time",stime=="no"? time.toString("hh:mm") : stime);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
}

void User::saveRedPackMsg(QJsonObject msgData){
    QSqlQuery query(*db);
    QString savesql = QString("INSERT INTO message(`id`, `type`, `avatar`, `body`, `fromb`, `tob`, `avatarName`, `timeString`, `sended`, `total`, `fileName`, `mid`)");
    savesql += QString(" VALUES(NULL,'%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11')")
            .arg("redpacknormal")
            .arg("")
            .arg("")
            .arg(msgData.value("from").toString())
            .arg(msgData.value("to").toString())
            .arg(msgData.value("from").toString())
            .arg(QTime::currentTime().toString("hh:mm"))
            .arg(0)
            .arg(0)
            .arg("0")
            .arg(msgData.value("mid").toString());
    bool ok=query.exec(savesql);
    if(ok){
         //return 1;
    }else{
        //errMsg = "插入失败";
        //return 0;
    }
}
void User::sendRedPack(QString mid, QString from, QString avatarUser, QString stime)
{
    QJsonObject newMessage;
    QJsonObject msgData;
    QTime  time = QTime::currentTime();
    newMessage.insert("action","newRedPack");
    msgData.insert("from",from);
    msgData.insert("mid",mid);
    msgData.insert("avatarname",avatarUser=="self"?from:avatarUser);
    msgData.insert("time",stime=="no"? time.toString("hh:mm") : stime);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
}
void User::sendImg(QString msg,QString from,QString mid,QString avatarUser,QString stime){
    QJsonObject newMessage;
    QJsonObject msgData;
    QTime  time = QTime::currentTime();
    newMessage.insert("action","newImg");
    msgData.insert("body",msg);
    msgData.insert("from",from);
    msgData.insert("mid",mid);
    msgData.insert("avatarname",avatarUser=="self"?from:avatarUser);
    msgData.insert("time",stime=="no"? time.toString("hh:mm") : stime);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
}

void User::sendSpeak(int sec,QString msg,QString from,QString mid,QString avatarUser,QString stime){
    QJsonObject newMessage;
    QJsonObject msgData;
    QTime  time = QTime::currentTime();
    newMessage.insert("action","newSpeak");
    msgData.insert("body",msg);
    msgData.insert("from",from);
    msgData.insert("mid",mid);
    msgData.insert("sec",sec);
    msgData.insert("avatarname",avatarUser=="self"?from:avatarUser);
    msgData.insert("time",stime=="no"? time.toString("hh:mm") : stime);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
}

void User::sendFileData(QString name){
    QFileInfo fileinfo = QFileInfo(name);
    QByteArray outBlock;
    QFile fileObj(name);
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
        QMutex mutex;
        QWaitCondition sleep;
        mutex.lock();
        sleep.wait(&mutex, 500);
        mutex.unlock();
    }
    qDebug()<<socket->errorString();
    socket->sendBinaryMessage(QString("ok,finished52121225").toUtf8());
    socket->flush();
}

void User::sendShake(QString from){
    QJsonObject newMessage;
    QJsonObject msgData;
    QTime  time = QTime::currentTime();
    newMessage.insert("action","newShake");
    msgData.insert("from",from);
    msgData.insert("avatarname",from);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
}

void User::sendFile(QString realName,QString fileName,QString from,qint64 size,QString mid ,QString avatarUser,QString stime){
    QJsonObject newMessage;
    QJsonObject msgData;
    QTime  time = QTime::currentTime();
    newMessage.insert("action","newFile");
    msgData.insert("realname",realName);
    msgData.insert("from",from);
    msgData.insert("mid",mid);
    msgData.insert("time",stime=="no"? time.toString("hh:mm") : stime);
    msgData.insert("size",size);
    msgData.insert("avatarname",avatarUser=="self"?from:avatarUser);
    msgData.insert("filename",fileName);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
}

void User::sendBroadcast(int status,QString user){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","broadcast");
    msgData.insert("username",user);
    msgData.insert("status",status);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
}

void User::newGroup(QString groupname){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","newGroup");
    msgData.insert("groupname",groupname);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}

void User::withDraw(QString mid, QString um){
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","withDraw");
    msgData.insert("mid",mid);
    msgData.insert("from",um);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}
