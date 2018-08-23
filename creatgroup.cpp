#include "creatgroup.h"
#include "ui_creatgroup.h"

#include <QDebug>

CreatGroup::CreatGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatGroup)
{
    ui->setupUi(this);
}

CreatGroup::~CreatGroup()
{
    delete ui;
}

void CreatGroup::Init(QHash<QString, QString> userList ,QString currentUser){
    QHash<QString, QString>::const_iterator i;
    for(i=userList.constBegin(); i!=userList.constEnd(); ++i){
        if(i.key()!=currentUser){
            ui->listWidget->addItem(i.key());
        }

    }


}

void CreatGroup::on_pushButton_clicked()
{
    if(ui->listWidget->currentItem()!=NULL){
        ui->listWidget_2->addItem(ui->listWidget->currentItem()->text());
        selected.append(ui->listWidget->currentItem()->text());
        QListWidgetItem* user = ui->listWidget->currentItem();
        ui->listWidget->removeItemWidget(user);
        delete user;
    }

}

void CreatGroup::on_pushButton_2_clicked()
{
    if(ui->listWidget_2->currentItem()!=NULL){
        ui->listWidget->addItem(ui->listWidget_2->currentItem()->text());
        selected.removeOne(ui->listWidget->currentItem()->text());
        QListWidgetItem* user = ui->listWidget_2->currentItem();
        ui->listWidget_2->removeItemWidget(user);
        delete user;
    }
}

void CreatGroup::on_buttonBox_accepted()
{
    qDebug()<<"newGroup-1";
    groupName = ui->lineEdit->text();
}
