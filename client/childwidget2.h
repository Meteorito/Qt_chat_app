#ifndef CHILDWIDGET2_H
#define CHILDWIDGET2_H

#include <QWidget>
#include <QStringListModel>

#include <QHostAddress>
#include <QAbstractSocket>
#include <QUdpSocket>
#include <QDataStream>
#include <QVector>

#include <QThread>

#include <QMessageBox>
#include <QCloseEvent>

#include <QDebug>

#include <QtGlobal>
#include <QTime>
#include <QPropertyAnimation>

#include<QStandardItemModel>
#include "room.h"
#include "global.h"

#include "audiodrive.h"
#include "cameradrive.h"
#include "camerapack.h"
#include "audiopack.h"

namespace Ui {
class ChildWidget2;
}

class ChildWidget2 : public QWidget
{
    Q_OBJECT

public:
    explicit ChildWidget2(QWidget *parent = 0);
    explicit ChildWidget2(Room room,QString userName);
    ~ChildWidget2();

    void setRoom(Room room);
    void setUserName(QString userName);

    virtual void closeEvent(QCloseEvent *event);
private:
    void initial();
    void cameraSetInitial();
    void audioSetInitial();

    void joinRoomBroadcast();
    void leftRoomBroadcast();
    void closeRoomBroadcast();
    void UserVectorBroadcast();
    void messageBroadcast(QString msg);

    void closeCameraBroadcast();
    void closeAudioBroadcast();
    void openAudioBroadcast();

    void quitRoom();
    void closeUDPSocket();

    void UserVectorRcv(QDataStream &in);
    void messageRcv(QDataStream &in);

    void barrageMsg(QString msg);

public slots:
    void udpCmdReadSlot();

    void msgBrowserAutoScroll();

    void sendCameraData(QPixmap pixmap);
    void sendAudioData(char* buff,int len);

    void cameraRcv();
    void audioRcv();

private slots:
    void on_sendMsgBtn_clicked();

    void on_quitRoom_clicked();
    void on_refreshUserBtn_clicked();

    void videoChangedSlot(QPixmap pixmap);

    void on_camerStatusBox_currentIndexChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(const QString &arg1);

signals:
    deleteRoomSignal(QString roomOwner,bool exit);
    leftRoomSignal();

    cameraInitialSignal();
    cameraStartSignal();
    cameraStopSignal();

    audioInitialSignal();
    audioInputStartSignal();
    audioInputStopSignal();
    audioOutputStartSignal();
    audioOutputStopSignal();
    audioPlaySignal(char*,int);

private:
    Ui::ChildWidget2 *ui;

    QStandardItemModel* model;

    QString userName;
    Room room;
    QVector<QString> userVector;

    QThread* audioThread;
    AudioDrive* audio;

    QThread* cameraThread;
    cameraDrive* camera;

    QUdpSocket* udpAudioSocket;//?????????????????????
    int audioPort; //????????????
    QUdpSocket* udpCameraSocket;//????????????????????????
    int cameraPort;//???????????????
    QUdpSocket* udpCmdSocket;//?????????????????????
    int cmdPort;//????????????

    QUdpSocket* udpSendSocket;//???????????????????????????
    QUdpSocket* udpSendAudioSocket;//?????????????????????
    QUdpSocket* udpSendCameraSocket;//????????????????????????

};

#endif // CHILDWIDGET2_H
