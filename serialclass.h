#ifndef SERIALCLASS_H
#define SERIALCLASS_H
#include <windows.h>
#include <stdio.h>

#include <sstream>

#include <QDebug>

#define STX                     0xFFFFFFFE
#define ETX                     0xFF

#define PD_INOUT                0x01
#define PD_INDEX                0x02
#define DANGER_LEVEL            0x04
#define CHECK_CONNECTION        0x05

#define PD_INOUT_ACK            0x11
#define DANGER_LEVEL_ACK        0x14
#define CHECK_CONNECTION_ACK    0x15

#define IN                      0x02
#define OUT                     0x01

#define NACK                    0x30
#define ACK                     0x31

#define MAX_SLAVE_NUM 25
#define RESEND_TIME 3

class SerialClass
{
public:
    SerialClass();
    ~SerialClass();

    bool Open(int n_Port, int Baudrate);
    bool Close(void);
    bool writeCommByte(unsigned char ucByte);
    size_t writeData(char *buffer, size_t size);
    size_t readData(char *buffer, size_t limit);
    char readOneByte();
    bool isOpened(void);


protected:
    HANDLE IDComDev;
    OVERLAPPED overLappedRead, overLappedWrite;
    bool serialStatus;

};

#endif // SERIALCLASS_H
