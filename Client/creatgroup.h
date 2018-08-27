#ifndef CREATGROUP_H
#define CREATGROUP_H

#include <QDialog>
#include <QHash>
#include <QList>

namespace Ui {
class CreatGroup;
}

class CreatGroup : public QDialog
{
    Q_OBJECT

public:
    explicit CreatGroup(QWidget *parent = 0);
    QList<QString> selected;
    QString groupName;
    void Init(QHash<QString,QString>,QString);
    ~CreatGroup();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_buttonBox_accepted();

private:
    Ui::CreatGroup *ui;
};

#endif // CREATGROUP_H
