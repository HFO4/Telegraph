#ifndef HOME_H
#define HOME_H

#include "messagethread.h"
#include "message.h"
#include "dialog.h"
#include "purchase.h"
#include "useritem.h"
#include "creatgroup.h"
#include "filecontext.h"
#include "dialog/editprofile.h"
#include "dialog/stylechange.h"
#include "dialog/normalred.h"
#include "dialog/garbresult.h"
#include "dialog/luckyred.h"
#include "dialog/recordshow.h"
#include "dialog/biaoqingselect.h"

#include <QWidget>
#include <QThread>
#include <QHash>
#include <QList>
#include <QTime>
#include <QJsonObject>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QMenu>
#include <QAudioRecorder>
#include <QAudioEncoderSettings>
#include <QMediaPlayer>

namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    QString Token;
    QString userName;
    QAudioRecorder* audioRecorder;
    explicit Home(QWidget *parent = 0);
    ~Home();
    void startListen();
public slots:
    void updateList(QJsonObject list);
    void newMsg(Message*);
    void updateStatus(int,QString);
    void sendNewFile(QString,qint64,QString,QString);
    void setProgress(QString,qint64,qint64);
    void newJoinedGroup(QString);
    void backMsg(QString,QString);
    void offLine();
    void changeStyle(QString selfback,QString selftexr,QString youback,QString youtext);
    void changeBackground(QString);
    void changeBackColor(QString);
    void plusGrade(int);
    void inform(QString);
    void decreaseGrade(int);
    void doShake(QString);
    void chooseRedPack(QAction*);
    void showResult(QJsonObject);
    void updateProgress(qint64);
    void play(QString fname);
    void sendBqb(QString path);

private:
    Ui::Home *ui;
    RecordShow* recShow;
    QThread* thread;
    MessageThread* messageThread;
    Purchase* purchase;
    FileContext* webContext;
    QMenu* statusmenu;
    QAction *signleRedPack;
    QAction *muiltyRedPack;
    QString currentUser;
    int grade;
    void changeSelected();
    void insertTime(QTime);
    void keyPressEvent(QKeyEvent * event);
    QWebEngineView* chat;
    QHash<QString,QString> usernameAvatar;
    QHash<QString,QString> usernameEmail;
    QHash<QString, UserItem* > usernameItem;
    QHash<QString,QList<Message> > messageList;
    QHash<QString,QTime> userTime;
    QString randomString();
    QString recordSec;
    QMediaPlayer* player;
    QString recordName;
signals:
    void startThread();
    void sendMsg(QString,QString,QString,QString,QString);
    void sendImg(QString,QString,QString,QString);
    void sendFile(QString,QString,QString,QString);
    void creatGroup(QString,QString,QList<QString>);
    void changeProfile(QString,QString);
    void sendShake(QString);
    void sendRedPack(QString,int,int,QString,QString);
    void startRecord();
    void stopRecord(QString sec,QString to,QString from,QString selfAvatar);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_12_pressed();
    void on_pushButton_12_released();
};

#endif // HOME_H
