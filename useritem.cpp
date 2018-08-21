#include "useritem.h"
#include "ui_useritem.h"

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
    ui->label->setStyleSheet("border-image: url(:/avatar/"+avatar+".jpg);border-radius:32;");
}
void UserItem::addUnreadMessage(){
    ui->label_4->show();
    unread++;
    ui->label_4->setText(QString::number(unread));
    ui->label_4->adjustSize();
}
void UserItem::readAll(){
    unread=0;
    ui->label_4->hide();

}
UserItem::~UserItem()
{
    delete ui;
}
