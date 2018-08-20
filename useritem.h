#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>

namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT

public:
    explicit UserItem(QWidget *parent = 0);
    void setUserInfo(QString username,QString des,QString avatar);
    ~UserItem();
    QString userName;

private:
    Ui::UserItem *ui;
};

#endif // USERITEM_H
