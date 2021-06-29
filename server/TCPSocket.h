#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QWidget>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMutex>

#include <QDataStream>
#include <QDebug>

#include "global.h"
#include "userdata.h"
#include "room.h"
#include "mysqltool.h"

class TCPSocket : public QObject
{
    Q_OBJECT
public:
    TCPSocket(QWidget* parent = 0);

    void setUserInfoMap(QMap<QString,UserData>* userInfoMap);
    void setRoomInfoMap(QMap<QString,Room>* roomInfoMap);
    void setMutex(QMutex* mutex);

    void settcpSocket(QTcpSocket* tcpSocket);
    QTcpSocket* gettcpSocket();

signals:
    writeSignal(QTcpSocket* socket,QByteArray buff);
    closeTCPSocket(QTcpSocket* closeSocket);

public slots:
    void work();
    void socketDisconnectSlot();

private:
    bool UserLogin(QDataStream &in);
    bool UserRegister(QDataStream &in);
    QDataStream& updateRoomInfo(QDataStream &out);
    QDataStream& createRoom(QDataStream &out,QString userName,QString roomName);
    void deleteRoom(QString roomOwner);

private:
    QMutex* mutex;
    QTcpSocket* tcpSocket;

    UserData userData;
    QMap<QString,UserData>* userInfoMap;
    QMap<QString,Room>* roomInfoMap;

};

#endif // CONTRALCMDTHREAD_H
