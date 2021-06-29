#include "audiodrive.h"

AudioDrive::AudioDrive(QObject* parent):QObject(parent)
{

}

void AudioDrive::audioDriveInitial()
{
    audioFormat.setSampleRate(8000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(16);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    audioInput = new QAudioInput(audioFormat);
    audioOutput = new QAudioOutput(audioFormat);
    read = false;
}

void AudioDrive::audioInputStart()
{
    if(audioInput == NULL ){
        return;
    }
    inputDevice = audioInput->start();
    connect(inputDevice,SIGNAL(readyRead()),this,SLOT(readDevice()));
}

void AudioDrive::readDevice()
{
    char buff[1024];
    inputDevice->read(buff,1024);
    emit audioDataSignal(buff,1024);
}

void AudioDrive::audioInputStop()
{
    if(audioInput != NULL){
        audioInput->stop();
    }
}

void AudioDrive::audioOutputStart()//没什么作用
{
    if(audioOutput == NULL)
        return;
    outputDevice = audioOutput->start();
    read = true;
}

void AudioDrive::audioOutputStop()
{
    if(audioOutput != NULL){
        audioOutput->stop();
        read = false;
    }
}

void AudioDrive::audioDataSlot(char *buff, int len)
{
    if(read == false)
        return;
    outputDevice->write(buff,len);
}

