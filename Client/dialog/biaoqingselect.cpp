#include "biaoqingselect.h"
#include "ui_biaoqingselect.h"

#include <QLabel>
#include <QMovie>
#include <QDebug>

BiaoqingSelect::BiaoqingSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BiaoqingSelect)
{
    ui->setupUi(this);
   // this->setAttribute(Qt::WA_ShowWithoutActivating);
}

BiaoqingSelect::~BiaoqingSelect()
{
    delete ui;
}

void BiaoqingSelect::initEmotion(){
    QString path = "biaoqing/1 (%1).jpg";
        for (int i = 0; i <= 64; i++)
        {
            addEmotionItem(path.arg(i + 1));
        }
}

void BiaoqingSelect::addEmotionItem(QString fileName){
    // 获取当前添加到第几行第几列
       int row = m_emotionList.size() / ui->tableWidget->rowCount();
       int column = m_emotionList.size() % ui->tableWidget->rowCount();
       imageList[row][column] = fileName;
       QTableWidgetItem* tableWidgetItem = new QTableWidgetItem;
       ui->tableWidget->setItem(row, column, tableWidgetItem);

       // 因为表情大部分为gif图片格式，所以这里用QMovie来显示
       QLabel* emotionIcon = new QLabel;
       emotionIcon->setMargin(4);
       QMovie* movie = new QMovie;
       movie->setScaledSize(QSize(64, 64));
       movie->setFileName(fileName);
       movie->start();
       emotionIcon->setMovie(movie);
       ui->tableWidget->setCellWidget(row, column, emotionIcon);
       m_emotionList.push_back(fileName);
}

bool BiaoqingSelect::event ( QEvent * event )
{
   if (event->type() == QEvent::ActivationChange)
   {
        if(QApplication::activeWindow() != this)
        {
             this->hide();
        }
   }
   return QWidget::event(event);
}

void BiaoqingSelect::on_tableWidget_clicked(const QModelIndex &index)
{

}

void BiaoqingSelect::on_tableWidget_cellClicked(int row, int column)
{
    emit sendBqb(imageList[row][column]);
    this->close();

}
