#ifndef CAMERAPACK_H
#define CAMERAPACK_H

#include <QWidget>
#include <string.h>

class CameraPack
{
private:
    struct Data{
        char text[1024];
        int id;
        int width;
        int length;
        int lastByte;
        int bytesPerLine;
        bool isLast;
    }data;
public:
    CameraPack();
    void setText(char* str,int len){memcpy(data.text,str,len);}
    char* getText(){return data.text;}

    void setId(int id){data.id = id;}
    int getId(){return data.id;}
    void addId(){data.id++;}

    void setWidth(int width){data.width = width;}
    int getWidth(){return data.width;}

    void setLen(int len){data.length = len;}
    int getLen(){return data.length;}

    void setLastByte(int last){data.lastByte = last;}
    int getLastByte(){return data.lastByte;}

    void setIsLast(bool i){data.isLast = i;}
    bool getIsLast(){return data.isLast;}

    void setBytesPerLine(int bytes){data.bytesPerLine = bytes;}
    int getBytesPerLine(){return data.bytesPerLine;}

    char* getData(){return (char*)&data;}
    int getSize(){return sizeof(data);}
};

#endif // CAMERAPACK_H
