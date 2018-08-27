#ifndef GARBRESULT_H
#define GARBRESULT_H

#include <QDialog>
#include<QJsonObject>

namespace Ui {
class GarbResult;
}

class GarbResult : public QDialog
{
    Q_OBJECT

public:
    explicit GarbResult(QWidget *parent = 0);
    void Init(QJsonObject msg);
    ~GarbResult();

private:
    Ui::GarbResult *ui;
};

#endif // GARBRESULT_H
