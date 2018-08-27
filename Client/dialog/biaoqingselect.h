#ifndef BIAOQINGSELECT_H
#define BIAOQINGSELECT_H

#include <QDialog>

namespace Ui {
class BiaoqingSelect;
}

class BiaoqingSelect : public QDialog
{
    Q_OBJECT

public:
    explicit BiaoqingSelect(QWidget *parent = 0);
    void initEmotion();
    QHash<int,QHash<int,QString > > imageList;
    void addEmotionItem(QString fileName);
    QList<QString> m_emotionList;
    ~BiaoqingSelect();
signals:
    void sendBqb(QString);

private slots:
    void on_tableWidget_clicked(const QModelIndex &index);

    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::BiaoqingSelect *ui;
    bool event(QEvent *event);
};

#endif // BIAOQINGSELECT_H
