#include "TCPSocket.h"

TCPSocket::TCPSocket(QWidget *parent)
{
}

void TCPSocket::work(){
    if(tcpSocket == NULL)
        return;

    QByteArray buff;
    QDataStream in(&buff,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_7);

    QByteArray writeBuff;
    QDataStream out(&writeBuff,QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_5_7);

    mutex->lock();
    buff = tcpSocket->readAll();
    mutex->unlock();

    int type;
    in>>type;
    qDebug()<<type;
    switch (type) {
    case MessageType::LOGIN:
        out<<MessageType::LOGIN<<UserLogin(in);
        break;
    case MessageType::REGISTER:
        out<<MessageType::REGISTER<<UserRegister(in);
        break;
    case MessageType::ROOMLIST:
        out<<MessageType::ROOMLIST;
        updateRoomInfo(out);
        break;
    case MessageType::CreateRoom:{
        QString roomName;
        QString userName;
        in>>userName>>roomName;
        out<<MessageType::CreateRoom;
        createRoom(out,userName,roomName);
        break;
    }
    case MessageType::DeleteRoom:{
        QString roomOwner;
        in>>roomOwner;
        deleteRoom(roomOwner);
        //更新客户端房间信息
        out<<MessageType::ROOMLIST;
        updateRoomInfo(out);
        break;
    }

    default:
        break;
    }
    qDebug()<<writeBuff;
    emit writeSignal(tcpSocket,writeBuff);//发送写的数据

}

void TCPSocket::socketDisconnectSlot()
{
    QMap<QString,UserData>::iterator it = userInfoMap->find(userData.getUserName());
    if(it != userInfoMap->end()){
        it.value().setStatus(false);
    }
    QMap<QString,Room>::Iterator roomIt = roomInfoMap->find(userData.getUserName());
    if(roomIt != roomInfoMap->end()){
        roomInfoMap->remove(it.key());
    }
}

bool TCPSocket::UserLogin(QDataStream &in)
{
    QString name;
    QString pswd;
    in>>name>>pswd;
    QMap<QString,UserData>::iterator it = userInfoMap->find(name);

    if(it != userInfoMap->end() && it.value().getUserPswd() == pswd){//是否需要锁？
        if(it.value().getStatus() == false){ //用户没有登录
            it.value().setStatus(true);
            userData = it.value();
            return true;
        }else{                                      //用户已经登录
            return false;
        }
    }else{
        return false;
    }
}

bool TCPSocket::UserRegister(QDataStream &in)
{
    QString name;
    QString pswd;
    MySqlTool* pSQLTool = MySqlTool::getSQLInstance();
    int ret;
    in>>name>>pswd;
    QMap<QString,UserData>::iterator it = userInfoMap->find(name);
    if(it ==  userInfoMap->end()){
        UserData userData(name,pswd);
        QString execMsg = QString("insert into tb1(name,pwd) values ('%1','%2');")
                                    .arg(name).arg(pswd);

        mutex->lock();
        ret = pSQLTool->execSQL(execMsg);
        mutex->unlock();

        if(ret == true){
            userInfoMap->insert(name,userData);
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

//房间数量<<     房主<<房间名<<房间IP地址<<房间端口  <<....
QDataStream &TCPSocket::updateRoomInfo(QDataStream &out)
{
    out<<roomInfoMap->size();
    QMap<QString,Room>::iterator it = roomInfoMap->begin();
    for(;it != roomInfoMap->end();++it){
        out<<it.key()<<it.value().getRoomName();
        out<<it.value().getHost().toIPv4Address()<<it.value().getPort();
    }
    return out;

}

//回应状态<<房间名<<房间IP地址<<端口
QDataStream &TCPSocket::createRoom(QDataStream &out,QString userName,QString roomName)
{
    if(roomInfoMap->find(userName) != roomInfoMap->end()){  //该用户创建了房间
        out<<false<<QString("请退出原有的房间");
        return out;
    }

    Room room;
    quint32 host;
    host = QHostAddress("224.0.0.1").toIPv4Address()+(quint32)(roomInfoMap->size()+1);
    room.setRoomOwner(userName);
    room.setRoomName(roomName);
    QHostAddress HOST(host);
    room.setHost(HOST);
    qDebug()<<"quint32:"<<host;
    qDebug()<<"HOST:"<<HOST.toIPv4Address();
    qDebug()<<"host:"<<room.getHost().toIPv4Address();
    roomInfoMap->insert(userName,room);

    out<<true<<roomName<<room.getHost().toIPv4Address()<<room.getPort();
    return out;

}

void TCPSocket::deleteRoom(QString roomOwner)
{
     roomInfoMap->remove(roomOwner);
}

void TCPSocket::setUserInfoMap(QMap<QString, UserData> *userInfoMap)
{
    this->userInfoMap = userInfoMap;
}

void TCPSocket::setRoomInfoMap(QMap<QString, Room> *roomInfoMap)
{
    this->roomInfoMap = roomInfoMap;
}

void TCPSocket::setMutex(QMutex *mutex)
{
    this->mutex = mutex;
}

void TCPSocket::settcpSocket(QTcpSocket *tcpSocket)
{
    this->tcpSocket = tcpSocket;
    //connect(tcpSocket,SIGNAL(readyRead()),this,SLOT());
}

QTcpSocket *TCPSocket::gettcpSocket()
{
    return tcpSocket;
}
