#include "serialclass.h"

SerialClass::SerialClass()
{
    serialStatus = false;
}

SerialClass::~SerialClass()
{
    Close();
}

bool SerialClass::Open(int n_Port, int Baudrate)
{
    if(serialStatus)
        return true;

    wchar_t Port[25] = {0, };
    swprintf(Port, sizeof(Port) / sizeof(wchar_t), L"\\\\.\\COM%d", n_Port);
    IDComDev = CreateFileW(Port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if(IDComDev == NULL)
        return false;

    COMMTIMEOUTS CommTimeouts;
    CommTimeouts.ReadIntervalTimeout = 0xFFFFFFFF;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 5000;
    SetCommTimeouts(IDComDev, &CommTimeouts);

    memset(&overLappedRead, 0, sizeof(OVERLAPPED));
    memset(&overLappedWrite, 0, sizeof(OVERLAPPED));
    overLappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    overLappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    DCB dcb;// = {0, };
    dcb.DCBlength = sizeof(DCB);
    GetCommState(IDComDev, &dcb);
    switch(Baudrate)
    {
    case 9600:
        dcb.BaudRate = CBR_9600;
        break;
    case 19200:
        dcb.BaudRate = CBR_19200;
        break;
    case 115200:
        dcb.BaudRate = CBR_115200;
        break;
    }
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

//    dcb.fBinary           = true;
//    dcb.fDsrSensitivity   = false;
//    dcb.fParity           = NOPARITY;
//    dcb.fOutX             = false;
//    dcb.fInX              = false;
//    dcb.fNull             = false;
//    dcb.fAbortOnError     = true;
//    dcb.fOutxCtsFlow      = false;
//    dcb.fOutxDsrFlow      = false;
//    dcb.fDtrControl       = DTR_CONTROL_DISABLE;
//    dcb.fDsrSensitivity   = false;
//    dcb.fRtsControl       = RTS_CONTROL_DISABLE;
//    dcb.fOutxCtsFlow      = false;

    if((SetCommState(IDComDev, &dcb) == false) || (SetupComm(IDComDev, 4096, 4096) == false) || (overLappedRead.hEvent == NULL) || (overLappedWrite.hEvent == NULL))
    {
        if(overLappedRead.hEvent != NULL)
            CloseHandle(overLappedRead.hEvent);
        if(overLappedWrite.hEvent != NULL)
            CloseHandle(overLappedWrite.hEvent);
        if(IDComDev != NULL)
            CloseHandle(IDComDev);
        return false;
    }
    serialStatus = true;
    return true;
}

bool SerialClass::Close()
{
    if((serialStatus = false)||(IDComDev == NULL))
        return true;

    if(overLappedRead.hEvent != NULL)
        CloseHandle(overLappedRead.hEvent);
    if(overLappedWrite.hEvent != NULL)
        CloseHandle(overLappedWrite.hEvent);
    if(IDComDev != NULL)
        CloseHandle(IDComDev);

    serialStatus = false;
    IDComDev = NULL;
    return true;
}

bool SerialClass::writeCommByte(unsigned char ucByte)
{
    BOOL writeStatus = false;
    DWORD bytesWritten;
    writeStatus = WriteFile(IDComDev, (LPSTR)&ucByte, 1, &bytesWritten, &overLappedWrite);
    if(!writeStatus && (GetLastError() == ERROR_IO_PENDING))
    {
        if(WaitForSingleObject(overLappedWrite.hEvent, 1000))
            bytesWritten = 0;
        else
        {
            GetOverlappedResult(IDComDev, &overLappedWrite, &bytesWritten, FALSE);
            overLappedWrite.Offset += bytesWritten;
        }
    }
    return true;
}
size_t SerialClass::writeData(char *buffer, size_t size)
{
    if((serialStatus == false) || (IDComDev == NULL))
        return 0;

    DWORD bytesWritten = 0;

    for(int i=0; i<size; i++)
    {
        writeCommByte(buffer[i]);
        bytesWritten++;
    }
    return size_t(bytesWritten);
}
size_t SerialClass::readData(char *buffer, size_t limit)
{
    if((serialStatus == false) || (IDComDev == NULL))
        return 0;

    BOOL readStatus;
    DWORD bytesRead, ErrorFlags;
    COMSTAT comStat;
    ClearCommError(IDComDev, &ErrorFlags, &comStat);

    if(!comStat.cbInQue)
        return 0;
    bytesRead = (DWORD)comStat.cbInQue;
    if(int(limit) < int(bytesRead))
        bytesRead = DWORD(limit);
    readStatus = ReadFile(IDComDev, buffer, bytesRead, &bytesRead, &overLappedRead);

    if(!readStatus)
    {
        if(GetLastError() == ERROR_IO_PENDING)
        {
            WaitForSingleObject(overLappedRead.hEvent, 100);
            return int(bytesRead);
        }
        return 0;
    }
    return size_t(bytesRead);
}

char SerialClass::readOneByte()
{
    if((serialStatus == false) || (IDComDev == NULL))
        return NULL;

    BOOL readStatus;
    DWORD bytesRead, ErrorFlags;
    COMSTAT comStat;
    ClearCommError(IDComDev, &ErrorFlags, &comStat);

    if(!comStat.cbInQue)
        return 0;
    bytesRead = (DWORD)comStat.cbInQue;
    if(bytesRead <= 0)
        return NULL;
    char buffer = ' ';
    DWORD size = 1;
    readStatus = ReadFile(IDComDev, &buffer, size, &size, &overLappedRead);
    if(!readStatus)
    {
        if(GetLastError() == ERROR_IO_PENDING)
        {
            WaitForSingleObject(overLappedRead.hEvent, 100);
            return NULL;
        }
        return NULL;
    }
    return buffer;
}


bool SerialClass::isOpened()
{
    return serialStatus;
}

