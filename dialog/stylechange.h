#ifndef STYLECHANGE_H
#define STYLECHANGE_H

#include <QDialog>

namespace Ui {
class StyleChange;
}

class StyleChange : public QDialog
{
    Q_OBJECT

public:
    explicit StyleChange(QWidget *parent = 0);
    ~StyleChange();
    QString selfback = "#eceff1";
    QString selftext = "#1a1a1a";
    QString youback = "#00b0ff";
    QString youtext = "#fff";
signals:
    void changeStyle(QString,QString,QString,QString);
    void changeBackground(QString);
    void changeBackColor(QString);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::StyleChange *ui;
};

#endif // STYLECHANGE_H
