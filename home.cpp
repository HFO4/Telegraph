#include "home.h"
#include "ui_home.h"
#include "useritem.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QListWidgetItem>
#include <QList>
#pragma execution_character_set("utf-8")
Home::Home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home)
{
    ui->setupUi(this);
    ui->splitter_2->setStretchFactor(0, 5);
    ui->splitter_2->setStretchFactor(1, 5);
    ui->splitter->setStretchFactor(0, 8);
    ui->splitter->setStretchFactor(1, 2);
    chat = new QWebEngineView(this);
    chat->load(QUrl("qrc:/html/chat/chat.html"));
    chat->setContextMenuPolicy(Qt::NoContextMenu);
    ui->verticalLayout_2->addWidget(chat);
    connect(ui->listWidget,&QListWidget::itemSelectionChanged,this,&Home::changeSelected);
}
void::Home::changeSelected(){
    UserItem* currentItem = (UserItem*)ui->listWidget->itemWidget(ui->listWidget->currentItem());
    currentUser = currentItem->userName;
    chat->page()->runJavaScript("Clear()");
    if(!messageList.contains(currentUser)){
        chat->page()->runJavaScript("Empty()");
    }else{
        QListIterator<Message> i(messageList[currentUser]);
        while (i.hasNext()) {
            Message msg = i.next();
            if(msg.selfsend){
                chat->page()->runJavaScript("addSelfTextMsg('"+msg.body+"','"+msg.avatar+"')");
            }else{
                if(currentUser == msg.from){
                    chat->page()->runJavaScript("addTextMsg('"+msg.body+"','"+usernameAvatar[msg.from]+"')");
                }else{

                }
            }
        }
    }

}
void Home::startListen(){
    thread = new QThread(this);
    messageThread = new MessageThread();
    messageThread->Token = Token;
    messageThread->userName = userName;
    messageThread->moveToThread(thread);
    connect(this,&Home::startThread,messageThread,&MessageThread::Start);
    connect(messageThread,SIGNAL(updateList(QJsonObject)),this,SLOT(updateList(QJsonObject)));
    connect(messageThread,SIGNAL(newMsg(Message*)),this,SLOT(newMsg(Message*)));
    emit startThread();
    thread->start();
    connect(this,SIGNAL(sendMsg(QString,QString,QString,QString)),messageThread,SLOT(sendMsg(QString,QString,QString,QString)));
}
void Home::updateList(QJsonObject list){
    qDebug()<<"COPY";
    QList<int> added;
    usernameAvatar.clear();
    if(list.contains("online")){
        QJsonValue value = list.value("online");
        QJsonArray onlineList = value.toArray();
        int nSize = onlineList.size();
        for (int i = 0; i < nSize; ++i) {
            QJsonValue onlines = onlineList.at(i);
            QJsonObject singleUser = onlines.toObject();
            UserItem* user = new UserItem();
            QListWidgetItem *listItem =  new QListWidgetItem();
            listItem->setSizeHint(QSize(220,80));
            ui->listWidget->addItem(listItem);
            user->setUserInfo(singleUser.value("username").toString(),"在线",QString::number(singleUser.value("avatar").toInt(), 10));
            ui->listWidget->setItemWidget(listItem,user);
            added.append(singleUser.value("uid").toInt());
            usernameAvatar[singleUser.value("username").toString()]=QString::number(singleUser.value("avatar").toInt(), 10);
            //qDebug()<<singleUser.value("username").toString();
        }
    }

    if(list.contains("all")){
        QJsonValue valueOff = list.value("all");
        QJsonArray offLineList = valueOff.toArray();
        int nSizeOff = offLineList.size();
        for (int i = 0; i < nSizeOff; ++i) {
            QJsonValue offlines = offLineList.at(i);
            QJsonObject singleUserOff = offlines.toObject();
            if(!added.contains(singleUserOff.value("uid").toInt())){
                UserItem* user = new UserItem();
                QListWidgetItem *listItem =  new QListWidgetItem();
                listItem->setSizeHint(QSize(220,80));
                ui->listWidget->addItem(listItem);
                user->setUserInfo(singleUserOff.value("username").toString(),"离线",QString::number(singleUserOff.value("avatar").toInt(), 10)+"b");
                ui->listWidget->setItemWidget(listItem,user);
                usernameAvatar[singleUserOff.value("username").toString()]=QString::number(singleUserOff.value("avatar").toInt(), 10)+"b";
            }
        }
    }
}

void Home::newMsg(Message * msg){
    if(msg->selfsend){
        chat->page()->runJavaScript("addSelfTextMsg('"+msg->body+"','"+msg->avatar+"')");
    }else{
        if(currentUser == msg->from){
            qDebug()<<"new Msg";
            chat->page()->runJavaScript("addTextMsg('"+msg->body+"','"+usernameAvatar[msg->from]+"')");
        }else{

        }
    }
    QString session = msg->selfsend ? msg->to : msg->from;
    messageList[session].append(*msg);
    delete msg;

}

Home::~Home()
{
    delete ui;
    thread->quit();
    thread->wait();
    delete thread;
    delete messageThread;
}

void Home::on_pushButton_clicked()
{
    QString msgBody = ui->textEdit_2->toPlainText();
    Message *msg = new Message("text",msgBody,userName,currentUser,1);
    msg->avatar = usernameAvatar[userName];
    newMsg(msg);
    emit sendMsg(currentUser,msgBody,"text",userName);
}

void Home::on_pushButton_2_clicked()
{
    Dialog* dlg = new Dialog;
    dlg->setWindowTitle("充值");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    if(dlg->exec()==QDialog::Accepted){
        purchase = new Purchase();
        purchase->init(dlg->num,userName);
        purchase->show();
        purchase->loadQRCode();
    }else{

    }
}
