#include "myabstractvideosurface.h"

MyAbstractVideoSurface::MyAbstractVideoSurface(QObject* parent):
    QAbstractVideoSurface(parent)
{

}

QList<QVideoFrame::PixelFormat> MyAbstractVideoSurface::supportedPixelFormats(
                QAbstractVideoBuffer::HandleType handleType)const
{
    Q_UNUSED(handleType)
    return QList<QVideoFrame::PixelFormat>()<<QVideoFrame::Format_RGB32;
}

bool MyAbstractVideoSurface::present(const QVideoFrame &frame){
    emit videoChanged(frame);
    return true;
}
