#ifndef HOME_H
#define HOME_H

#include "messagethread.h"
#include "message.h"
#include "dialog.h"
#include "purchase.h"

#include <QWidget>
#include <QThread>
#include <QHash>
#include <QList>
#include <QJsonObject>
#include <QtWebEngineWidgets/QWebEngineView>
namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    QString Token;
    QString userName;
    explicit Home(QWidget *parent = 0);
    ~Home();
    void startListen();
public slots:
    void updateList(QJsonObject list);
    void newMsg(Message*);

private:
    Ui::Home *ui;
    QThread* thread;
    MessageThread* messageThread;
    Purchase* purchase;
    QString currentUser;
    void changeSelected();
    QWebEngineView* chat;
    QHash<QString,QString> usernameAvatar;
    QHash<QString,QString> usernameEmail;
    QHash<QString,QList<Message> > messageList;
signals:
    void startThread();
    void sendMsg(QString,QString,QString,QString);
    void sendImg(QString,QString,QString,QString);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_9_clicked();
};

#endif // HOME_H
