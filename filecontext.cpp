#include "filecontext.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
FileContext::FileContext()
{

}

void FileContext::Download(QString realname,QString filename ,qint64 size){
    emit doDownload(realname,filename,size);
}

void FileContext::Open(QString name){
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("tmp/"+name).absoluteFilePath()));
}
