#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    pSqlTool = MySqlTool::getSQLInstance();

    pSqlTool->connectSQL("localhost",3306,"qmysql","root","123456");//连接数据库
    pSqlTool->execSQL("select * from tb1");//初始化信息
    while(pSqlTool->pSqlQuery->next()){
        UserData userData;
        userData.setUserName(pSqlTool->pSqlQuery->value("name").toString());
        userData.setUserPswd(pSqlTool->pSqlQuery->value("pwd").toString());
        map.insert(userData.getUserName(),userData);
    }

    server = new QTcpServer(this);
    connect(server,SIGNAL(newConnection()),this,SLOT(newConnectSlot()));
    server->listen(QHostAddress::Any,12345);

   // timer.start(20);
    //connect(&timer,SIGNAL(timeout()),this,timerOutSlot());
}

Widget::~Widget()
{
    threadFinish();
    pSqlTool->closeSQL();
    delete ui;
}

void Widget::threadFinish()
{
    QMap<QTcpSocket*,QThread*>::iterator it = threadMap.begin();
    for( ; it != threadMap.end();++it){
        it.value()->quit();
        it.value()->wait();
    }
    threadMap.clear();
}

void Widget::newConnectSlot(){
    QTcpSocket* socket;
    socket =  server->nextPendingConnection();

    QThread* thread = new QThread(this);//创建线程
    threadMap.insert(socket,thread);//插入线程

    TCPSocket* tcpWork = new TCPSocket;

    tcpWork->settcpSocket(socket);//设置套接字
    tcpWork->setMutex(&(this->mutex));//设置互斥锁
    tcpWork->setUserInfoMap(&(this->map));//设置用户信息
    tcpWork->setRoomInfoMap(&(this->roomMap));

    tcpWork->moveToThread(thread);//移动到线程处理

    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));//线程结束，自动删除
    connect(socket,SIGNAL(readyRead()),tcpWork,SLOT(work()));//连接套接字读取信号与线程执行槽
    connect(socket,SIGNAL(disconnected()),tcpWork,SLOT(socketDisconnectSlot()));
    connect(tcpWork,SIGNAL(writeSignal(QTcpSocket*,QByteArray)),this,SLOT(tcpWriteSlot(QTcpSocket*,QByteArray)));//由主线程写
    connect(tcpWork,SIGNAL(closeTCPSocket(QTcpSocket*)),this,SLOT(closeThread(QTcpSocket*)));

    thread->start();//执行线程


    QString text;
    text = "客户端:"+socket->peerName() \
             + "HostAddress:"+socket->peerAddress().toString() \
             +"端口号："+QString::number(socket->peerPort());
    ui->label->setText(ui->label->text()+"\n"+text);

}

void Widget::tcpWriteSlot(QTcpSocket *socket, QByteArray buff)
{
  if(socket->state() == QAbstractSocket::ConnectedState){
      socket->write(buff);
      socket->waitForBytesWritten();
  }
}

void Widget::closeThread(QTcpSocket*  socket)
{
    QMap<QTcpSocket*,QThread*>::iterator it = threadMap.find(socket);

    if(it != threadMap.end()){
        it.value()->quit();
        it.value()->wait();
        threadMap.remove(socket);
    }
}







