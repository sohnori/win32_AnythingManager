#pragma once

#define WND_INITSIZE_W	800
#define WND_INITSIZE_H	440

#define CONTROL_INIT_X	10

#define STT_GAP	10

#define BTN1_INIT_POSX	10
#define STT1_INIT_POSX	100
#define STT2_INIT_POSX	STT1_INIT_POSX+STT1_SIZEW+STT_GAP
#define STT3_INIT_POSX	STT2_INIT_POSX+STT2_SIZEW+STT_GAP
#define STT4_INIT_POSX	STT3_INIT_POSX+STT3_SIZEW+STT_GAP
#define STT5_INIT_POSX	STT1_INIT_POSX
#define STT6_INIT_POSX	STT5_INIT_POSX+STT5_SIZEW+STT_GAP
#define STT7_INIT_POSX	STT6_INIT_POSX+STT6_SIZEW+STT_GAP
#define STT8_INIT_POSX	STT7_INIT_POSX+STT7_SIZEW+STT_GAP

#define BTN1_INIT_POSY	10
#define STT1_INIT_POSY	10
#define STT2_INIT_POSY	10
#define STT3_INIT_POSY	10
#define STT4_INIT_POSY	10
#define STT5_INIT_POSY	30
#define STT6_INIT_POSY	30
#define STT7_INIT_POSY	30
#define STT8_INIT_POSY	30

#define BTN1_SIZEW		80
#define STT1_SIZEW		160
#define STT2_SIZEW		160
#define STT3_SIZEW		160
#define STT4_SIZEW		160
#define STT5_SIZEW		160
#define STT6_SIZEW		160
#define STT7_SIZEW		160
#define STT8_SIZEW		160

#define BTN1_SIZEH		25
#define STT1_SIZEH		15
#define STT2_SIZEH		15
#define STT3_SIZEH		15
#define STT4_SIZEH		15
#define STT5_SIZEH		15
#define STT6_SIZEH		15
#define STT7_SIZEH		15
#define STT8_SIZEH		15

#define CONNECT_PORT		1
#define SAVE_OUTPUT			2
#define CLEAR_OUTPUT		3
#define SEND_CMD			4

#define ID_R101		101
#define ID_R102		102
#define ID_R103		103
#define ID_R104		104

#define ID_ADD_CKSUM		110
#define ID_BTN_MAKECKSUM	111
#define ID_COMBO_CRC		112
#define ID_EDIT_CKSUM		113
#define ID_RCRC8			116
#define ID_RCRC16			117
#define ID_RCRC32			118
#define ID_RUSER			119

#define WM_USER_THREAD_OUTPUT	WM_USER+1

#define MY_THREAD_TX				1
#define MY_THREAD_RX				2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hBtnTerminal, hBtnSave, hBtnClear, hBtnSend;
HWND hGBtn1;
HWND hRBtnHex1, hRBtnAscii1, hRBtnHex2, hRBtnAscii2;
HWND hCBtnSend, hCBtnReceive;
HWND hCBtnCksum , hBtnCRCmake;
HWND hRBtnCRC8, hRBtnCRC16, hRBtnCRC32, hRBtnUser;
HWND hComboCRC;
HWND hEditCksum;
HWND hStatic1, hStatic2, hStatic3, hStatic4, hStatic5, hStatic6, hStatic7, hStatic8, hStatic9;
HWND hEditOutput, hEditSend;
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("MySerialComm");

const TCHAR* strPort = TEXT("Port: ");
const TCHAR* strBR = TEXT("Baudrate: ");
const TCHAR* strDB = TEXT("DataBits: ");
const TCHAR* strParity = TEXT("Parity: ");
const TCHAR* strStop = TEXT("Stop: ");
const TCHAR* strFlow = TEXT("Flow: ");
const TCHAR* strTimeOut = TEXT("TimeOut: ");

const TCHAR* strLocalIP = TEXT("Local IP(binding addr): ");
const TCHAR* strRemoteIP = TEXT("Remote IP: ");
const TCHAR* strNport = TEXT("Port: ");
const TCHAR* strNull = TEXT("");

TCHAR sTime[50];
RECT rtTime{ 10, 10, 400, 100 };

static HWND hWndMain;
static unsigned char portNum = 0;
static TCHAR rxBuff[2048] = { 0 };
static INT dwBytesRead = 0;
static TCHAR txBuff[1024] = { 0 };
static INT dwBytesWrite = 0;
static BOOL flagEditSendASCII = 0;
static INT flagConnectedType = 0;

// 메인 윈도우 프로시저 처리 함수
INT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnDeviceChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnUserThreadOutput(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 함수 정의

// 윈도우 프로시저 포인터
WNDPROC OldWndProc;

// 스레드 함수
DWORD WINAPI ThreadSerialFunc(LPVOID value);
DWORD WINAPI ThreadNetworkFunc(LPVOID value);

// 서브 클래싱 함수 정의
LRESULT CALLBACK SendEditProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{ // 우하단 차일드 윈도우의 보내기 명령창의 키 입력 처리
	switch (iMessage) {
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			//SetFocus(hWnd);
			SendMessage(hWndMain, WM_COMMAND, SEND_CMD, 0);
		}
		if (wParam == VK_UP) {
			SendMessage(hEditSend, EM_UNDO, 0, 0);
		}
		if (wParam == VK_DOWN) {
			SendMessage(hEditSend, EM_CANUNDO, 0, 0);
		}
		break;
	}
	return CallWindowProc(OldWndProc, hWnd, iMessage, wParam, lParam);
}