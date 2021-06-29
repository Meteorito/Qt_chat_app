#include "cameradrive.h"

cameraDrive::cameraDrive(QObject *parent) : QObject(parent)
{
    status = false;
}

void cameraDrive::videoChangedSlot(QVideoFrame curFrame)
{
    QVideoFrame frame(curFrame);
    frame.map(QAbstractVideoBuffer::ReadOnly);

    QImage image(frame.bits(),
                 frame.width(),
                 frame.height(),
                 QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()));

    QMatrix matrix;
    matrix.rotate(180);
    image = image.transformed(matrix);

    QPixmap pixmap = QPixmap::fromImage(image);
    emit videoPixmapSignal(pixmap);
}

void cameraDrive::cameraInitial()
{
    QCameraInfo info = QCameraInfo::defaultCamera();
    camera = new QCamera(info,this);

    videoSurface = new MyAbstractVideoSurface(this);
    camera->setViewfinder(videoSurface);
    connect(videoSurface,SIGNAL(videoChanged(QVideoFrame)),
            this,SLOT(videoChangedSlot(QVideoFrame)));
    status = false;
}

void cameraDrive::cameraStart()
{
    if(status == false && camera != NULL){
        camera->start();
        status = true;
    }
}

void cameraDrive::cameraStop()
{
    if(status == true && camera != NULL){
         camera->stop();
         status = false;
    }

}
