#include "editprofile.h"
#include "ui_editprofile.h"
#include <QPushButton>

#pragma execution_character_set("utf-8")
EditProfile::EditProfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditProfile)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
}

EditProfile::~EditProfile()
{
    delete ui;
}

void EditProfile::Init(QString pwd){
    ui->lineEdit->setText(pwd);
}

void EditProfile::on_buttonBox_accepted()
{
    passWord=ui->lineEdit_2->text();
    userName = ui->lineEdit->text();
}
