#include "useritem.h"
#include "ui_useritem.h"

UserItem::UserItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
}
void UserItem::setUserInfo(QString username, QString des, QString avatar){
    ui->label_2->setText(username);
    userName = username;
    ui->label_3->setText(des);
    ui->label->setStyleSheet("border-image: url(:/avatar/"+avatar+".jpg);border-radius:32;");
}
UserItem::~UserItem()
{
    delete ui;
}
