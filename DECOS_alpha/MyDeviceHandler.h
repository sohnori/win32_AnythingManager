#pragma once
#include <Windows.h>
#include "MyDevice.h"

#define MYDEVICE_COUNT_MAX	256

class MyDeviceHandler
{
private:
	BOOL flagIsDevice[MYDEVICE_COUNT_MAX] = { 0, };
	UCHAR cntDevice = 0;
public:
	MyDevice * MyDev[MYDEVICE_COUNT_MAX];
public:
	INT NewMyDevice(tagDeviceInfo &dev, UCHAR numDevice);
	INT DelMyDevice(UCHAR numDevice);
	INT DelMyDevices();
	INT GetDeviceNum(TCHAR *strSrc);
	INT GetDeviceNum(UCHAR index);
	BOOL GetFlagIsDevice(UCHAR num);
	UCHAR GetAvailableMyDevice();
	BOOL CheckOverlapDevName(TCHAR * name);
	INT ResetMyDeviceTerminals(TCHAR *tnameStrSrc);
	~MyDeviceHandler();
};
