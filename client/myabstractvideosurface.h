#ifndef MYABSTRACTVIDEOSURFACE_H
#define MYABSTRACTVIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QObject>
#include <QThread>
#include <QDebug>

class MyAbstractVideoSurface:public QAbstractVideoSurface
{
    Q_OBJECT
public:
    MyAbstractVideoSurface(QObject* parent =0);
    QList<QVideoFrame::PixelFormat>supportedPixelFormats(
                    QAbstractVideoBuffer::HandleType handleType)const;
    bool present(const QVideoFrame &frame);
signals:
    void videoChanged(QVideoFrame);
};

#endif // MYABSTRACTVIDEOSURFACE_H
