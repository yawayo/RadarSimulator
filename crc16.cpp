#include "crc16.h"


unsigned short calCRC(unsigned char *buf, int len)
{
    //char data[32] = {0, };
    //memcpy(data, buf, len);
    unsigned short crc = 0;
    for(int counter = 0; counter < len; counter++)
        crc = (crc<<8) ^ crc16Table[((crc>>8) ^ *buf++)&0x00FF];
    return crc;
}
