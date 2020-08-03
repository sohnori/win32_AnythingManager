#include "MyDevice.h"

MyDevice::MyDevice()
{
	this->deviceInfo.deviceName[0] = NULL;
	this->deviceInfo.deviceType = dNone;
	this->deviceInfo.terminalName[0] = NULL;
	this->deviceInfo.terminalType = tNone;
	this->deviceInfo.flagSimulator = 0;
	this->deviceInfo.flagTerminal = 0;
	this->deviceInfo.index = 0;
	this->deviceInfo.attribute = 0;
}

MyDevice::MyDevice(const tagDeviceInfo& dev) : deviceInfo(dev) {}

tagDeviceInfo MyDevice::GetDeviceInfo()
{
	return deviceInfo;
}

INT MyDevice::SetDeviceInfo(const tagDeviceInfo& dev)
{
	this->deviceInfo = dev;
	return 1;
}

TCHAR *MyDevice::GetDeviceName()
{
	strcpy_s(this->buffStr, this->deviceInfo.deviceName);
	return buffStr;
}

INT MyDevice::SetDeviceName(const TCHAR *src)
{
	if (lstrlen(src) > MAX_PATH) return -1;
	strcpy_s(this->deviceInfo.deviceName, src);
	return 1;
}

dType MyDevice::GetDeviceType()
{
	return this->deviceInfo.deviceType;
}

INT MyDevice::SetDeviceType(dType devType)
{
	this->deviceInfo.deviceType = devType;
	return 1;
}

TCHAR *MyDevice::GetDeviceTerminalName()
{
	strcpy_s(this->buffStr, this->deviceInfo.terminalName);
	return buffStr;
}

INT MyDevice::SetDeviceTerminalName(const TCHAR *src)
{
	if (lstrlen(src) > MAX_PATH) return -1;
	strcpy_s(this->deviceInfo.terminalName, src);
	return 1;
}

tType MyDevice::GetDeviceTerminalType()
{
	return this->deviceInfo.terminalType;
}

INT MyDevice::SetDeviceTerminalType(tType terminalType)
{
	this->deviceInfo.terminalType = terminalType;
	return 1;
}

BOOL MyDevice::GetDeviceTerminalFlag()
{
	return this->deviceInfo.flagTerminal;
}

INT MyDevice::SetDeviceTerminalFlag(BOOL value)
{
	this->deviceInfo.flagTerminal = value;
	return 1;
}

VOID MyDevice::ResetDeviceTerminal()
{
	this->deviceInfo.terminalName[0] = NULL;
	this->deviceInfo.flagTerminal = FALSE;
	this->deviceInfo.terminalType = tNone;
}

UCHAR MyDevice::GetDeviceIndex()
{
	return this->deviceInfo.index;
}

INT MyDevice::SetDeviceIndex(UCHAR value)
{
	this->deviceInfo.index = value;
	return 1;
}
