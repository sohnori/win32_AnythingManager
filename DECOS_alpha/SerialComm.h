#pragma once
#include <Windows.h>

namespace SERIALCOMM_CONST {
	enum eFlowControl {
		FC_NONE = 0,
		FC_XONXOFF,
		FC_RTSCTS,
		FC_DTRDSR,
		FC_FULLHARDWARE,
	};
}

using namespace SERIALCOMM_CONST;

class SerialComm {
private:
	HANDLE _comHandle;
	HWND hPortWnd;
	TCHAR _portName[32 + 1];
	INT _baudRate;
	TCHAR _dataBits;
	TCHAR _stopBits;
	TCHAR _parity;
	TCHAR _flowControl;
	DWORD _timeout;
	INT attribute;

public:
	SerialComm(const TCHAR *portName, INT baudRate, TCHAR dataBits = 8, TCHAR parity = NOPARITY, TCHAR stopBits = ONESTOPBIT,
		eFlowControl flowControl = FC_NONE, DWORD timeout = 100);
	~SerialComm();
	HWND GetPortWndHandle();
	BOOL SetPortWndHandle(HWND hWnd);	
	INT GetPortAttribute();
	VOID SetPortAttribute(INT attribute);
	TCHAR *GetPortName();
	BOOL GetPortName(TCHAR *dest);
	BOOL GetBaudRate(INT *baudRate);
	BOOL GetDataBits(TCHAR *dataBits);
	BOOL GetParity(TCHAR *parity);
	BOOL GetStopBits(TCHAR *stopBits);
	BOOL GetFlowControl(TCHAR *flowControl);
	BOOL GetTimeout(DWORD *timeout);
	BOOL Open(TCHAR *Mes);
	BOOL Connect(TCHAR *Mes);
	BOOL Close();
	DWORD Send(const TCHAR *buff, INT length, TCHAR *RetMes);
	DWORD SendAndClose(const TCHAR *buff, INT length, TCHAR *RetMes);
	DWORD Recv(TCHAR *buff, INT length, TCHAR *RetMes);
	DWORD RecvAndClose(TCHAR *buff, INT length, TCHAR *RetMes);
	BOOL GetMaskRXChar();
	DWORD ProcessEvent();
	void Purge(TCHAR *RetMes);

	BOOL IsConnected() { return _comHandle != INVALID_HANDLE_VALUE; }
	BOOL SetTimeout(INT readTimeout, INT writeTimeout, INT readIntervalTimeout, TCHAR *RetMes);

	void EscapeCommFunc(DWORD func) { if (_comHandle != INVALID_HANDLE_VALUE) EscapeCommFunction(_comHandle, func); }
	void GetCommModemStat(DWORD &stat) { if (_comHandle != INVALID_HANDLE_VALUE) GetCommModemStatus(_comHandle, &stat); else stat = 0; }
	INT CountRx(TCHAR *RetMes);
	const TCHAR *CommErrorString(DWORD comError);	
};
