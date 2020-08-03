// 잘 사용되지 않는 헤더 파일은 컴파일에서 제외할 수 있는 조건부 컴파일 매크로
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "InverterComm.h"
#include "ConvertData.h"
#include "DlgProc_ConnectPort.h"
#include "SerialCommHandler.h"
#include "Dlg_IDScan.h"
#include "DlgProc_InvComm.h"
#include "resource.h"


// 
// 도움말 팁 - 검색하고자하는 대상을 선택한 후에 F1키를 누르면 자동으로 MSDN웹 도움말에서 검색한다.
//

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("인버터 통신 프로그램");
TCHAR sTime[50];
RECT rtTime{10, 10, 400, 100};
HFONT hFont;
static HBITMAP MyBitmap;

// 메인 윈도우 프로시저 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 기타 필요한 함수
void DrawBitmap(HWND hWnd, HDC hdc, int x, int y, HBITMAP hBit);

// 차일드 버튼 핸들
HWND hBtnConn;

// 차일드 스태틱 핸들
HWND hStatic1, hStatic2, hStatic3, hStatic4, hStatic5, hStatic6;

// 차일드 모델리스 핸들
HWND hMDlgIDScan, hMDlgInvComm;

// 시리얼통신포트 핸들러
SerialCommHandler Serial;
UCHAR portNum = 0;
TCHAR SerialBuff[512];
DWORD dwBytesRead = 0;
TCHAR flagRX = 0;

// 스레드 핸들
HANDLE hSerialThread;

// 스레드 함수
DWORD WINAPI SerialThreadFunc(LPVOID prc);

extern tagDCBInfoStr DCBInfoStr;

static HBRUSH hBk1, hBk2, hBk3;
static HBRUSH hBrushNow;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{	
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	//WndClass.hCursor = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_CURSOR1));
	//WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	//WndClass.lpszMenuName = NULL;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	HACCEL hAccel;
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	hWndMain = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_TABSTOP | WS_MINIMIZEBOX,
		0, 0, 680, 800, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWndMain, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		if (!TranslateAccelerator(hWndMain, hAccel, &Message) || 
			!IsWindow(hMDlgIDScan) || !IsDialogMessage(hMDlgIDScan, &Message) ||
			!IsWindow(hMDlgInvComm) || !IsDialogMessage(hMDlgInvComm, &Message)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}		
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{	
	switch (iMessage) {
	case WM_CREATE:	OnCreate(hWnd, wParam, lParam); return 0;
	case WM_SIZE: OnSize(hWnd, wParam, lParam); return 0;
	case WM_COMMAND: OnCommand(hWnd, wParam, lParam); return 0;
	case WM_TIMER: OnTimer(hWnd, wParam, lParam); return 0;
	case WM_PAINT: OnPaint(hWnd, wParam, lParam); return 0;
	case WM_DESTROY:
		DeleteObject(hBk1);
		DeleteObject(hBk2);
		DeleteObject(hBk3);
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{		
	DWORD ThreadID;
	hBtnConn = CreateWindow(TEXT("button"), TEXT("OpenPort"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 15, 80, 25, hWnd, (HMENU)1, g_hInst, NULL);
	hStatic1 = CreateWindow(TEXT("static"), TEXT("Port: "), WS_CHILD | WS_VISIBLE,
		100, 10, 120, 15, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic2 = CreateWindow(TEXT("static"), TEXT("Baudrate: "), WS_CHILD | WS_VISIBLE,
		230, 10, 150, 15, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic3 = CreateWindow(TEXT("static"), TEXT("DataBits: "), WS_CHILD | WS_VISIBLE,
		390, 10, 100, 15, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic4 = CreateWindow(TEXT("static"), TEXT("Parity: "), WS_CHILD | WS_VISIBLE,
		100, 30, 120, 15, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic5 = CreateWindow(TEXT("static"), TEXT("Stop: "), WS_CHILD | WS_VISIBLE,
		230, 30, 150, 15, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic6 = CreateWindow(TEXT("static"), TEXT("Flow: "), WS_CHILD | WS_VISIBLE,
		390, 30, 100, 15, hWnd, (HMENU)-1, g_hInst, NULL);
	
	hMDlgIDScan = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_IDSCAN), hWnd, IDScanDlgProc);
	hMDlgInvComm = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_INVCOMM), hWnd, InvCommDlgProc);
	ShowWindow(hMDlgIDScan, SW_SHOW);
	ShowWindow(hMDlgInvComm, SW_SHOW);
	MoveWindow(hMDlgIDScan, 0, 60, 680, 160, TRUE);
	MoveWindow(hMDlgInvComm, 0, 230, 680, 480, TRUE);
	MyBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	SetTimer(hWnd, 1, 1000, NULL);
	SendMessage(hWnd, WM_TIMER, 1, 0);	
	//hBk1 = CreateSolidBrush(RGB(255, 0, 0)); // 레드
	//hBk2 = CreateSolidBrush(RGB(0, 255, 0)); // 그린
	//hBk3 = CreateSolidBrush(RGB(0, 0, 255)); // 파랑
	hBk1 = CreateSolidBrush(RGB(245, 245, 245)); // 옅은 회색
	hBk2 = CreateSolidBrush(RGB(225, 225, 225)); // 회색
	hBk3 = CreateSolidBrush(RGB(200, 200, 200)); // 진한 회색
	hBrushNow = (HBRUSH)(COLOR_WINDOW + 1);	

	hSerialThread = CreateThread(NULL, 0, SerialThreadFunc, &portNum, 0, &ThreadID);
	CloseHandle(hSerialThread);
	//SuspendThread(hSerialThread);
	//ResumeThread(hSerialThread);
	return 0;
}

INT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rt;	
	GetClientRect(hWnd, &rt);
	rtTime.left = rt.right - 140;
	rtTime.right = rt.right;
	rtTime.top = rt.bottom - 20;
	rtTime.bottom = rt.bottom;
	return 0;
}

INT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	TCHAR strBuff[MAX_PATH];
	switch (LOWORD(wParam)) {
	case 1:
		Serial.ClosePorts();
		portNum = 0;		
		if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_SERIALPORT), hWnd, ConnectPortDlgProc) == IDOK) {
			SetWindowText(hBtnConn, TEXT("Connected"));
			wsprintf(strBuff, "Port: %s", DCBInfoStr.portName);
			SetWindowText(hStatic1, strBuff);
			wsprintf(strBuff, "Baudrate: %s", DCBInfoStr.baudrate);
			SetWindowText(hStatic2, strBuff);
			wsprintf(strBuff, "DataBits: %s", DCBInfoStr.dataBits);
			SetWindowText(hStatic3, strBuff);
			wsprintf(strBuff, "Parity: %s", DCBInfoStr.parity);
			SetWindowText(hStatic4, strBuff);
			wsprintf(strBuff, "Stop: %s", DCBInfoStr.stopBits);
			SetWindowText(hStatic5, strBuff);
			wsprintf(strBuff, "Flow: %s", DCBInfoStr.flowControl);
			SetWindowText(hStatic6, strBuff);
			portNum = Serial.GetConnectedPortNum();			
		}
		else {
			SetWindowText(hBtnConn, TEXT("OpenPort"));
			SetWindowText(hStatic1, TEXT("Port: "));
			SetWindowText(hStatic2, TEXT("Baudrate: "));
			SetWindowText(hStatic3, TEXT("DataBits: "));
			SetWindowText(hStatic4, TEXT("Parity: "));
			SetWindowText(hStatic5, TEXT("Stop: "));
			SetWindowText(hStatic6, TEXT("Flow: "));
			Serial.ClosePorts();
			portNum = 0;
		}
		break;
	case ID_FILE_NEW:
		break;
	case ID_FILE_SAVE:
		break;
	case ID_FILE_LOAD:
		break;
	case ID_FILE_EXIT:
		DestroyWindow(hWnd);
		break;
	case ID_VIEW_BACKGROUND:
		if (hBrushNow == hBk1) hBrushNow = hBk2;
		else if (hBrushNow == hBk2) hBrushNow = hBk3;
		else if (hBrushNow == hBk3) hBrushNow = (HBRUSH)(COLOR_WINDOW + 1);
		else if (hBrushNow == (HBRUSH)(COLOR_WINDOW + 1)) hBrushNow = hBk1;
		// 동록한 윈도우 클래스의 속성을 변경하는 함수
		SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrushNow);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case ID_HELP_INFORMATION:
		break;
	}
	return 0;
}

INT OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SYSTEMTIME st;	
	GetLocalTime(&st);	
	wsprintf(sTime, TEXT("%d-%02d-%02d %02d:%02d:%02d"), 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	InvalidateRect(hWnd, &rtTime, TRUE);
	return 1;
}

INT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;	
	hdc = BeginPaint(hWnd, &ps);
	DrawBitmap(hWnd, hdc, 10, 10, MyBitmap);
	TextOut(hdc, rtTime.left, rtTime.top, sTime, lstrlen(sTime));
	EndPaint(hWnd, &ps);
	return 0;
} // 순수 그리는 작업 코드만 적용하는게 시스템 운영상 효율적이다.

void DrawBitmap(HWND hWnd, HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;
	RECT rt;

	GetClientRect(hWnd, &rt);
	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, MyBitmap);
	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;
	//BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY); // if want to set start position
	BitBlt(hdc, rt.right - bx - 5, rt.top + 5, bx, by, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}

DWORD WINAPI SerialThreadFunc(LPVOID prc)
{
	UCHAR portNum;
	TCHAR msg[MAX_PATH];	
	for (;;) {
		Sleep(500);
		portNum = *(UCHAR*)prc;
		if (portNum != 0) {
			dwBytesRead = Serial.SerialPort[portNum]->Receive(SerialBuff, sizeof(SerialBuff), msg);
			if (dwBytesRead > 0) {
				SerialBuff[dwBytesRead] = NULL;
				SendMessage(hMDlgInvComm, WM_USER, dwBytesRead, (LPARAM)SerialBuff);
				flagRX++;
			}
		}
	}
	return 0;
}

/*case WM_CREATE:return OnCreate(hWnd, wParam, lParam);
case WM_DESTROY:OnDestroy(hWnd, wParam, lParam); return 0;
case WM_COMMAND:OnCommand(hWnd, wParam, lParam); break;
case WM_TIMER:OnTimer(hWnd, wParam, lParam); return 0;
case WM_CLOSE:if (OnClose(hWnd, wParam, lParam)) return 0; break;
case WM_QUERYENDSESSION:if (OnQueryEndSession(hWnd, wParam, lParam)) return 0; break;
case WM_INITMENU:OnInitMenu(hWnd, wParam, lParam); return 0;
case WM_DROPFILES:OnDropFiles(hWnd, wParam, lParam); return 0;
case WM_FONTCHANGE:OnFontChange(hWnd, wParam, lParam); return 0;
case WM_ACTIVATEAPP:OnActivateApp(hWnd, wParam, lParam); return 0;
case WM_USER + 1:OnUser1(hWnd, wParam, lParam); return 0;
case WM_USER + 2:OnUser2(hWnd, wParam, lParam); return 0;
case WM_SIZE:if (OnSize(hWnd, wParam, lParam)) return 0; break;
case WM_SETCURSOR:if (OnSetCursor(hWnd, wParam, lParam)) return 0; break;
case WM_LBUTTONDOWN:OnLButtonDown(hWnd, wParam, lParam); return 0;
case WM_MOUSEMOVE:OnMouseMove(hWnd, wParam, lParam); return 0;
case WM_LBUTTONUP:OnLButtonUp(hWnd, wParam, lParam); return 0;
case WM_NOTIFY:OnNotify(hWnd, wParam, lParam); return 0;
case WM_SYSCOLORCHANGE:OnSysColorChange(hWnd, wParam, lParam); return 0;
case WM_USER + 3:OnUser3(hWnd, wParam, lParam); return 0;
case WM_USER + 4:OnUser4(hWnd, wParam, lParam); return 0;*/