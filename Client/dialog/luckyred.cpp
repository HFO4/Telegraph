#include "luckyred.h"
#include "ui_luckyred.h"

#include <QPushButton>
#include <QDebug>
#pragma execution_character_set("utf-8")
LuckyRed::LuckyRed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LuckyRed)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
}

LuckyRed::~LuckyRed()
{
    delete ui;
}

void LuckyRed::Init(int g){
    gg = g;
    ui->label_5->setText(QString::number(gg));
}
int LuckyRed::getGrade(){
    return ui->lineEdit->text().toInt();
}
int LuckyRed::getNum(){
    return ui->lineEdit_2->text().toInt();
}
