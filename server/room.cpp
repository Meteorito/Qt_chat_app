#include "room.h"

Room::Room()
{
    this->port = 12345;
    this->host = QHostAddress("224.0.0.1");
}

QString Room::getRoomName()
{
    return this->roomName;
}

void Room::setRoomName(QString roomName)
{
    this->roomName = roomName;
}

QString Room::getRoomOwner()
{
    return this->roomOwner;
}

void Room::setRoomOwner(QString roomOwner)
{
    this->roomOwner = roomOwner;
}

QHostAddress Room::getHost()
{
    return this->host;
}

void Room::setHost(QHostAddress host)
{
    this->host = host;
}

int Room::getPort()
{
    return this->port;
}

void Room::setPort(int port)
{
    this->port = port;
}
