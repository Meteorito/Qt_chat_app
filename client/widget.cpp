#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_radioButton_clicked()
{
    if(ui->radioButton->isChecked() == true){
        ui->lineEdit_pswd->setEchoMode(QLineEdit::Normal);
    }else{
        ui->lineEdit_pswd->setEchoMode(QLineEdit::Password);
    }
}
