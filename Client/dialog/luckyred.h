#ifndef LUCKYRED_H
#define LUCKYRED_H

#include <QDialog>

namespace Ui {
class LuckyRed;
}

class LuckyRed : public QDialog
{
    Q_OBJECT

public:
    int grade;
    int gg;
    void Init(int);
    int getGrade();
    int getNum();
    explicit LuckyRed(QWidget *parent = 0);
    ~LuckyRed();

private:
    Ui::LuckyRed *ui;
};

#endif // LUCKYRED_H
