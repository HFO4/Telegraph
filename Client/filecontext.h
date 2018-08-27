#ifndef FILECONTEXT_H
#define FILECONTEXT_H

#include <QObject>

class FileContext : public QObject
{
    Q_OBJECT
public:
     FileContext();

signals:
    void doDownload(QString,QString,qint64);
    void withDrawMsg(QString);
    void garb(QString);
    void play(QString);
public slots:
    void Download(QString,QString,qint64);
    void Open(QString);
    void withDraw(QString);
    void Garb(QString);
    void playSpeak(QString);
};

#endif // FILECONTEXT_H
