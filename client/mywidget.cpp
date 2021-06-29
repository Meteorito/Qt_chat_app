/*使用时提前在数据库中创建名为：qmysql的数据库；
  在qmysql数据库中创建表单tb1；
  tb1包含：（name char(10) primary key,pwd char(8)）;
*/
#include "mywidget.h"
#include "ui_mywidget.h"


myWidget::myWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myWidget)
{
    ui->setupUi(this);
    widget1 = NULL;

    socket = new QTcpSocket(this);
    //connect(socket,SIGNAL(connected()),this, SLOT(connectSlot()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readDataSlot()));

    QObject::connect(ui->button_signin,SIGNAL(clicked(bool)),this,SLOT(signIn()));
    QObject::connect(ui->button_signup,SIGNAL(clicked(bool)),this,SLOT(signUp()));
    QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connectError(QAbstractSocket::SocketError)));

    connect(this,&myWidget::SignInSuccess,this,&myWidget::SignInSuccessSlot);
    connect(this,&myWidget::SignInFail,this,&myWidget::SignInFailSlot);
    connect(this,&myWidget::SignUpFail,this,&myWidget::SignUpFailSlot);
    connect(this,&myWidget::SignUpSuccess,this,&myWidget::SignUpSuccessSlot);

    socket->connectToHost(QHostAddress::LocalHost,12345);

}

myWidget::~myWidget()
{
    delete ui;
    if(widget1!=NULL){
        delete widget1;
    }
}

//连接
bool myWidget::connectSlot(){
    return true;
}

void myWidget::readDataSlot(){
    QByteArray buffArray;
    buffArray.resize(socket->readBufferSize());
    QDataStream in(&buffArray,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Version::Qt_5_7);
    buffArray = socket->readAll();

    int msgType;

    in>>msgType;
    qDebug()<<"myWidget MsgType:"<<msgType;
    switch (msgType){
    case MessageType::LOGIN:{
        bool status;
        in>>status;
        if(status == true){
           emit SignInSuccess(socket);
        }else{
            emit SignInFail();

        }
        break;
    }
    case MessageType::REGISTER:{
        bool status;
        in>>status;
        if(status == true){
            emit SignUpSuccess();
        }else{
            emit SignUpFail();
        }
        break;
    }
    default:
        break;
    }

}

void myWidget::connectError(QAbstractSocket::SocketError error){
    Q_UNUSED(error)
    QMessageBox msgBox;
    msgBox.critical(this,"警告","未连接到服务器");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    ui->msg_lable->setPalette(pa);
    ui->msg_lable->setText("警告,未连接到服务器");
}

//登录
void myWidget::signIn(void){
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("消息");

    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_5_7);

    out<<MessageType::LOGIN;
    out<<ui->lineEdit_name->text();
    out<<ui->lineEdit_pswd->text();
    if(socket->state() == QTcpSocket::ConnectedState){
        socket->write(buff);
        socket->waitForBytesWritten();
    }
}

//注册
void myWidget::signUp(void){
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("消息");

    if(ui->lineEdit_name->text() == ""){
        messageBox.setText("用户名不能为空");
        messageBox.exec();
        return;
    }

    if(ui->lineEdit_pswd->text() == ""){
        messageBox.setText("密码不能为空");
        messageBox.exec();
        return;
    }

    userData.setUserName(ui->lineEdit_name->text());
    userData.setUserPswd(ui->lineEdit_pswd->text());

    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_5_7);

    out<<MessageType::REGISTER;
    out<<ui->lineEdit_name->text();
    out<<ui->lineEdit_pswd->text();

    if(socket->state() == QTcpSocket::ConnectedState){
        socket->write(buff);
        socket->waitForBytesWritten();
    }
}

void myWidget::SignInSuccessSlot(){
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("消息");
    messageBox.setText("登录成功！");

    userData.setUserName(ui->lineEdit_name->text());
    userData.setUserPswd(ui->lineEdit_pswd->text());

    disconnect(socket,SIGNAL(readyRead()),this,SLOT(readDataSlot()));//断开与本地socket读槽的连接
    this->widget1 = new ChildWidget1(userData,socket);           //登录成功，进入新窗口1
    connect(socket,SIGNAL(readyRead()),widget1,SLOT(tcpReadSlot()));//将socket准备读取信号连接到窗口1读槽

    this->hide();                                                   //隐藏登录注册窗口
    this->widget1->show();

    messageBox.exec();
}

void myWidget::SignInFailSlot(){
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("消息");
    messageBox.setText("登录失败！");
    messageBox.exec();
}

void myWidget::SignUpSuccessSlot(){
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("消息");
    messageBox.setText("注册成功！");
    messageBox.exec();

}

void myWidget::SignUpFailSlot(){
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("消息");
    messageBox.setText("注册失败！");
    messageBox.exec();
}

void myWidget::closeEvent(QCloseEvent *ev){
    Q_UNUSED(ev)
    socket->close();
}

void myWidget::on_radioButton_clicked()
{
    if(ui->radioButton->isChecked() == false){
        ui->lineEdit_pswd->setEchoMode(QLineEdit::Normal);
    }else{
        ui->lineEdit_pswd->setEchoMode(QLineEdit::Password);
    }
}
