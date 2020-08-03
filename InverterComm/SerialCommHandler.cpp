#include "SerialCommHandler.h"


SerialCommHandler::SerialCommHandler()
{
}

void SerialCommHandler::ResetFlagAvailablePort()
{
	INT cnt;
	for (cnt = 0; cnt < 256; cnt++) {
		this->flagAvailablePort[cnt] = FALSE;
	}
}

INT SerialCommHandler::SetFlagAvailablePort()
{
	TCHAR portName[MAX_PATH] = "";
	INT cnt;
	INT cntAvailablePort = 0;
	HANDLE hSerial;	
	for (cnt = 0; cnt < 256; cnt++) {
		wsprintf(portName, "\\\\.\\COM%d", cnt);
		hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hSerial != INVALID_HANDLE_VALUE) {
			this->flagAvailablePort[cnt] = TRUE;
			CloseHandle(hSerial);
			cntAvailablePort++;
		}
		else {
			;
		}		
	}
	return cntAvailablePort;
}

INT SerialCommHandler::GetAvailablePortNum(TCHAR *dest, INT cbDest)
{
	INT cnt;
	INT cntAvailable = 0;	
	if (cbDest < 256) {
		strcpy_s(this->errmsg, sizeof(this->errmsg), "Destination buffer size too small!");
		return -1;
	}
	this->ResetFlagAvailablePort();
	this->SetFlagAvailablePort();
	for (cnt = 0; cnt < 256; cnt++) {
		if (this->flagAvailablePort[cnt] == TRUE) {
			dest[cntAvailable] = cnt;
			cntAvailable++;
		}
	}
	dest[cntAvailable] = NULL;
	return cntAvailable;
}

BOOL SerialCommHandler::ConnectPort(tagDCBInfo DCBInfo, UCHAR indexPort)
{
	this->SerialPort[indexPort] = new SerialComm(DCBInfo.portName, DCBInfo.baudrate, DCBInfo.dataBits, DCBInfo.parity,
		DCBInfo.stopBits, (eFlowControl)DCBInfo.flowControl, DCBInfo.timeout);
	if (this->SerialPort[indexPort]->Connect(this->errmsg) == FALSE) return FALSE;
	this->flagConnectedPort[indexPort] = TRUE;
	return TRUE;
}

INT SerialCommHandler::GetConnectedPortNum(TCHAR *dest, INT cbDest)
{
	INT cnt;
	INT cntConnected = 0;
	if (cbDest < 256) {
		strcpy_s(this->errmsg, sizeof(this->errmsg), "Destination buffer size too small!");
		return -1;
	}
	for (cnt = 0; cnt < 256; cnt++) {
		if (this->flagConnectedPort[cnt] == TRUE) {
			dest[cntConnected] = cnt;
			cntConnected++;
		}
	}
	dest[cntConnected] = NULL;
	return cntConnected;
}

UCHAR SerialCommHandler::GetConnectedPortNum()
{
	TCHAR cnt;		
	for (cnt = 0; cnt < 256; cnt++) {
		if (this->flagConnectedPort[cnt] == TRUE) break;
	}	
	return cnt;
}

INT SerialCommHandler::ReflashConnectedPort()
{
	INT cnt;
	INT retCnt = 0;
	for (cnt = 0; cnt < 256; cnt++) {
		if (this->flagConnectedPort[cnt] == TRUE && this->SerialPort[cnt]->IsConnected() == FALSE) {
			this->SerialPort[cnt]->Close();
			this->flagConnectedPort[cnt] = FALSE;
			retCnt++;
		}
	}
	return retCnt;
}

BOOL SerialCommHandler::ClosePort(UCHAR indexPort)
{
	if (this->flagConnectedPort[indexPort] != TRUE) return FALSE;
	this->SerialPort[indexPort]->Close();
	delete this->SerialPort[indexPort];
	flagConnectedPort[indexPort] = FALSE;
	return TRUE;
}

INT SerialCommHandler::ClosePorts()
{
	INT cnt;
	INT cntClosed = 0;
	for (cnt = 0; cnt < 256; cnt++) {
		if (this->flagConnectedPort[cnt] == TRUE) {
			this->SerialPort[cnt]->Close();
			delete this->SerialPort[cnt];
			flagConnectedPort[cnt] = FALSE;
			cntClosed++;
		}
	}
	return cntClosed;
}

TCHAR* SerialCommHandler::GetErrMSGPtr()
{
	return this->errmsg;
}

SerialCommHandler::~SerialCommHandler()
{
	this->ClosePorts();
}
