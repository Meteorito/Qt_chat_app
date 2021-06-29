#ifndef GLOBAL_H
#define GLOBAL_H

#include<QDataStream>

const QDataStream::Version VERSION=QDataStream::Qt_5_7; //QDataStreamʹ版本
typedef quint16 MessageSize;
namespace MessageType{
    enum MessageType{
        LOGIN=0x0001,           //tcp
        REGISTER=0x0002,        //tcp
        USERLIST=0x0004,
        ROOMLIST=0x0008,        //tcp

        JOINROOM=0x0010,        //udp
        Message=0x0011,         //udp
        LeftRoom=0x0012,        //udp

        CreateRoom=0x0014,    //tcp

        DeleteRoom=0x0018,      //udp

        RoomUserList = 0x0020,  //udp

        CloseCamera = 0x0022,//udp
        OpenCamera = 0x0024,//udp

        CloseAudio = 0x0028,//udp
        OpenAudio = 0x0030//udp
    };
typedef enum MessageType MessageType;
}

#endif // GLOBAL_H
