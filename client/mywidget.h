#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QMap>
#include <QMessageBox>
#include <QPalette>

#include <QHostAddress>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDataStream>

#include "global.h"

#include "userdata.h"
#include "childwidget1.h"
#include "childwidget2.h"

#include <QDebug>
#include <QCloseEvent>

namespace Ui {
class myWidget;
}

class myWidget : public QWidget
{
    Q_OBJECT

public:
    explicit myWidget(QWidget *parent = 0);
    ~myWidget();

    virtual void closeEvent(QCloseEvent* ev);

public slots:
    bool connectSlot();
    void readDataSlot();
    void signIn(void);//登录
    void signUp(void);//注册

    void SignInSuccessSlot();
    void SignInFailSlot();

    void SignUpFailSlot();
    void SignUpSuccessSlot();

    void connectError(QAbstractSocket::SocketError);

signals:
//    登录信号
    void SignInSuccess(QTcpSocket*);
    void SignInFail();
//  注册信号
    void SignUpSuccess();
    void SignUpFail();


private slots:
    void on_radioButton_clicked();

private:
    Ui::myWidget *ui;
    QTcpSocket* socket;
    UserData userData;
    ChildWidget1* widget1;
};

#endif // MYWIDGET_H
