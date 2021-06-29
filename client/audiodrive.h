#ifndef AUDIODRIVE_H
#define AUDIODRIVE_H

#include <QObject>
#include <QAudio>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QDebug>

class AudioDrive:public QObject
{
    Q_OBJECT
public:
    explicit AudioDrive(QObject* parent = 0);

public slots:
    void audioDriveInitial();

    void audioInputStart();
    void audioInputStop();

    void audioOutputStart();
    void audioOutputStop();

    void audioDataSlot(char* buff,int len);

signals:
    void audioDataSignal(char* buff,int len);

private slots:
    void readDevice();

private:
    QAudioFormat audioFormat;
    QAudioInput* audioInput;
    QAudioOutput* audioOutput;

    QIODevice* inputDevice;
    QIODevice* outputDevice;

    bool read;

};

#endif // AUDIODRIVE_H
