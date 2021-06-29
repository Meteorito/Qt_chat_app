#include "userdata.h"

UserData::UserData(QString name,QString pswd):
    name(name),
    pswd(pswd)
{
    qString = "";
}

void UserData::setUserName(QString name){
    this->name = name;
}

QString UserData::getName(){
    return this->name;
}

void UserData::setUserPswd(QString pswd){
    this->pswd = pswd;
}

QString UserData::toQString(){
    qString = name + "," + pswd;
    return qString;
}
