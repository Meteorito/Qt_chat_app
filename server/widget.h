#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDataStream>
#include <QMutex>

#include <QMap>
#include <QVector>
#include <QList>
#include <QTimer>

#include <QDebug>

#include "TCPSocket.h"
#include "mysqltool.h"
#include "global.h"
#include "userdata.h"
#include "room.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void threadFinish();

signals:

public slots:
    void newConnectSlot();
    void tcpWriteSlot(QTcpSocket* socket,QByteArray buff);
    void closeThread(QTcpSocket* socket);

private:
    Ui::Widget *ui;
    QTcpServer* server;
    MySqlTool* pSqlTool;
    QMutex mutex;
    QMap<QString,UserData> map;

    QMap<QTcpSocket*,QThread*> threadMap;
    QMap<QString,Room> roomMap;

};

#endif // WIDGET_H
