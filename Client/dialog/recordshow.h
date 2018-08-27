#ifndef RECORDSHOW_H
#define RECORDSHOW_H

#include <QWidget>

namespace Ui {
class RecordShow;
}

class RecordShow : public QWidget
{
    Q_OBJECT

public:
    explicit RecordShow(QWidget *parent = 0);
    ~RecordShow();

private:
    Ui::RecordShow *ui;
};

#endif // RECORDSHOW_H
