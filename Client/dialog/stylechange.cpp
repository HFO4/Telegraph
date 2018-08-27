#include "stylechange.h"
#include "ui_stylechange.h"

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#pragma execution_character_set("utf-8")
StyleChange::StyleChange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StyleChange)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
}

StyleChange::~StyleChange()
{
    delete ui;
}

void StyleChange::on_pushButton_clicked()
{
    QColor color = QColorDialog::getColor(Qt::black);

    if (color.isValid())
    {
        selfback = color.name();
        ui->selfback->setStyleSheet("background-color:"+selfback);
        emit changeStyle(selfback,selftext,youback,youtext);
    }
}

void StyleChange::on_pushButton_2_clicked()
{
    QColor color = QColorDialog::getColor(Qt::black);

    if (color.isValid())
    {
        selftext = color.name();
        ui->selftext->setStyleSheet("background-color:"+selftext);
        emit changeStyle(selfback,selftext,youback,youtext);
    }
}

void StyleChange::on_pushButton_3_clicked()
{
    QColor color = QColorDialog::getColor(Qt::black);

    if (color.isValid())
    {
        youback = color.name();
        ui->youback->setStyleSheet("background-color:"+youback);
        emit changeStyle(selfback,selftext,youback,youtext);
    }
}

void StyleChange::on_pushButton_4_clicked()
{
    QColor color = QColorDialog::getColor(Qt::black);

    if (color.isValid())
    {
        youtext = color.name();
        ui->youtext->setStyleSheet("background-color:"+youtext);
        emit changeStyle(selfback,selftext,youback,youtext);
    }
}

void StyleChange::on_pushButton_5_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,"选择图片","图片","*.png *.jpg *.gif *.jpeg *.bmp");
    if(!file_name.isNull()){
        ui->label_5->setText(file_name);
        ui->label_5->setWordWrap(true);
        ui->label_5->setAlignment(Qt::AlignTop);
        emit changeBackground(file_name);
    }
}

void StyleChange::on_pushButton_6_clicked()
{
    QColor color = QColorDialog::getColor(Qt::black);

    if (color.isValid())
    {
        ui->back->setStyleSheet("background-color:"+color.name());
        emit changeBackColor(color.name());
    }
}
