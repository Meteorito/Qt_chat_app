#ifndef USERDATA_H
#define USERDATA_H

#include <QString>

class UserData
{
public:
    explicit UserData(QString name="",QString pswd = "");
    void setUserName(QString name);
    QString getName();

    void setUserPswd(QString pswd);

    QString toQString();

private:
    QString name;
    QString pswd;
    QString qString;
};

#endif // USERDATA_H
