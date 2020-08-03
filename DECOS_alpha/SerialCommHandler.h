#pragma once
#include "SerialComm.h"

#define SIZE_ERRMSG			256
#define SERIALPORT_COUNT_MAX	256

struct tagDCBInfo {
	TCHAR portName[32 + 1];
	INT baudrate;
	TCHAR dataBits;
	TCHAR stopBits;
	TCHAR parity;
	TCHAR flowControl;
	DWORD timeout = 500;
};

struct tagDCBInfoStr {
	TCHAR portName[32 + 1];
	TCHAR baudrate[10];
	TCHAR dataBits[2];
	TCHAR stopBits[4];
	TCHAR parity[10];
	TCHAR flowControl[10];
	TCHAR timeout[10];
};

class SerialCommHandler
{
private:
	BOOL flagConnectedPort[SERIALPORT_COUNT_MAX] = { 0, };
	BOOL flagAvailablePort[SERIALPORT_COUNT_MAX] = { 0, };
	TCHAR errmsg[SIZE_ERRMSG] = "";
public:
	SerialComm * SerialPort[SERIALPORT_COUNT_MAX];
private:
	void ResetFlagAvailablePort();
	INT SetFlagAvailablePort();
public:
	SerialCommHandler();
	BOOL GetFlagConnectedPort(UCHAR portNum);
	BOOL GetFlagAvailablePort(UCHAR portNum);
	INT GetAvailablePortNum(TCHAR *dest, INT cbDest);	
	BOOL ConnectPort(tagDCBInfo DCBInfo);	
	INT GetConnectedPortNums(TCHAR *dest, INT cbDest);
	INT GetConnectedPortNum(TCHAR *strSrc);
	UCHAR GetConnectedPortFirstNum();
	tagDCBInfo GetDCBInfo(UCHAR indexPort);
	BOOL IsConnectedPort(TCHAR *strSrc);
	INT ReflashConnectedPort();
	BOOL ClosePort(UCHAR indexPort);
	BOOL ClosePort(TCHAR *strSrc);
	INT ClosePorts();
	TCHAR*GetErrMSGPtr();
	~SerialCommHandler();
};
