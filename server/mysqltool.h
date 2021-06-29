#ifndef MYSQLTOOL_H
#define MYSQLTOOL_H

#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QDebug>
class MySqlTool
{
private:
    MySqlTool(QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"));
   static  MySqlTool* instance;

   static QSqlDatabase db;

public:
    static QSqlQuery* pSqlQuery;
    ~MySqlTool();
    static MySqlTool* getSQLInstance();

    static bool connectSQL(QString host,int port, \
                 QString baseName,QString userName,QString pwd);
    static bool execSQL(QString msgs);
    static void closeSQL();
};
#endif // MYSQLTOOL_H
