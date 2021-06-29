#ifndef CHILDWIDGET1_H
#define CHILDWIDGET1_H

#include <QWidget>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QinputDialog>

#include<QMap>

#include <QVariant>

#include <QAbstractSocket>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QHostAddress>

#include <QDebug>

#include "childwidget2.h"

#include "global.h"
#include "userdata.h"
#include "room.h"

namespace Ui {
class ChildWidget1;
}

class ChildWidget1 : public QWidget
{
    Q_OBJECT

public:
    explicit ChildWidget1(QWidget *parent = 0);
    explicit ChildWidget1(UserData userData,QTcpSocket* tcpSocket);
    ~ChildWidget1();

private:
    void initial();
    void createRoomQuery(QString roomName);
    void joinRoomQuery();

    void handleRoomList(QDataStream &in);
    void handleRefreshRoom();
    void handleCreateRoom(QDataStream &in);
    void handleJoinRoom(QString roomOwner);

public slots:
    void tcpReadSlot();
    void deleteRoomQuery(QString roomOwner,bool exit);
    void leftRoom();

private slots:
    void on_createRoomBtn_clicked();

    void on_RefreshRoomBtn_clicked();

    void on_joinRoomBtn_clicked();

    void on_quitChildWidget1_clicked();

private:
    Ui::ChildWidget1 *ui;
    QTcpSocket* tcpSocket;

    QUdpSocket* roomUdpSocket;
    QHostAddress* roomHost;
    int roomPort;

    QUdpSocket* udpSocket;

    QStandardItemModel* model;
    UserData userData;
    QMap<QString,Room> roomMap;

    ChildWidget2* widget2;

};

#endif // CHILDWIDGET1_H
