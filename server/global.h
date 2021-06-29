#ifndef GLOBAL_H
#define GLOBAL_H

#include<QDataStream>

const QDataStream::Version VERSION=QDataStream::Qt_5_7; //QDataStreamʹ版本
typedef quint16 MessageSize;
namespace MessageType{
    enum MessageType{
        LOGIN=0x0001,
        REGISTER=0x0002,
        USERLIST=0x0004,
        ROOMLIST=0x0008,
        JOINROOM=0x0010,
        Message=0x0011,
        LeftRoom=0x0012,
        CreateRoom=0x0014,
        DeleteRoom=0x0018,
        Quit = 0x0020
    };
typedef enum MessageType MessageType;
}

#endif // GLOBAL_H
