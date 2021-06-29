#ifndef CAMERADRIVE_H
#define CAMERADRIVE_H

#include <QObject>
#include <QCameraInfo>
#include <QCamera>
#include <QPixmap>

#include <QDebug>
#include <QThread>

#include "myabstractvideosurface.h"



class cameraDrive : public QObject
{
    Q_OBJECT
public:
    explicit cameraDrive(QObject *parent = 0);


signals:
   void videoPixmapSignal(QPixmap);
public slots:
    void videoChangedSlot(QVideoFrame curFrame);
    void cameraInitial();
    void cameraStart();
    void cameraStop();


private:
    QCamera* camera;
    MyAbstractVideoSurface* videoSurface;
    bool status;
};

#endif // CAMERADRIVE_H
