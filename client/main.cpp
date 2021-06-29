#include "mywidget.h"
#include "childwidget1.h"
#include "childwidget2.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    myWidget w;
    w.show();

    return a.exec();
}
