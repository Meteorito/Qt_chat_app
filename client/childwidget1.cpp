#include "childwidget1.h"
#include "ui_childwidget1.h"

ChildWidget1::ChildWidget1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChildWidget1)
{
    ui->setupUi(this);
}

ChildWidget1::ChildWidget1(UserData userData, QTcpSocket *tcpSocket):
    ui(new Ui::ChildWidget1)
{
    ui->setupUi(this);
    this->userData = userData;
    this->tcpSocket = tcpSocket;

    initial();
}

ChildWidget1::~ChildWidget1()
{
    delete ui;
}

//创建房间请求
void ChildWidget1::createRoomQuery(QString roomName)
{
    if(roomName == NULL){
        QMessageBox::critical(this,"错误","房间名为空");
        return;
    }

    QByteArray writeArray;
    QDataStream out(&writeArray,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::CreateRoom<<userData.getName()<<roomName;

    this->tcpSocket->write(writeArray);
}

void ChildWidget1::handleRefreshRoom()
{
    model->clear();//清空模型数据
    //重新绑定表头
    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal,"创建者");//第一列为创建者
    model->setHeaderData(1,Qt::Horizontal,"房间名");//第二列为房间名

    QMap<QString,Room>::iterator it = roomMap.begin();
    for(int i = 0; i < roomMap.size();++i){
        model->setItem(i,0,new QStandardItem(it.key()));
        model->setItem(i,1,new QStandardItem(it.value().getRoomName()));
        ++it;
    }
    ui->roomNumLabel->setText(QString("%1").arg(roomMap.count()));
    ui->roomtableView->setFocus();

}

void ChildWidget1::handleRoomList(QDataStream &in)
{
    int roomCnt;

    Room room;
    QString roomOwner,roomName;
    quint32 roomHost;
    int roomPort;
    roomMap.clear();

    in>>roomCnt;
    qDebug()<<"房间数量:"<<roomCnt;
    for(int  i =0;i<roomCnt;i++){
       in>>roomOwner>>roomName>>roomHost>>roomPort;

       room.setRoomOwner(roomOwner);
       room.setRoomName(roomName);
       QHostAddress host(roomHost);
       room.setHost(host);
       room.setPort(roomPort);

       roomMap.insert(roomOwner,room);//更新roomMap
    }
}

void ChildWidget1::initial()
{
    model = new QStandardItemModel(this);
    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal,"创建者");//第一列为创建者
    model->setHeaderData(1,Qt::Horizontal,"房间名");//第二列为房间名
    ui->roomtableView->setModel(model);
    ui->roomtableView->setSelectionBehavior(QAbstractItemView::SelectRows);//选择整行
    ui->roomtableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    //ui->roomtableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自动设置表格大小
    ui->roomtableView->horizontalHeader()->setStretchLastSection(true);//设置最后一列占据所有空间，不留空白列
    ui->userNickNameLabel->setText(userData.getName());
    ui->roomNumLabel->setText(QString("%1").arg(0));
}

void ChildWidget1::tcpReadSlot()
{
    QByteArray buffArray;
    buffArray.resize(tcpSocket->readBufferSize());
    QDataStream in(&buffArray,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Version::Qt_5_7);
    buffArray = tcpSocket->readAll();

    QByteArray writeArray;
    QDataStream out(&writeArray,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    int msgType;

    in>>msgType;

    qDebug()<<"msgType:"<<msgType;
    switch (msgType){
        case MessageType::ROOMLIST:{
                handleRoomList(in);
                handleRefreshRoom();//更新房间信息
             }
             break;
        case MessageType::CreateRoom:{
             bool status;
             in>>status;
             if(status == true){
                QMessageBox::information(this,"消息","房间创建成功");
                handleCreateRoom(in);
                handleRefreshRoom();
               // this->hide();
                //widget2.show();
             }else{
                 QString errorMsg;
                 in>>errorMsg;
                 QMessageBox::critical(this,"错误","房间创建失败:"+errorMsg);//不会失败
             }
             break;
        }
        default:
            break;
    }
}

void ChildWidget1::deleteRoomQuery(QString roomOwner,bool exit)
{
    QByteArray writeArray;
    QDataStream out(&writeArray,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::DeleteRoom<<roomOwner;

    this->tcpSocket->write(writeArray);

    if(exit == false)
        this->show();
}

void ChildWidget1::leftRoom()
{
    this->show();
    on_RefreshRoomBtn_clicked();//刷新
}

//
void ChildWidget1::handleCreateRoom(QDataStream &in)
{
    QString roomName;
    quint32 roomHost;
    int roomPort;
    Room room;

    in>>roomName>>roomHost>>roomPort;
    room.setRoomOwner(userData.getName());
    room.setRoomName(roomName);
    QHostAddress host(roomHost);
    room.setHost(host);
    room.setPort(roomPort);

    roomMap.insert(userData.getName(),room);

    widget2 = new ChildWidget2(room,userData.getName());
    //连接Widget1删除房间操作与Widget2删除房间信号;
    connect(widget2,SIGNAL(deleteRoomSignal(QString,bool)),this,SLOT(deleteRoomQuery(QString,bool)));
    connect(widget2,SIGNAL(leftRoomSignal()),this,SLOT(leftRoom()));
    this->hide();
    widget2->show();
}

void ChildWidget1::handleJoinRoom(QString roomOwner)
{
    QMap<QString,Room>::iterator it;
    it = roomMap.find(roomOwner);
    if(it == roomMap.end()){
        QMessageBox::critical(this,"错误","房间不存在");
        return;
    }
    Room room = it.value();
    widget2 = new ChildWidget2(room,userData.getName());
    //连接Widget1删除房间操作与Widget2删除房间信号;
    connect(widget2,SIGNAL(deleteRoomSignal(QString,bool)),this,SLOT(deleteRoomQuery(QString,bool)));
    connect(widget2,SIGNAL(leftRoomSignal()),this,SLOT(leftRoom()));
    this->hide();
    widget2->show();
}


void ChildWidget1::on_createRoomBtn_clicked()
{

    bool ok;
    QString text =  QInputDialog::getText(this,"创建房间","请输入房间名",QLineEdit::Normal,0,&ok);
    if(ok == true){
        if(text.isEmpty()){
            QMessageBox::critical(this,"错误","房间名不能为空");
        }
        createRoomQuery(text);
    }
}

void ChildWidget1::on_RefreshRoomBtn_clicked()
{
    QByteArray buffArray;
    QDataStream out(&buffArray,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_5_7);

    out<<MessageType::ROOMLIST;

    tcpSocket->write(buffArray);
}

void ChildWidget1::on_joinRoomBtn_clicked()
{
    QString roomOwner;
    if(model->item(0,0) == NULL){
        QMessageBox::warning(this,"警告","没有房间存在");
    }else{
        ui->roomtableView->setFocus();
        int row = ui->roomtableView->currentIndex().row();
        roomOwner = model->item(row,0)->text();
        handleJoinRoom(roomOwner);
    }
}

void ChildWidget1::on_quitChildWidget1_clicked()
{
    this->close();
}
