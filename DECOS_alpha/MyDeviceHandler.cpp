#include "MyDeviceHandler.h"

INT MyDeviceHandler::NewMyDevice(tagDeviceInfo &dev, UCHAR numDevice)
{
	this->MyDev[numDevice] = new MyDevice(dev);
	this->flagIsDevice[numDevice] = TRUE;
	this->cntDevice++;
	return numDevice;
}

INT MyDeviceHandler::DelMyDevice(UCHAR numDevice)
{
	delete this->MyDev[numDevice];
	this->flagIsDevice[numDevice] = FALSE;
	this->cntDevice--;
	return numDevice;
}

INT MyDeviceHandler::DelMyDevices()
{
	INT cnt;
	INT cnt1 = 0;
	for (cnt = 0; cnt < MYDEVICE_COUNT_MAX; cnt++) {
		if (this->flagIsDevice[cnt] == TRUE) { delete this->MyDev[cnt]; cnt1++; }
	}
	this->cntDevice = 0;
	return cnt1;
}

INT MyDeviceHandler::GetDeviceNum(TCHAR *strSrc)
{
	INT cnt;
	for (cnt = 0; cnt < MYDEVICE_COUNT_MAX; cnt++) {
		if (this->flagIsDevice[cnt] == TRUE && strcmp(this->MyDev[cnt]->GetDeviceName(), strSrc) == 0) return cnt;
	}
	return -1;
}

INT MyDeviceHandler::GetDeviceNum(UCHAR index)
{
	INT cnt;
	for (cnt = 0; cnt < MYDEVICE_COUNT_MAX; cnt++) {
		if (this->flagIsDevice[cnt] == TRUE && this->MyDev[cnt]->GetDeviceIndex()==index) return cnt;
	}
	return -1;
}

BOOL MyDeviceHandler::GetFlagIsDevice(UCHAR num)
{
	return this->flagIsDevice[num];
}

UCHAR MyDeviceHandler::GetAvailableMyDevice()
{
	INT cnt;	
	for (cnt = 0; cnt < MYDEVICE_COUNT_MAX; cnt++) {
		if (this->flagIsDevice[cnt] == FALSE) break;
	}
	return (UCHAR)cnt;
}

BOOL MyDeviceHandler::CheckOverlapDevName(TCHAR * name)
{
	TCHAR *devName;
	INT cnt;
	for (cnt = 0; cnt < MYDEVICE_COUNT_MAX; cnt++) {
		if (this->GetFlagIsDevice(cnt) == TRUE) {
			devName = this->MyDev[cnt]->GetDeviceName();
			if (strcmp(devName, name) == 0) return TRUE;
		}
	}	
	return FALSE;
}

INT MyDeviceHandler::ResetMyDeviceTerminals(TCHAR *tnameStrSrc)
{
	INT cnt;
	INT cnt1 = 0;
	TCHAR *strPtr;
	for (cnt = 0; cnt < MYDEVICE_COUNT_MAX; cnt++) {
		if (this->flagIsDevice[cnt] == TRUE) {
			strPtr = this->MyDev[cnt]->GetDeviceTerminalName();
			if (strcmp(strPtr, tnameStrSrc) == 0) { this->MyDev[cnt]->ResetDeviceTerminal(); cnt1++; }
		}		
	}
	return cnt1;
}

MyDeviceHandler::~MyDeviceHandler()
{	
	this->DelMyDevices();
}