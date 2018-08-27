#include "recordshow.h"
#include "ui_recordshow.h"

#include <QPainter>

RecordShow::RecordShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordShow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_ShowWithoutActivating);
    this->setWindowFlags(Qt::FramelessWindowHint);
    QPalette palette;
    palette.setColor(QPalette::Window,Qt::black);
    this->setPalette(palette);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

RecordShow::~RecordShow()
{
    delete ui;
}
