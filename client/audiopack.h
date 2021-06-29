#ifndef AUDIOPACK_H
#define AUDIOPACK_H

#include <string.h>

class AudioPack
{
private:
    struct Data{
        char text[1024];
        int len;
    }data;

public:
    AudioPack();

    char* getText();
    void setText(char* str,int len);

    int getLen();
    void setLen(int len);

    char* getData();
    int getSize();

};

#endif // AUDIOPACK_H
