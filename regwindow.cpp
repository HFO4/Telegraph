#include "regwindow.h"
#include "ui_regwindow.h"

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#pragma execution_character_set("utf-8")
RegWindow::RegWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegWindow)
{
    ui->setupUi(this);
}

RegWindow::~RegWindow()
{
    delete ui;
}

void RegWindow::on_pushButton_clicked()
{
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    if(password != ui->lineEdit_3->text()){
        QMessageBox::warning(NULL, "错误", tr("两次密码输入不一致"));
        return;
    }
    if(username.isEmpty() || password.isEmpty()){
        QMessageBox::warning(NULL, "错误", tr("用户名或密码不能为空"));
        return;
    }
    ui->pushButton->setEnabled(false);
    socket = new QWebSocket();
    connect(socket, &QWebSocket::connected, this,&RegWindow::socket_Connected);
    connect(socket, &QWebSocket::disconnected, this, &RegWindow::socket_Disconnected);
    socket->open(QUrl("ws://localhost:8964"));
}

void RegWindow::socket_Read_Data(QString buffer){
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
                       QMessageBox::information(NULL, "提示", tr("注册成功"));
                       socket->close();
                       this->close();
                    }else{
                       QMessageBox::critical(NULL, "错误", object.value("msg").toString());
                       socket->close();
                    }
                    ui->pushButton->setEnabled(true);
                }
            }
        }
    }
}
void RegWindow::socket_Disconnected(){

}

void RegWindow::socket_Connected(){
    qDebug() << "Connect successfully!";
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    QString sex = ui->comboBox->currentText();
    QString email = ui->lineEdit_5->text();
    QString md5;
    QByteArray bytePwd = password.toLatin1();
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    md5.append(bytePwdMd5.toHex());
    connect(socket, &QWebSocket::textMessageReceived,
                this, &RegWindow::socket_Read_Data);
    //生成JSON字符串
    QJsonObject newMessage;
    QJsonObject msgData;
    newMessage.insert("action","register");
    msgData.insert("username",username);
    msgData.insert("password",md5);
    msgData.insert("sex",sex);
    msgData.insert("email",email);
    newMessage.insert("data",msgData);
    QJsonDocument document;
    document.setObject(newMessage);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    //发送数据
    socket->sendTextMessage(strJson.toUtf8());
    socket->flush();
}
