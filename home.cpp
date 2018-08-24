#include "home.h"
#include "ui_home.h"
#include "useritem.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QList>
#include <QWebChannel>
#include <QMetaType>
#include <QFileDialog>

#pragma execution_character_set("utf-8")
Home::Home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home)
{
    qRegisterMetaType< QList<QString> >("QList<QString>");
    ui->setupUi(this);
    ui->splitter_2->setStretchFactor(0, 5.2);
    ui->splitter_2->setStretchFactor(1, 4.8);
    ui->splitter->setStretchFactor(0, 8);
    ui->splitter->setStretchFactor(1, 2);
    chat = new QWebEngineView(this);
    chat->load(QUrl("qrc:/html/chat/chat.html"));
    chat->setContextMenuPolicy(Qt::NoContextMenu);
    QWebChannel* webchannel = new QWebChannel(chat);
    chat->page()->setWebChannel(webchannel);
    webContext = new FileContext;
    webchannel->registerObject(QStringLiteral("content"), (QObject*)webContext);
    ui->verticalLayout_2->addWidget(chat);
    connect(ui->listWidget,&QListWidget::itemSelectionChanged,this,&Home::changeSelected);
}

QString Home::randomString(){
        int max = 8;
        QString tmp = QString("0123456789ABCDEFGHIJKLMNOPQRSTUVWZYZ");
        QString str = QString();
        QTime t;
        t= QTime::currentTime();
        qsrand(t.msec()+t.second()*1000);
        for(int i=0;i<max;i++) {
            int ir = qrand()%tmp.length();
            str[i] = tmp.at(ir);
        }
        return str;
}

void::Home::changeSelected(){
    UserItem* currentItem = (UserItem*)ui->listWidget->itemWidget(ui->listWidget->currentItem());
    currentUser = currentItem->userName;
    messageThread->currentUm = currentUser;
    chat->page()->runJavaScript("Clear()");
    currentItem->readAll();
    if(!messageList.contains(currentUser)){
        chat->page()->runJavaScript("Empty()");
    }else{
        QListIterator<Message> i(messageList[currentUser]);
        while (i.hasNext()) {
            Message msg = i.next();
            if(msg.type=="img"){
                if(msg.selfsend){
                    if(userTime[msg.to].toString("mm")!=msg.time.toString("mm")){
                        userTime[msg.to] = msg.time;
                        insertTime(msg.time);
                    }
                    chat->page()->runJavaScript("addSelfImgMsg('"+msg.body.toHtmlEscaped()+"','"+msg.avatar+"','"+msg.mid+"')");
                }else{
                    if(currentUser == msg.from){
                        qDebug()<<"new Msg";
                        chat->page()->runJavaScript("addImgMsg('"+msg.body.toHtmlEscaped()+"','"+usernameAvatar[msg.avatarName]+"','"+msg.mid+"')");
                        if(userTime[msg.from].toString("mm")!=msg.time.toString("mm")){
                            userTime[msg.from] = msg.time;
                            insertTime(msg.time);
                        }
                    }else{

                    }
                }
            }else if(msg.type=="file"){
                if(msg.selfsend){
                    if(userTime[msg.to].toString("mm")!=msg.time.toString("mm")){
                        userTime[msg.to] = msg.time;
                        insertTime(msg.time);
                    }
                    chat->page()->runJavaScript("newSendFile('"+msg.fileName+"',"+QString::number(msg.total)+",'"+msg.body+"','"+msg.avatar+"','"+msg.mid+"')");
                    setProgress(msg.body,100,100);
                }else{
                    if(currentUser == msg.from){
                        if(userTime[msg.to].toString("mm")!=msg.time.toString("mm")){
                            userTime[msg.to] = msg.time;
                            insertTime(msg.time);
                        }
                        chat->page()->runJavaScript("newReceiveFile('"+msg.fileName+"',"+QString::number(msg.total)+",'"+msg.body+"','"+usernameAvatar[msg.avatarName]+"','"+msg.mid+"')");
                    }else{

                    }
                }
            }else{
                if(msg.selfsend){
                    if(userTime[msg.to].toString("mm")!=msg.time.toString("mm")){
                        userTime[msg.to] = msg.time;
                        insertTime(msg.time);
                    }
                    chat->page()->runJavaScript("addSelfTextMsg('"+msg.body.toHtmlEscaped()+"','"+msg.avatar+"','"+msg.mid+"')");
                }else{
                    if(currentUser == msg.from){
                        if(userTime[msg.from].toString("mm")!=msg.time.toString("mm")){
                            userTime[msg.from] = msg.time;
                            insertTime(msg.time);
                        }
                        chat->page()->runJavaScript("addTextMsg('"+msg.body.toHtmlEscaped()+"','"+usernameAvatar[msg.avatarName]+"','"+msg.mid+"')");
                    }else{

                    }
                }
            }
        }
    }
}

void Home::startListen(){
    thread = new QThread(this);
    messageThread = new MessageThread();
    messageThread->Token = Token;
    messageThread->userName = userName;
    messageThread->moveToThread(thread);
    connect(this,&Home::startThread,messageThread,&MessageThread::Start);
    connect(messageThread,SIGNAL(updateList(QJsonObject)),this,SLOT(updateList(QJsonObject)));
    connect(messageThread,SIGNAL(newMsg(Message*)),this,SLOT(newMsg(Message*)));
    emit startThread();
    thread->start();
    connect(this,SIGNAL(sendMsg(QString,QString,QString,QString,QString)),messageThread,SLOT(sendMsg(QString,QString,QString,QString,QString)));
    connect(this,SIGNAL(sendImg(QString,QString,QString,QString)),messageThread,SLOT(sendImg(QString,QString,QString,QString)));
    connect(this,SIGNAL(sendFile(QString,QString,QString,QString)),messageThread,SLOT(sendFile(QString,QString,QString,QString)));
    connect(this,SIGNAL(creatGroup(QString,QString,QList<QString>)),messageThread,SLOT(createGroup(QString,QString,QList<QString>)));
    connect(this,SIGNAL(changeProfile(QString,QString)),messageThread,SLOT(changeProfile(QString,QString)));
    connect(messageThread,SIGNAL(newSendFile(QString,qint64,QString,QString)),this,SLOT(sendNewFile(QString,qint64,QString,QString)));
    connect(messageThread,SIGNAL(setProgress(QString,qint64,qint64)),this,SLOT(setProgress(QString,qint64,qint64)));
    connect(messageThread,SIGNAL(updateStatus(int,QString)),this,SLOT(updateStatus(int,QString)));
    connect(messageThread,SIGNAL(newJoinedGroup(QString)),this,SLOT(newJoinedGroup(QString)));
    connect(messageThread,SIGNAL(offLine()),this,SLOT(offLine()));
    connect(webContext,SIGNAL(doDownload(QString,QString,qint64)),messageThread,SLOT(Download(QString,QString,qint64)));
    connect(webContext,SIGNAL(withDrawMsg(QString)),messageThread,SLOT(withDraw(QString)));
    connect(messageThread,SIGNAL(backMsg(QString,QString)),this,SLOT(backMsg(QString,QString)));
}

void Home::newJoinedGroup(QString grouoname){
    UserItem* user = new UserItem();
    QListWidgetItem *listItem =  new QListWidgetItem();
    listItem->setSizeHint(QSize(220,80));
    ui->listWidget->addItem(listItem);
    user->setUserInfo("[群聊]"+grouoname,"群聊","group");
    ui->listWidget->setItemWidget(listItem,user);
    usernameItem["[群聊]"+grouoname]=user;
    userTime.insert("[群聊]"+grouoname,QTime(0,0));
}

void Home::updateList(QJsonObject list){
    qDebug()<<"COPY";
    QList<int> added;
    usernameAvatar.clear();
    usernameItem.clear();
    userTime.clear();
    if(list.contains("online")){
        QJsonValue value = list.value("online");
        QJsonArray onlineList = value.toArray();
        int nSize = onlineList.size();
        for (int i = 0; i < nSize; ++i) {
            QJsonValue onlines = onlineList.at(i);
            QJsonObject singleUser = onlines.toObject();
            UserItem* user = new UserItem();
            QListWidgetItem *listItem =  new QListWidgetItem();
            listItem->setSizeHint(QSize(220,80));
            ui->listWidget->addItem(listItem);
            user->setUserInfo(singleUser.value("username").toString(),"在线",QString::number(singleUser.value("avatar").toInt(), 10));
            ui->listWidget->setItemWidget(listItem,user);
            usernameItem[singleUser.value("username").toString()]=user;
            added.append(singleUser.value("uid").toInt());
            usernameAvatar[singleUser.value("username").toString()]=QString::number(singleUser.value("avatar").toInt(), 10);
            userTime.insert(singleUser.value("username").toString(),QTime(0,0));
            //qDebug()<<singleUser.value("username").toString();
        }
    }

    if(list.contains("all")){
        QJsonValue valueOff = list.value("all");
        QJsonArray offLineList = valueOff.toArray();
        int nSizeOff = offLineList.size();
        for (int i = 0; i < nSizeOff; ++i) {
            QJsonValue offlines = offLineList.at(i);
            QJsonObject singleUserOff = offlines.toObject();
            if(!added.contains(singleUserOff.value("uid").toInt())){
                UserItem* user = new UserItem();
                QListWidgetItem *listItem =  new QListWidgetItem();
                listItem->setSizeHint(QSize(220,80));
                ui->listWidget->addItem(listItem);
                user->setUserInfo(singleUserOff.value("username").toString(),"离线",QString::number(singleUserOff.value("avatar").toInt(), 10)+"b");
                ui->listWidget->setItemWidget(listItem,user);
                usernameItem[singleUserOff.value("username").toString()]=user;
                usernameAvatar[singleUserOff.value("username").toString()]=QString::number(singleUserOff.value("avatar").toInt(), 10)+"b";
                userTime.insert(singleUserOff.value("username").toString(),QTime(0,0));
            }
        }
    }
}

void Home::insertTime(QTime msgTime){
    chat->page()->runJavaScript("insertTime('"+msgTime.toString("hh:mm")+"')");
}



void Home::updateStatus(int status,QString user){
    if(usernameItem.contains(user)){
        usernameItem[user]->setStatus(status);
    }
    if(status == 1){
        usernameAvatar[user]= usernameAvatar[user].replace("b","");
    }else{
        usernameAvatar[user]= usernameAvatar[user].replace("b","")+"b";
    }
}

void Home::backMsg(QString mid,QString um){
    QString isSelf = (um=="current") ? "yes":"np";
    chat->page()->runJavaScript("withDrawMsg('"+mid+"','"+isSelf+"')");
    um=(um=="current")?currentUser:um;
    for(int i = 0 ; i < messageList[um].size() ; i++) {
        if(messageList[um][i].mid==mid){
            messageList[um].removeAt(i);
        }
    }
}

void Home::newMsg(Message * msg){
    if(msg->type=="img"){
        if(msg->selfsend){
            if(userTime[msg->to].toString("mm")!=msg->time.toString("mm")){
                userTime[msg->to] = msg->time;
                insertTime(msg->time);
            }
            chat->page()->runJavaScript("addSelfImgMsg('"+msg->body.toHtmlEscaped()+"','"+msg->avatar+"','"+msg->mid+"')");
        }else{
            if(currentUser == msg->from){
                qDebug()<<"new Msg";
                chat->page()->runJavaScript("addImgMsg('"+msg->body.toHtmlEscaped()+"','"+usernameAvatar[msg->avatarName]+"','"+msg->mid+"')");
                if(userTime[msg->from].toString("mm")!=msg->time.toString("mm")){
                    userTime[msg->from] = msg->time;
                    insertTime(msg->time);
                }
            }else{
                usernameItem[msg->from]->addUnreadMessage();
            }
        }
    }else if(msg->type=="file"){
        if(msg->selfsend){
            if(userTime[msg->to].toString("mm")!=msg->time.toString("mm")){
                userTime[msg->to] = msg->time;
                insertTime(msg->time);
            }
            chat->page()->runJavaScript("newSendFile('"+msg->fileName+"',"+QString::number(msg->total)+",'"+msg->body+"','"+msg->avatar+"','"+msg->mid+"')");
        }else{
            if(currentUser == msg->from){
                if(userTime[msg->from].toString("mm")!=msg->time.toString("mm")){
                    insertTime(msg->time);
                    userTime[msg->from] = msg->time;
                }
                chat->page()->runJavaScript("newReceiveFile('"+msg->fileName+"',"+QString::number(msg->total)+",'"+msg->body+"','"+usernameAvatar[msg->avatarName]+"','"+msg->mid+"')");
            }else{
                usernameItem[msg->from]->addUnreadMessage();
            }
        }
    }else{
        if(msg->selfsend){
            if(userTime[msg->to].toString("mm")!=msg->time.toString("mm")){
                userTime[msg->to] = msg->time;
                insertTime(msg->time);
            }
            chat->page()->runJavaScript("addSelfTextMsg('"+msg->body.toHtmlEscaped()+"','"+msg->avatar+"','"+msg->mid+"')");
        }else{
            if(currentUser == msg->from){
                if(userTime[msg->from].toString("mm")!=msg->time.toString("mm")){
                    insertTime(msg->time);
                    userTime[msg->from] = msg->time;
                }
                chat->page()->runJavaScript("addTextMsg('"+msg->body.toHtmlEscaped()+"','"+usernameAvatar[msg->avatarName]+"','"+msg->mid+"')");
            }else{

                usernameItem[msg->from]->addUnreadMessage();
            }
        }
    }
    QString session = msg->selfsend ? msg->to : msg->from;
    messageList[session].append(*msg);
    delete msg;

}

void Home::sendNewFile(QString filename, qint64 fsize, QString id,QString mid){
    Message *msg = new Message("file",id,userName,currentUser,1);
    msg->fileName = filename;
    msg->total = fsize;
    msg->mid = mid;
    msg->setTime(QTime::currentTime().toString("hh:mm"));
    msg->avatar = usernameAvatar[userName];
    newMsg(msg);
}

void Home::setProgress(QString id, qint64 sended, qint64 total){
    chat->page()->runJavaScript("setProgress('"+id+"',"+QString::number(sended)+",'"+QString::number(total)+"')");
}

Home::~Home()
{
    delete ui;
    thread->quit();
    thread->wait();
    delete thread;
    delete messageThread;
    delete webContext;
}

void Home::on_pushButton_clicked()
{
    QString msgBody = ui->textEdit_2->toPlainText();
    Message *msg = new Message("text",msgBody,userName,currentUser,1);
    msg->setTime(QTime::currentTime().toString("hh:mm"));
    msg->avatar = usernameAvatar[userName];
    msg->mid = randomString();
    newMsg(msg);
    emit sendMsg(currentUser,msgBody,"text",userName,msg->mid);
    ui->textEdit_2->setPlainText("");
}

void Home::on_pushButton_2_clicked()
{
    Dialog* dlg = new Dialog;
    dlg->setWindowTitle("充值");
    //dlg->setAttribute(Qt::WA_DeleteOnClose);
    if(dlg->exec()==QDialog::Accepted){
        purchase = new Purchase();
        purchase->init(dlg->num,userName);
        purchase->show();
        purchase->loadQRCode();
    }else{

    }
    delete dlg;
}

void Home::on_pushButton_9_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,"选择图片","图片","*.png *.jpg *.gif *.jpeg *.bmp");
    if(!file_name.isNull()){
        emit sendImg(currentUser,file_name,userName,usernameAvatar[userName]);
    }
}

void Home::on_pushButton_3_clicked()
{
    chat->page()->runJavaScript("Empty()");
    messageList[currentUser].clear();
    userTime[currentUser] = QTime(0,0);
}

void Home::on_pushButton_10_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,"选择文件","文档","*.*");
    if(!file_name.isNull()){
        emit sendFile(currentUser,file_name,userName,usernameAvatar[userName]);
    }
}

void Home::on_pushButton_4_clicked()
{
    CreatGroup* dlg = new CreatGroup;
    //dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->Init(usernameAvatar,userName);
    if(dlg->exec()==QDialog::Accepted){
        if(dlg->groupName.isEmpty()||dlg->selected.count()==0){
            QMessageBox::information(NULL, "错误", "信息填写不完成");
        }else{
            emit creatGroup(dlg->groupName,userName,dlg->selected);
        }
    }else{

    }
    delete dlg;
}

void Home::offLine(){
    QMessageBox::information(NULL, "错误", "与服务端断开连接，请尝试重新登录");
    close();
}

void Home::on_pushButton_6_clicked()
{
    EditProfile* dlg = new EditProfile;
    //dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->Init(userName);
    if(dlg->exec()==QDialog::Accepted){
        emit changeProfile(dlg->userName,dlg->passWord);
    }else{

    }
    delete dlg;
}

void Home::changeStyle(QString selfback, QString selftexr, QString youback, QString youtext){
    chat->page()->runJavaScript("changeStyle('"+selfback+"','"+selftexr+"','"+youback+"','"+youtext+"')");
}

void Home::changeBackground(QString img){
     chat->page()->runJavaScript("changeBackground('"+img+"')");
}

void Home::changeBackColor(QString color){
    chat->page()->runJavaScript("changeBackColor('"+color+"')");
}

void Home::on_pushButton_5_clicked()
{
    StyleChange* dlg = new StyleChange;
    connect(dlg,SIGNAL(changeStyle(QString,QString,QString,QString)),this,SLOT(changeStyle(QString,QString,QString,QString)),Qt::UniqueConnection);
    connect(dlg,SIGNAL(changeBackground(QString)),this,SLOT(changeBackground(QString)),Qt::UniqueConnection);
    connect(dlg,SIGNAL(changeBackColor(QString)),this,SLOT(changeBackColor(QString)),Qt::UniqueConnection);
    //dlg->setAttribute(Qt::WA_DeleteOnClose);
    if(dlg->exec()==QDialog::Accepted){
    }else{

    }
    delete dlg;
}
