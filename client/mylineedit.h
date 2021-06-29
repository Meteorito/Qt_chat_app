#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>

class MyLineEdit:public QLineEdit
{
    Q_OBJECT
public:
    MyLineEdit(QWidget* parent = 0);
    virtual void keyPressEvent(QKeyEvent* ev);

signals:
    void pressEnter();

};

#endif // MYLINEEDIT_H
