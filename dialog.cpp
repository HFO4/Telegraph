#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_lineEdit_textChanged(const QString &arg1)
{
    int num2 = arg1.toInt();
    double price = num2*0.1;
    ui->label_3->setText(QString::number(price));
}

void Dialog::on_buttonBox_accepted()
{
    if(ui->lineEdit->text().toInt() <=1){
        num=1;
    }
    num = ui->lineEdit->text().toInt();
}

