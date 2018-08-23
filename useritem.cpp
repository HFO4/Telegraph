#include "useritem.h"
#include "ui_useritem.h"
#pragma execution_character_set("utf-8")
UserItem::UserItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
    ui->label_4->hide();
}
void UserItem::setUserInfo(QString username, QString des, QString avatar){
    ui->label_2->setText(username);
    userName = username;
    ui->label_3->setText(des);
    avatarId = avatar;
    ui->label->setStyleSheet("border-image: url(:/avatar/avatars/"+avatar+".jpg);border-radius:32;");
}
void UserItem::addUnreadMessage(){
    ui->label_4->show();
    unread++;
    ui->label_4->setText(QString::number(unread));
    ui->label_4->adjustSize();
}

void UserItem::setStatus(int status){
    if(status==1){
         ui->label->setStyleSheet("border-image: url(:/avatar/avatars/"+avatarId.replace("b","")+".jpg);border-radius:32;");
         ui->label_3->setText("在线");
    }else{
        ui->label->setStyleSheet("border-image: url(:/avatar/avatars/"+avatarId.replace("b","")+"b.jpg);border-radius:32;");
        ui->label_3->setText("离线");
    }
}

void UserItem::readAll(){
    unread=0;
    ui->label_4->hide();

}
UserItem::~UserItem()
{
    delete ui;
}
