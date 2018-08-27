#ifndef EDITPROFILE_H
#define EDITPROFILE_H

#include <QDialog>

namespace Ui {
class EditProfile;
}

class EditProfile : public QDialog
{
    Q_OBJECT

public:
    QString userName;
    QString passWord;
    void Init(QString pwd);
    explicit EditProfile(QWidget *parent = 0);
    ~EditProfile();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::EditProfile *ui;
};

#endif // EDITPROFILE_H
