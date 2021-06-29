#include "audiopack.h"

AudioPack::AudioPack()
{

}

char *AudioPack::getText()
{
    return data.text;
}

void AudioPack::setText(char *str, int len)
{
    memcpy(data.text,str,len);
}

int AudioPack::getLen()
{
    return data.len;
}

void AudioPack::setLen(int len)
{
    data.len = len;
}

char *AudioPack::getData()
{
    return (char*)&data;
}

int AudioPack::getSize()
{
    return sizeof(data);
}

