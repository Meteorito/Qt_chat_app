#ifndef USERDATA_H
#define USERDATA_H

#include <QString>
#include <QTcpSocket>

class UserData
{
public:
    explicit UserData(QString name="",QString pswd = "");
    void setUserName(QString name);
    QString getUserName();

    void setUserPswd(QString pswd);
    QString getUserPswd();

    void setStatus(bool status);
    bool getStatus();

    void setSocket(QTcpSocket* socket);
    QTcpSocket* getSocket();

    QString toQString();

private:
    QString name;
    QString pswd;
    bool status;

    QTcpSocket* socket;

    QString qString;
};

#endif // USERDATA_H
