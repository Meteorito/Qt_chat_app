#ifndef ROOM_H
#define ROOM_H

#include <QString>
#include <QHostAddress>

class Room
{
public:
    Room();

    QString getRoomName();
    void setRoomName(QString roomName);

    QString getRoomOwner();
    void setRoomOwner(QString roomOwner);

    QHostAddress getHost();
    void setHost(QHostAddress host);

    int getPort();
    void setPort(int port);

private:
    QString roomName;
    QString roomOwner;
    QHostAddress host;
    int port;
};

#endif // ROOM_H
