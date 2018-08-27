#ifndef LOGINTHREAD_H
#define LOGINTHREAD_H

#include <QThread>
#include <QWebSocket>

class LoginThread : public QObject
{
    Q_OBJECT
public:
    LoginThread();
    QString username;
    QString password;
    QString errMsg;
protected:
    void run();
private:
    QWebSocket *socket;
public:
    void socket_Read_Data(QString buffer);
    void socket_Disconnected();
    void socket_Connected();
    void startConnect();
signals:
    void loginSuccess();
    void loginFailed();
public slots:
};

#endif // LOGINTHREAD_H
