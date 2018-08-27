#ifndef PURCHASE_H
#define PURCHASE_H

#include <QWidget>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QBuffer>
#include <QImage>
#include <QPixmap>
#include <QTimer>
namespace Ui {
class Purchase;
}

class Purchase : public QWidget
{
    Q_OBJECT

public:
    explicit Purchase(QWidget *parent = 0);
    void init (int num,QString username);
    void loadQRCode();
    void dropOrder();
    ~Purchase();
signals:
    void addGrade(int);
private slots:
    void firstFinished(QNetworkReply*);
    void callbakc(QNetworkReply*);
    void queryStatus();
private:
    Ui::Purchase *ui;
    QString userName;
    QString price;
    QNetworkAccessManager* req;
    QNetworkAccessManager* callback;
    int sended=0;
    QString token;
    int grade;
    QTimer* timer;
    QString qrId;
    QPixmap  Base64_To_Image(QByteArray bytearray,QString SavePath);
    bool first=1;
    bool query=0;
};

#endif // PURCHASE_H
