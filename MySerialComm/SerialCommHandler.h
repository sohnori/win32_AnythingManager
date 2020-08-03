#pragma once
#include "SerialComm.h"

#define SIZE_ERRMSG			256

struct tagDCBInfo {
	TCHAR portName[32 + 1];
	INT baudrate;
	TCHAR dataBits;
	TCHAR stopBits;
	TCHAR parity;
	TCHAR flowControl;
	DWORD timeout = 500;
};

class SerialCommHandler
{
private:
	BOOL flagConnectedPort[256] = { 0, };
	BOOL flagAvailablePort[256] = { 0, };
	TCHAR errmsg[SIZE_ERRMSG] = "";
public:
	SerialComm * SerialPort[256];
private:
	void ResetFlagAvailablePort();
	INT SetFlagAvailablePort();
public:
	SerialCommHandler();	
	INT GetAvailablePortNum(TCHAR *dest, INT cbDest);	
	BOOL ConnectPort(tagDCBInfo DCBInfo, UCHAR indexPort);	
	INT GetConnectedPortNum(TCHAR *dest, INT cbDest);
	UCHAR GetConnectedPortNum();	
	INT ReflashConnectedPort();
	BOOL ClosePort(UCHAR indexPort);
	INT ClosePorts();
	TCHAR*GetErrMSGPtr();
	~SerialCommHandler();
};
