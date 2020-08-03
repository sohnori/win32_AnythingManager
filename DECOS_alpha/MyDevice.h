#pragma once
#include <Windows.h>
#include <cstring>

namespace MyDeviceEnum
{
	enum dType {
		dNone=0 , CoordinatorIoT, Inverter, etcDevice, HostPC=255
	};
	enum tType{
		tNone=0, Serial, TCP_IP
	};
}

using namespace MyDeviceEnum;

struct tagDeviceInfo {
	TCHAR deviceName[MAX_PATH];
	dType deviceType;
	BOOL flagSimulator;
	TCHAR terminalName[MAX_PATH];
	tType terminalType;
	BOOL flagTerminal;
	INT attribute;
	UCHAR index;
};

class MyDevice 
{
private:
	struct tagDeviceInfo deviceInfo;
public:
	TCHAR buffStr[MAX_PATH];
public:
	MyDevice();
	MyDevice(const tagDeviceInfo& dev);
	tagDeviceInfo GetDeviceInfo();
	INT SetDeviceInfo(const tagDeviceInfo& dev);
	TCHAR *GetDeviceName();
	INT SetDeviceName(const TCHAR *src);
	dType GetDeviceType();
	INT SetDeviceType(dType devType);
	TCHAR *GetDeviceTerminalName();
	INT SetDeviceTerminalName(const TCHAR *src);
	tType GetDeviceTerminalType();
	INT SetDeviceTerminalType(tType terminalType);
	BOOL GetDeviceTerminalFlag();
	INT SetDeviceTerminalFlag(BOOL value);
	VOID ResetDeviceTerminal();
	UCHAR GetDeviceIndex();
	INT SetDeviceIndex(UCHAR value);
};
