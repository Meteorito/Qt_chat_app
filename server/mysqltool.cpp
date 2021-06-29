#include "mysqltool.h"

MySqlTool* MySqlTool::instance = NULL;
QSqlQuery* MySqlTool::pSqlQuery = NULL;
QSqlDatabase MySqlTool::db =  QSqlDatabase::addDatabase("QMYSQL");

MySqlTool::MySqlTool(QSqlDatabase db)
{
    this->db = db;
}

MySqlTool* MySqlTool::getSQLInstance()
{
    if(instance == NULL){
        instance = new MySqlTool(db);
    }
    return instance;
}
MySqlTool::~MySqlTool(){
    delete[] instance;
    delete[] pSqlQuery;
    pSqlQuery = NULL;
    instance = NULL;
}

bool MySqlTool::connectSQL(QString host, int port, \
                        QString baseName, QString userName, QString pwd){
    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(baseName);
    db.setUserName(userName);
    db.setPassword(pwd);
    if(!db.open()){
        QMessageBox::critical(0,QObject::tr("无法打开数据库！"),"无法建立连接",QMessageBox::Yes);
        return false;
    }else{
        return true;
    }
}

bool MySqlTool::execSQL(QString msgs){
    if(pSqlQuery == NULL){
        pSqlQuery =  new QSqlQuery(db);
    }
    int res =  pSqlQuery->exec(msgs);
    if(!res){
        qDebug()<<"Database name: "<<db.databaseName();
        qDebug()<<"操作失败！error:"<<db.lastError().text();
        return false;
    }else{
        return true;
    }
}

void MySqlTool::closeSQL(){
    db.close();
}
