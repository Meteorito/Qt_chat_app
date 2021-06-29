#include "userdata.h"

UserData::UserData(QString name,QString pswd):
    name(name),
    pswd(pswd)
{
    status = false;
    qString = "";
}

void UserData::setUserName(QString name){
    this->name = name;
}

QString UserData::getUserName()
{
    return this->name;
}

void UserData::setUserPswd(QString pswd){
    this->pswd = pswd;
}

QString UserData::getUserPswd()
{
    return this->pswd;
}

void UserData::setStatus(bool status)
{
    this->status = status;
}

bool UserData::getStatus()
{
    return this->status;
}

void UserData::setSocket(QTcpSocket *socket)
{
    this->socket = socket;
}

QTcpSocket *UserData::getSocket()
{
    return this->socket;
}

QString UserData::toQString(){
    qString = name + "," + pswd;
    return qString;
}
