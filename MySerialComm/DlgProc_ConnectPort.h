#pragma once
#include <Windows.h>

#define IDSERIAL	3
#define IDTCPCNT	4
#define IDTCPSERVER	6
#define IDUDPSERVER	7
#define IDUDPCNT	8

struct tagDCBInfoStr {
	TCHAR portName[32 + 1];
	TCHAR baudrate[10];
	TCHAR dataBits[2];
	TCHAR stopBits[4];
	TCHAR parity[10];
	TCHAR flowControl[10];
	TCHAR timeout[10];
};

struct tagNetworkInfo {
	TCHAR localIPstr[20];
	TCHAR remoteIPstr[20];
	TCHAR portNumStr[10];
	DWORD localIPlong;
	DWORD remoteIPlong;
	WORD portNumShort;
};

BOOL CALLBACK ConnectPortDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

tagDCBInfoStr GetDCBInfoStr();

tagNetworkInfo GetNetworkInfo();