#ifndef NORMALRED_H
#define NORMALRED_H

#include <QDialog>

namespace Ui {
class NormalRed;
}

class NormalRed : public QDialog
{
    Q_OBJECT

public:
    int grade;
    int gg;
    void Init(int);
    int getGrade();
    explicit NormalRed(QWidget *parent = 0);
    ~NormalRed();

private slots:
    void on_buttonBox_accepted();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::NormalRed *ui;
};

#endif // NORMALRED_H
