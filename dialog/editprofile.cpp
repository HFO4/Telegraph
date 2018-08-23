#include "editprofile.h"
#include "ui_editprofile.h"

EditProfile::EditProfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditProfile)
{
    ui->setupUi(this);
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
