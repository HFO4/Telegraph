#include "normalred.h"
#include "ui_normalred.h"

#include <QPushButton>
#include <QDebug>
#pragma execution_character_set("utf-8")
NormalRed::NormalRed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NormalRed)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
}

NormalRed::~NormalRed()
{
    delete ui;
}

void NormalRed::Init(int g){
    gg = g;
    ui->label_4->setText(QString::number(gg));
}

void NormalRed::on_buttonBox_accepted()
{

}

void NormalRed::on_lineEdit_textChanged(const QString &arg1)
{

}

int NormalRed::getGrade(){
    return ui->lineEdit->text().toInt();
}
