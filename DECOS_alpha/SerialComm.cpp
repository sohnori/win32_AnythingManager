#include "SerialComm.h"
#include <assert.h>

static void OnFileAccessError(const TCHAR *errHeader, TCHAR *RetMes)
{
	int errCode = GetLastError();
	LPSTR errString = NULL;

	int size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, errCode, 0, (LPSTR)&errString, 0, 0);
	wsprintf(RetMes, "%s (%d): %s\n", errHeader, errCode, errString);
	LocalFree(errString);
}

SerialComm::SerialComm(const TCHAR *portName, INT baudRate, TCHAR dataBits, TCHAR parity, TCHAR stopBits,
	eFlowControl flowControl, DWORD timeout) : _comHandle(INVALID_HANDLE_VALUE), _baudRate(baudRate), _dataBits(dataBits),
	_stopBits(stopBits), _parity(parity), _flowControl(flowControl), _timeout(timeout)
{
	strncpy_s(_portName, portName, 32);
	_portName[32] = NULL;
}

SerialComm::~SerialComm()
{
	if (_comHandle != INVALID_HANDLE_VALUE) {
		Close();
	}
}

HWND SerialComm::GetPortWndHandle()
{
	return this->hPortWnd;
}

BOOL SerialComm::SetPortWndHandle(HWND hWnd)
{
	this->hPortWnd = hWnd;
	if (hPortWnd == INVALID_HANDLE_VALUE) return FALSE;
	else return TRUE;
}

INT SerialComm::GetPortAttribute()
{
	return this->attribute;
}

VOID SerialComm::SetPortAttribute(INT attribute)
{
	this->attribute = attribute;
}

TCHAR *SerialComm::GetPortName()
{
	return this->_portName;
}

BOOL SerialComm::GetPortName(TCHAR *dest)
{
	int cnt;
	for (cnt = 0; this->_portName[cnt] != NULL; cnt++) {
		if (cnt >= 32) return FALSE;
		dest[cnt] = this->_portName[cnt];
	}
	dest[cnt] = NULL;
	return TRUE;
}

BOOL SerialComm::GetBaudRate(INT *baudRate)
{
	*baudRate = this->_baudRate;
	return TRUE;
}

BOOL SerialComm::GetDataBits(TCHAR *dataBits)
{
	*dataBits = this->_dataBits;
	return TRUE;
}

BOOL SerialComm::GetParity(TCHAR *parity)
{
	*parity = this->_parity;
	return TRUE;
}

BOOL SerialComm::GetStopBits(TCHAR *stopBits)
{
	*stopBits = this->_stopBits;
	return TRUE;
}

BOOL SerialComm::GetFlowControl(TCHAR *flowControl)
{
	*flowControl = this->_flowControl;
	return TRUE;
}

BOOL SerialComm::GetTimeout(DWORD *timeout)
{
	*timeout = this->_timeout;
	return TRUE;
}

BOOL SerialComm::Open(TCHAR *RetMes)
{
	if (_comHandle != INVALID_HANDLE_VALUE) {
		strcpy_s(RetMes, 22, "Already opened port \n");
		Close();
	}

	_comHandle = CreateFile(_portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (_comHandle == INVALID_HANDLE_VALUE) {
		OnFileAccessError("SerialComm::CreateFile (): ", RetMes);
		return FALSE;
	}

	SetupComm(_comHandle, 8192, 8192);
	PurgeComm(_comHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	DCB dcb;

	if (!GetCommState(_comHandle, &dcb)) { // get dcb default value
		OnFileAccessError("SerialComm::GetCommState (): ", RetMes);
		Close();
		return FALSE;
	}

	// Set the new data
	dcb.BaudRate = _baudRate;	// Baudrate at which running
	dcb.ByteSize = _dataBits;	// Number of bits/byte, 5-8
	dcb.Parity = _parity;		// 0-4=None,Odd,Even,Mark,Space  
	dcb.StopBits = _stopBits;	// 0,1,2 = 1, 1.5, 2
	dcb.fParity = _parity ? TRUE : FALSE;	// Enable parity checking
	
											// 일단 기본 flow control 옵션을 설정한다.
											// 뒤에서 flow control 옵션에 따라 필요한 항목을 Enable 한다.
	dcb.fOutxCtsFlow = false;		// CTS(Clear To Send) handshaking on output       
	dcb.fOutxDsrFlow = false;		// DSR(Data Set Ready) handshaking on output       
	dcb.fDtrControl = DTR_CONTROL_ENABLE;	// DTR(Data Terminal Ready) Flow control                
	dcb.fRtsControl = RTS_CONTROL_ENABLE;	// RTS(Ready To Send) Flow control                
											// Xon, Xoff flow control 시 아래 두 항목을 Enable 한다.
	dcb.fOutX = false;		// Enable output X-ON/X-OFF        
	dcb.fInX = false;		// Enable input X-ON/X-OFF         

							//Setup the flow control 
	dcb.fDsrSensitivity = false;
	switch (_flowControl) {
	case FC_XONXOFF:	// Software Handshaking 
		dcb.fOutX = true;
		dcb.fInX = true;
		dcb.XonChar = 0x11;			// Tx and Rx X-ON character      
		dcb.XoffChar = 0x13;		// Tx and Rx X-OFF character   
		dcb.XoffLim = 100;			// Transmit X-ON threshold 
		dcb.XonLim = 100;			// Transmit X-OFF threshold    
		break;
	case FC_RTSCTS:
		dcb.fOutxCtsFlow = true;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		break;
	case FC_DTRDSR:
		dcb.fOutxDsrFlow = true;
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
		break;
	case FC_FULLHARDWARE: // Hardware Handshaking
		dcb.fOutxCtsFlow = true;
		dcb.fOutxDsrFlow = true;
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		break;
	}

	dcb.fBinary = true;			// Binary Mode (skip EOF check)    
	dcb.fNull = false;		// Enable Null stripping           
	dcb.fAbortOnError = false;		// Abort all reads and writes on Error 
	dcb.fDsrSensitivity = false;		// DSR Sensitivity              
	dcb.fTXContinueOnXoff = true;		// Continue TX when Xoff sent 
	dcb.fErrorChar = false;		// Enable Err Replacement          
	dcb.ErrorChar = 0;			// Error replacement char          
	dcb.EofChar = 0;			// End of Input character          
	dcb.EvtChar = 0;			// Received Event character

	if (!SetCommState(_comHandle, &dcb)) {
		OnFileAccessError("SerialComm::SetCommState (): ", RetMes);
		Close();
		return FALSE;
	}
	SetCommMask(_comHandle, EV_BREAK | EV_ERR | EV_RX80FULL | EV_RXCHAR | EV_TXEMPTY);
	SetTimeout(_timeout, _timeout, 1, RetMes);
	return TRUE;
}

BOOL SerialComm::Connect(TCHAR *RetMes)
{
	if (_comHandle == INVALID_HANDLE_VALUE) Open(RetMes);
	return (_comHandle != INVALID_HANDLE_VALUE);
}

BOOL SerialComm::Close()
{
	if (_comHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(_comHandle);
		_comHandle = INVALID_HANDLE_VALUE;
		return TRUE;
	}
	return FALSE;
}

DWORD SerialComm::Send(const TCHAR *buff, INT length, TCHAR *RetMes)
{
	if (this->_comHandle == INVALID_HANDLE_VALUE) return -1;

	DWORD writtenBytes = 0;
	if (WriteFile(this->_comHandle, buff, length, &writtenBytes, NULL)) {
		return writtenBytes;
	}
	OnFileAccessError("SerialComm::Send (): ", RetMes);	
	return -1;
}

DWORD SerialComm::SendAndClose(const TCHAR *buff, INT length, TCHAR *RetMes)
{
	if (_comHandle == INVALID_HANDLE_VALUE) return -1;

	DWORD writtenBytes = 0;
	if (WriteFile(_comHandle, buff, length, &writtenBytes, NULL)) {
		return writtenBytes;
	}
	OnFileAccessError("SerialComm::Send (): ", RetMes);
	Close();
	return -1;
}

DWORD SerialComm::Recv(TCHAR *buff, INT length, TCHAR *RetMes) {
	if (this->_comHandle == INVALID_HANDLE_VALUE) return -1;

	DWORD readBytes = 0;
	if (ReadFile(this->_comHandle, buff, length, &readBytes, NULL)) {
		return readBytes;
	}
	OnFileAccessError("SerialComm::Recv (): ", RetMes);	
	return -1;
}

DWORD SerialComm::RecvAndClose(TCHAR *buff, INT length, TCHAR *RetMes) {
	if (_comHandle == INVALID_HANDLE_VALUE) return -1;

	DWORD readBytes = 0;
	if (ReadFile(_comHandle, buff, length, &readBytes, NULL)) {
		return readBytes;
	}
	OnFileAccessError("SerialComm::Recv (): ", RetMes);
	Close();
	return -1;
}

BOOL SerialComm::GetMaskRXChar()
{
	DWORD maskFlag = (DWORD) EV_RXCHAR;
	BOOL retValue;
	retValue = GetCommMask(_comHandle, &maskFlag);	
	//return GetCommMask(_comHandle, (LPDWORD)EV_RXCHAR);
	return retValue;
}

DWORD SerialComm::ProcessEvent()
{
	DWORD dwEvtMask;
	/*OVERLAPPED o;
	o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	o.Internal = 0;
	o.InternalHigh = 0;
	o.Offset = 0;
	o.OffsetHigh = 0;
	assert(o.hEvent);*/
	WaitCommEvent(_comHandle, &dwEvtMask, NULL);//&o);
	return dwEvtMask;
}

void SerialComm::Purge(TCHAR *RetMes)
{
	if (_comHandle == INVALID_HANDLE_VALUE) return;

	DWORD comError = 0;
	COMSTAT comStat;
	if (ClearCommError(_comHandle, &comError, &comStat)) {
		if (comError) {
			wsprintf(RetMes, "ERROR: SerialComm::Purge(): %s\n", CommErrorString(comError));
		}
		else {
			OnFileAccessError("SerialComm::ClearCommError (): ", RetMes);
		}
	}

	PurgeComm(_comHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

}

BOOL SerialComm::SetTimeout(INT readTimeout, INT writeTimeout, INT readIntervalTimeout, TCHAR *RetMes)
{
	COMMTIMEOUTS commTimeout;
	GetCommTimeouts(_comHandle, &commTimeout);

	commTimeout.ReadIntervalTimeout = readIntervalTimeout;
	commTimeout.ReadTotalTimeoutMultiplier = 0;
	commTimeout.ReadTotalTimeoutConstant = readTimeout;
	commTimeout.WriteTotalTimeoutMultiplier = 0;
	commTimeout.WriteTotalTimeoutConstant = writeTimeout;

	if (SetCommTimeouts(_comHandle, &commTimeout)) {
		return TRUE;
	}
	OnFileAccessError("SerialComm::SetCommTimeouts (): ", RetMes);
	return FALSE;
}

INT SerialComm::CountRx(TCHAR *RetMes)
{
	if (_comHandle == INVALID_HANDLE_VALUE) return -1;

	DWORD comError = 0;
	COMSTAT comStat;
	if (ClearCommError(_comHandle, &comError, &comStat)) {
		if (comError) {
			wsprintf(RetMes, "ERROR: SerialComm::CountEx(): %s\n", CommErrorString(comError));
		}
		return comStat.cbInQue;
	}
	OnFileAccessError("SerialComm::ClearCommError (): ", RetMes);
	return -1;
}

const TCHAR *SerialComm::CommErrorString(DWORD comError)
{
	switch (comError) {
	case 0:           return "";
	case CE_BREAK:    return "The hardware detected a break condition.";
	case CE_FRAME:    return "The hardware detected a framing error.";
	case CE_OVERRUN:  return "A character-buffer overrun has occurred. The next character is lost.";
	case CE_RXOVER:   return "An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character.";
	case CE_RXPARITY: return "The hardware detected a parity error.";
	default:          return "Unknown error code.";
	}
}

