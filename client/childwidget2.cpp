#include "childwidget2.h"
#include "ui_childwidget2.h"

ChildWidget2::ChildWidget2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChildWidget2)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    initial();
}

ChildWidget2::ChildWidget2(Room room, QString userName):
    ui(new Ui::ChildWidget2)
{
    ui->setupUi(this);
    this->room = room;
    this->userName = userName;

    udpCmdSocket = new QUdpSocket(this);//读命令
    cmdPort = room.getPort();//命令端口

    udpAudioSocket = new QUdpSocket(this);//读声音数据
    audioPort = room.getPort()+1;

    udpCameraSocket = new QUdpSocket(this);//读房主摄像头数据
    cameraPort = room.getPort()+2;

    udpSendAudioSocket = new QUdpSocket(this);
    udpSendCameraSocket = new QUdpSocket(this);
    udpSendSocket = new QUdpSocket(this);//写命令与数据


    initial();//界面初始化

    udpCmdSocket->bind(QHostAddress::AnyIPv4,cmdPort,QAbstractSocket::ReuseAddressHint);//第二个端口进行命令接收
    udpCmdSocket->joinMulticastGroup(room.getHost());
    connect(udpCmdSocket,SIGNAL(readyRead()),this,SLOT(udpCmdReadSlot()));

    udpAudioSocket->bind(QHostAddress::AnyIPv4,audioPort,QAbstractSocket::ReuseAddressHint);
    udpAudioSocket->joinMulticastGroup(room.getHost());
    connect(udpAudioSocket,SIGNAL(readyRead()),this,SLOT(audioRcv()));//接收音频数据

    udpCameraSocket->bind(QHostAddress::AnyIPv4,cameraPort,QAbstractSocket::ReuseAddressHint);
    udpCameraSocket->joinMulticastGroup(room.getHost());
    connect(udpCameraSocket,SIGNAL(readyRead()),this,SLOT(cameraRcv()));//接收相机数据

    connect(ui->lineEdit,SIGNAL(pressEnter()),this,SLOT(on_sendMsgBtn_clicked()));//消息自动滚动

    ui->myNickName->setText(this->userName);                //设置我的昵称
    ui->roomNameLabel->setText(room.getRoomName());    //设置房间名
    ui->roomOwnerLabel->setText(room.getRoomOwner()); //设置房主名

    if(userName != room.getRoomOwner()){     //不是房主，不能控制麦克风和摄像头
        ui->camerStatusBox->setEnabled(false);
        ui->comboBox->setEnabled(false);

    }else{
        cameraSetInitial();
    }
    audioSetInitial();

    joinRoomBroadcast();
}

ChildWidget2::~ChildWidget2()
{
    delete ui;
}

void ChildWidget2::setRoom(Room room)
{
    this->room = room;
}

void ChildWidget2::setUserName(QString userName)
{
    this->userName = userName;
}

void ChildWidget2::closeEvent(QCloseEvent *event)//窗口关闭
{
    Q_UNUSED(event)

    if(room.getRoomOwner() == userName){ //是房主，删除房间
        closeRoomBroadcast();                  //告知房间成员，房主已经退出
        emit cameraStopSignal();                //关闭摄像头
        emit audioInputStopSignal();            //关闭音频输入
        emit deleteRoomSignal(userName,true); //删除房间列表中对应的房间,true代表关闭所有窗口
    }else{
        emit audioOutputStopSignal();       //关闭音频输出
        leftRoomBroadcast();            //不是房主，离开房间，广播离开信息
    }
    closeUDPSocket();
    exit(0);//退出程序
}

void ChildWidget2::initial()
{
    model = new QStandardItemModel(this);
    this->setMinimumWidth(700);
    ui->rightWidget->setMaximumWidth(300);
    ui->rightDownWidget->setMaximumWidth(300);

    ui->tableView->setModel(model);

    model->setColumnCount(1);
    model->setHeaderData(0,Qt::Horizontal,"昵称");
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->tableView->horizontalHeader()->setStretchLastSection(true);//设置最后一列占据所有空间，不留空白列
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->userNum->setText("0 人");
    connect(ui->msgBrowser, SIGNAL(cursorPositionChanged()), this, SLOT(msgBrowserAutoScroll()));//消息自动滚动
    ui->cameraLabel->setScaledContents(true);//设置填满空间
}

void ChildWidget2::cameraSetInitial()
{
    camera = new cameraDrive;
    cameraThread = new QThread(this);
    camera->moveToThread(cameraThread);
    connect(cameraThread,SIGNAL(finished()),cameraThread,SLOT(deleteLater()));
    connect(this,SIGNAL(cameraInitialSignal()),camera,SLOT(cameraInitial()));
    connect(this,SIGNAL(cameraStartSignal()),camera,SLOT(cameraStart()));
    connect(this,SIGNAL(cameraStopSignal()),camera,SLOT(cameraStop()));
    connect(camera,SIGNAL(videoPixmapSignal(QPixmap)),this,SLOT(videoChangedSlot(QPixmap)));//本地播放
    connect(camera,SIGNAL(videoPixmapSignal(QPixmap)),this,SLOT(sendCameraData(QPixmap)));//发送到网络
    cameraThread->start();
    emit cameraInitialSignal();
}

void ChildWidget2::audioSetInitial()
{
    audio = new AudioDrive;
    audioThread = new QThread(this);
    audio->moveToThread(audioThread);

    connect(audioThread,SIGNAL(finished()),audioThread,SLOT(deleteLater()));

    connect(this,SIGNAL(audioInitialSignal()),audio,SLOT(audioDriveInitial()));
    connect(this,SIGNAL(audioInputStartSignal()),audio,SLOT(audioInputStart()));
    connect(this,SIGNAL(audioInputStopSignal()),audio,SLOT(audioInputStop()));
    connect(this,SIGNAL(audioOutputStartSignal()),audio,SLOT(audioOutputStart()));
    connect(this,SIGNAL(audioOutputStopSignal()),audio,SLOT(audioOutputStop()));
    connect(this,SIGNAL(audioPlaySignal(char*,int)),audio,SLOT(audioDataSlot(char*,int)));

    //connect(audio,SIGNAL(audioDataSignal(char*,int)),audio,SLOT(audioDataSlot(char*,int)));//发送到本地
    connect(audio,SIGNAL(audioDataSignal(char*,int)),this,SLOT(sendAudioData(char*,int)));//发送到网络
    audioThread->start();
    emit audioInitialSignal();
    if(userName != room.getRoomOwner()){
        emit audioOutputStartSignal();//打开音频接收
    }
}

void ChildWidget2::joinRoomBroadcast()//用户加入房间，广播用户加入信息
{
    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::JOINROOM<<userName;

    udpSendSocket->writeDatagram(buff,room.getHost(),cmdPort);
    udpSendSocket->waitForBytesWritten();
}

void ChildWidget2::leftRoomBroadcast()//本人不是房主，广播离开房间信息
{
    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::LeftRoom<<userName;

    udpSendSocket->writeDatagram(buff,room.getHost(),cmdPort);
    udpSendSocket->waitForBytesWritten();
}

void ChildWidget2::closeRoomBroadcast()//广播删除房间信息,本人是房主才会广播
{
    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::DeleteRoom;
    udpSendSocket->writeDatagram(buff,room.getHost(),cmdPort);
    udpSendSocket->waitForBytesWritten();

}

void ChildWidget2::UserVectorBroadcast()
{
    if(userName == room.getRoomOwner()){//是房主,才广播用户信息
        QByteArray buff;
        QDataStream out(&buff,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_7);

        int userNum = userVector.size();
        out<<MessageType::RoomUserList<<userNum;
        for(int i = 0;i<userNum;i++){
            out<<userVector.at(i);
        }

        udpSendSocket->writeDatagram(buff,room.getHost(),cmdPort);
        udpSendSocket->waitForBytesWritten();
    }
}

void ChildWidget2::messageBroadcast(QString msg)
{
    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::Message<<userName<<msg;
    udpSendSocket->writeDatagram(buff,room.getHost(),cmdPort);
    udpSendSocket->waitForBytesWritten();
}

void ChildWidget2::closeCameraBroadcast()
{
    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::CloseCamera;
    udpSendSocket->writeDatagram(buff,room.getHost(),cmdPort);
    udpSendSocket->waitForBytesWritten();
}

void ChildWidget2::closeAudioBroadcast()
{
    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::CloseAudio;
    udpSendSocket->writeDatagram(buff,room.getHost(),cmdPort);
    udpSendSocket->waitForBytesWritten();
}

void ChildWidget2::openAudioBroadcast()
{
    QByteArray buff;
    QDataStream out(&buff,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);

    out<<MessageType::OpenAudio;
    udpSendSocket->writeDatagram(buff,room.getHost(),cmdPort);
    udpSendSocket->waitForBytesWritten();
}

void ChildWidget2::quitRoom()//由于房主关闭房间引起的房间退出,本人不是房主,会弹出房主离开消息框，并关闭房间
{
    if(userName != room.getRoomOwner()){
        QMessageBox::critical(this,"错误","房主已经离开,房间退出...");
    }
    emit leftRoomSignal();
    closeUDPSocket();
    this->hide();

}

void ChildWidget2::closeUDPSocket()
{
    udpCmdSocket->close();
    udpSendSocket->close();
    udpAudioSocket->close();
}

void ChildWidget2::UserVectorRcv(QDataStream &in)//接收用户信息，不是房主才接收,
{
    if(userName == room.getRoomOwner()){
        return;
    }

    int userNum;
    QString userName;

    in>>userNum;
    for(int i = 0; i < userNum;i++){
        in>>userName;
        userVector.push_back(userName);
    }
}

void ChildWidget2::messageRcv(QDataStream &in)
{
    QString name;
    QString msg;
    in>>name>>msg;
    if(name == userName){
        ui->msgBrowser->insertPlainText(QString("[我]:%1\n").arg(msg));
    }else{
        ui->msgBrowser->insertPlainText(QString("[%1]:%2\n").arg(name).arg(msg));
    }
    barrageMsg(msg);

}

void ChildWidget2::barrageMsg(QString msg)
{
    //    int x = ui->cameraLabel->x();
    //    int y = ui->cameraLabel->y();
    //    int width = ui->cameraLabel->width();
        //int height = ui->cameraLabel->height();

        QLabel* label = new QLabel(this);
        label->show();
        label->setStyleSheet("color:white");
    //    label->setStyleSheet("background-color:green");
        label->setStyleSheet("border:2px solid red;");
        label->setFont(QFont( "宋体" , 28,QFont::Bold));
        label->setText(msg);
        label->adjustSize();

    //    QFontMetrics metrics(ui->cameraLabel->font());
    //    int textWidth = metrics.width(msg);
    //    int textHight = metrics.height();
        int textWidth =   label->width();
        int textHeight = label->height();
        int maxWidth = ui->cameraLabel->width();
        int maxHeight = ui->cameraLabel->height();
        int startX = ui->cameraLabel->geometry().x();
        int startY = ui->cameraLabel->geometry().y();

        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        int positionY = startY+qrand()%(maxHeight-textHeight);
        QPropertyAnimation* anim =new QPropertyAnimation(label, "pos");//"geometry" QRect
        anim ->setDuration(maxWidth*10);
        anim->setStartValue(QPoint(startX+maxWidth-textWidth,positionY));
        anim->setEndValue(QPoint(startX,positionY));
        anim ->setEasingCurve(QEasingCurve::Linear);
        connect(anim,&QPropertyAnimation::finished,[=]{//动画结束，自动关闭动画
            label->hide();
        });
        anim->start();
}

void ChildWidget2::cameraRcv()
{
    if(room.getRoomOwner() == userName){//房主不接受视频数据
        return;
    }

    static QByteArray* arr = new QByteArray(200000,0);
    static QDataStream* stream = new QDataStream(arr,QIODevice::ReadWrite);
    CameraPack buf;
    while(udpCameraSocket->hasPendingDatagrams()){
        udpCameraSocket->readDatagram(buf.getData(),buf.getSize());
        stream->device()->seek(buf.getId()*1024);//根据是第几次发送的图片偏移到正确的位置
        stream->writeRawData(buf.getText(),buf.getLastByte());
        if(buf.getIsLast()){
            QImage image((uchar*)arr->data(),buf.getWidth(),buf.getLen(),QImage::Format_RGB32);
            QPixmap pic = QPixmap::fromImage(image);

            pic.scaled(ui->cameraLabel->size());
            ui->cameraLabel->setPixmap(pic);
        }
    }
}

void ChildWidget2::audioRcv()
{
    if(userName == room.getRoomOwner()){//房主不接收音频
        return;
    }

    AudioPack pack;
    while(udpAudioSocket->hasPendingDatagrams()){
        udpAudioSocket->readDatagram(pack.getData(),pack.getSize());
        emit audioPlaySignal(pack.getData(),pack.getLen());//发送播放的信号数据
    }
}


void ChildWidget2::udpCmdReadSlot()
{
    QByteArray buff;
    QDataStream in(&buff,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_7);
    while(udpCmdSocket->hasPendingDatagrams())
    {
            buff.resize(udpCmdSocket->pendingDatagramSize());
            udpCmdSocket->readDatagram(buff.data(),buff.size());
    }
    int type;
    QString name;
    in>>type;

    switch (type){
    case MessageType::JOINROOM://如果本人是房主，才接收用户加入信息，广播房间所有成员昵称
        if(userName == room.getRoomOwner()){
            in>>name;
            userVector.push_back(name);
            UserVectorBroadcast();
        }
        break;
    case MessageType::LeftRoom://如果本人是房主，才接收信息，并删除用户信息，广播房间所有成员昵称
        if(userName == room.getRoomOwner()){
            in>>name;
            userVector.removeOne(name);//移除人员
            UserVectorBroadcast();
        }
        break;
    case MessageType::DeleteRoom://接收到房主退出房间信息，需删除该聊天室,并退出
        quitRoom();
        break;
    case MessageType::RoomUserList:
        UserVectorRcv(in);               //如果不是房主,接收房间用户信息
        break;
    case MessageType::Message:
        messageRcv(in);
        break;
    case MessageType::CloseCamera:
        if(userName != room.getRoomOwner()){
            QImage* imag = new QImage;
            imag->load(":new/icon/images/recorder.jpg");
            ui->cameraLabel->setPixmap(QPixmap::fromImage(*imag));
        }
        break;
    case MessageType::CloseAudio:
        if(userName != room.getRoomOwner())
            emit audioOutputStopSignal();
    case MessageType::OpenAudio:
        if(userName != room.getRoomOwner()){
            emit audioOutputStartSignal();
        }
    default:
        break;
    }

}

void ChildWidget2::msgBrowserAutoScroll()//消息自动滚动
{
    QTextCursor cursor =  ui->msgBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->msgBrowser->setTextCursor(cursor);
}

void ChildWidget2::sendCameraData(QPixmap pixmap)
{
    QImage image = pixmap.toImage();
    CameraPack buf;
    int writeBytes = 0;
    int countBytes = image.byteCount();
    buf.setId(0);
    buf.setWidth(image.width());
    buf.setLen(image.height());
    buf.setBytesPerLine(image.bytesPerLine());
    buf.setLastByte(1024);
    buf.setIsLast(false);
    while(1){
        writeBytes += 1024;
        if(writeBytes >= countBytes){
            //buf.setText((char*)image.bits()+buf.getId()*1024,countBytes-buf.getId()*1024);//首地址偏移到上次发送结束为止，长度为总长度减去之前发送的所有数据
            buf.setText((char*)image.bits()+writeBytes-1024,countBytes-(writeBytes-1024));
            buf.setIsLast(true);
            buf.setLastByte(countBytes-(writeBytes-1024));
        }else{
            //buf.setText((char*)image.bits()+buf.getId()*1024,1024);
            buf.setText((char*)image.bits()+(writeBytes-1024),1024);
        }
        udpSendCameraSocket->writeDatagram(buf.getData(),buf.getSize(),room.getHost(),cameraPort);
        buf.addId();
        if(writeBytes>=countBytes){break;}
    }
}

void ChildWidget2::sendAudioData(char *buff, int len)
{
    AudioPack pack;
    pack.setText(buff,len);
    pack.setLen(len);
    udpSendAudioSocket->writeDatagram(pack.getData(),pack.getSize(),room.getHost(),audioPort);
}

void ChildWidget2::on_sendMsgBtn_clicked()
{
    QString msg = ui->lineEdit->text();
    if(msg.isEmpty() == true)
        return;
    messageBroadcast(msg);
    ui->lineEdit->clear();
}

void ChildWidget2::on_quitRoom_clicked()
{
    if(room.getRoomOwner() == userName){ //是房主，删除房间
        closeRoomBroadcast();                  //告知房间成员，房主已经退出
        emit cameraStopSignal();             //关闭摄像头
        emit audioInputStopSignal();        //关闭音频输入
        emit deleteRoomSignal(userName,false); //删除房间列表中对应的房间,不会关闭程序
    }else{
            leftRoomBroadcast();            //不是房主，离开房间，广播离开信息
        emit audioOutputStopSignal();
        emit leftRoomSignal();              //发送离开房间信号
    }

    closeUDPSocket();
    this->hide();
}

void ChildWidget2::on_refreshUserBtn_clicked()
{
    model->clear();
    initial();
    QString userName;
    for(int i = 0; i < userVector.size();++i){
        userName = userVector.at(i);
        if(this->userName ==  userVector.at(i)){
             userName = "【我】"+userName;
        }
        if(this->room.getRoomOwner() ==  userVector.at(i)){
            userName = "【房主】"+userName;
        }else{}

        model->setItem(i,0,new QStandardItem(userName));
    }
    ui->userNum->setText(QString("%1 人").arg(userVector.size()));
}

void ChildWidget2::videoChangedSlot(QPixmap pixmap)
{
    pixmap.scaled(ui->cameraLabel->size());
    ui->cameraLabel->setPixmap(pixmap);
}

void ChildWidget2::on_camerStatusBox_currentIndexChanged(const QString &arg1)
{
    QString select = arg1;
    if(select == QString("开启")){
        emit cameraStartSignal();

    }else {
        emit cameraStopSignal();
        QImage* imag = new QImage;
        imag->load(":new/icon/images/recorder.jpg");
        ui->cameraLabel->setPixmap(QPixmap::fromImage(*imag));
        closeCameraBroadcast();
    }
}

void ChildWidget2::on_comboBox_currentIndexChanged(const QString &arg1)
{
    QString select = arg1;
    if(select == QString("开启")){
        emit audioInputStartSignal();
        openAudioBroadcast();
    }else{
        emit audioInputStopSignal();
        closeAudioBroadcast();
    }
}
