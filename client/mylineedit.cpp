#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget* parent):
    QLineEdit(parent)
{

}

void MyLineEdit::keyPressEvent(QKeyEvent *ev)
{
    QString str;
    if(ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return){
        emit this->pressEnter();
    }else if(ev->key() == Qt::Key_Backspace){
        str = this->text();
        str.chop(1);
        this->setText(str);
    }else{

        QString str = this->text();
        str += ev->text();
        this->setText(str);
    }
}
