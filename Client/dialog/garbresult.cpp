#include "garbresult.h"
#include "ui_garbresult.h"
#pragma execution_character_set("utf-8")
GarbResult::GarbResult(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GarbResult)
{
    ui->setupUi(this);
}

GarbResult::~GarbResult()
{
    delete ui;
}
void GarbResult::Init(QJsonObject msg){
    QPixmap image1;
    QPixmap image2;
    image1.load(":/image/smile.png");
    image2.load(":/image/sad.png");
    if(msg.value("status").toString()=="1"){
        ui->label_6->hide();
        ui->label_3->setText(QString::number(msg.value("garbed").toInt()));
        ui->label_7->setPixmap(image1);
    }else if(msg.value("status").toString()=="2"){
        ui->label_6->show();
        ui->label_3->hide();
        ui->label_2->hide();
        ui->label_4->hide();
         ui->label_6->setText("您已领取过此红包");
         ui->label_7->setPixmap(image2);
    }else if(msg.value("status").toString()=="3"){
        ui->label_6->show();
        ui->label_3->hide();
        ui->label_2->hide();
        ui->label_4->hide();
         ui->label_6->setText("红包被抢光了");
         ui->label_7->setPixmap(image2);
    }

    ui->label_5->setText("来自 "+msg.value("from").toString()+" 的红包");
}
