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
public slots:
    void Download(QString,QString,qint64);
    void Open(QString);
    void withDraw(QString);
};

#endif // FILECONTEXT_H
