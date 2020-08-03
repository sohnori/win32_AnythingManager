#include "Procedure_CRT.h"
#include "resource.h"
#include "DlgProc_HostPC.h"
#include "DlgProc_Inverter.h"
#include "DlgProc_Etc.h"
#include "DlgProc_Coordi.h"
#include "MyDevice.h"
#include "MyDeviceHandler.h"

#define INITPOS_WIDTH_YMAX	2000
#define SCROLL_VALUE_LINEUPDOWN	50
#define SCROLL_VALUE_PAGEUPDOWN	200
#define INITPOS_WIDTH_XMAX	600
#define SCROLL_VALUE_LINELEFTRIGHT	50
#define SCROLL_VALUE_PAGELEFTRIGHT	200
#define SPACE_DISTANCE	100


extern HINSTANCE g_hInst;

// 차일드 윈도우 핸들
extern HWND hCLT, hCRT, hCRB;

extern HWND hHostPC, hCoordi, hInverter, hEtc;

extern MyDeviceHandler MyDev;

static INT xPos, xMax;
static INT yPos, yMax;

// 오른위쪽 차일드의 메시지 프로시저
INT OnHScrollChildRTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnVScrollChildRTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnUser1ChildRTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnPaintChildRTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);

//
VOID SetInvPhaseCombobox(HWND hDlg, INT indexType, INT indexPhase);

// 오른위쪽 차일드의 프로시저
LRESULT CALLBACK ChildRTProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{			
	switch (iMessage) {
	case WM_CREATE:
		xPos = 0;
		yPos = 0;
		xMax = INITPOS_WIDTH_XMAX;
		yMax = INITPOS_WIDTH_YMAX;
		hHostPC = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_HOSTPC), hWnd, HostPCDlgProc);
		SetWindowPos(hHostPC, NULL, 20, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		ShowWindow(hHostPC, SW_SHOW);		

		hCoordi = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_CD_IOT), hWnd, CoordiDlgProc);
		SetWindowPos(hCoordi, NULL, 20, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		ShowWindow(hCoordi, SW_HIDE);

		hInverter = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_INVERTER), hWnd, InverterDlgProc);
		SetWindowPos(hInverter, NULL, 20, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		ShowWindow(hInverter, SW_HIDE);

		hEtc = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_ETC), hWnd, EtcDlgProc);
		SetWindowPos(hEtc, NULL, 20, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		ShowWindow(hEtc, SW_HIDE);		

		SetScrollRange(hWnd, SB_HORZ, 0, xMax, TRUE);
		SetScrollPos(hWnd, SB_HORZ, 0, TRUE);
		SetScrollRange(hWnd, SB_VERT, 0, yMax, TRUE);
		SetScrollPos(hWnd, SB_VERT, 0, TRUE);		
		return 0;	
	case WM_HSCROLL: OnHScrollChildRTProc(hWnd, wParam, lParam);	return 0;
	case WM_VSCROLL: OnVScrollChildRTProc(hWnd, wParam, lParam);	return 0;	
	case WM_SIZE:
		/*RECT crt;
		GetClientRect(hTest1, &crt);
		yMax = crt.bottom - crt.top;
		SetScrollRange(hWnd, SB_VERT, 0, yMax, TRUE);
		SetScrollPos(hWnd, SB_VERT, 0, TRUE);*/
		return 0;
	//case WM_COMMAND:	
	case WM_PAINT: OnPaintChildRTProc(hWnd, wParam, lParam); return 0;
	//case WM_USER:	
	case WM_USER + 1: OnUser1ChildRTProc(hWnd, wParam, lParam); return 0;	
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT OnHScrollChildRTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	INT xInc = 0;
	switch (LOWORD(wParam)) {
	case SB_LINELEFT:
		xInc = -SCROLL_VALUE_LINELEFTRIGHT;
		break;
	case SB_LINERIGHT:
		xInc = SCROLL_VALUE_LINELEFTRIGHT;
		break;
	case SB_PAGELEFT:
		xInc = -SCROLL_VALUE_PAGELEFTRIGHT;
		break;
	case SB_PAGERIGHT:
		xInc = SCROLL_VALUE_PAGELEFTRIGHT;
		break;
	case SB_THUMBTRACK:
		xInc = HIWORD(wParam) - xPos;
		break;
	}
	//xInc = max(-xPos, min(xInc, xMax - xPos));
	// 새로운 위치는 최소한 0 이상
	if (xPos + xInc < 0) xInc = -xPos;
	// 새로운 위치는 최대한 xMax 이하
	if (xPos + xInc > xMax) xInc = xMax - xPos;
	// 새로운 위치 계산
	xPos = xPos + xInc;
	// 스크롤시키고 썸 위치를 다시 계산한다.	
	ScrollWindow(hWnd, -xInc, 0, NULL, NULL);
	SetScrollPos(hWnd, SB_HORZ, xPos, TRUE);
	return 0;
}

INT OnVScrollChildRTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	INT yInc = 0;
	switch (LOWORD(wParam)) {
	case SB_LINEUP:
		yInc = -SCROLL_VALUE_LINEUPDOWN;
		break;
	case SB_LINEDOWN:
		yInc = SCROLL_VALUE_LINEUPDOWN;
		break;
	case SB_PAGEUP:
		yInc = -SCROLL_VALUE_PAGEUPDOWN;
		break;
	case SB_PAGEDOWN:
		yInc = SCROLL_VALUE_PAGEUPDOWN;
		break;
	case SB_THUMBTRACK:
		yInc = HIWORD(wParam) - yPos;
		break;
	}
	// yInc = max(-yPos, min(yInc, yMax - yPos));
	// 새로운 위치는 최소한 0 이상
	if (yPos + yInc < 0) yInc = -yPos;
	// 새로운 위치는 최대한 yMax 이하
	if (yPos + yInc > yMax) yInc = yMax - yPos;
	// 새로운 위치 계산
	yPos = yPos + yInc;
	// 스크롤시키고 썸 위치를 다시 계산한다.
	ScrollWindow(hWnd, 0, -yInc, NULL, NULL);
	SetScrollPos(hWnd, SB_VERT, yPos, TRUE);
	return 0;
}

INT OnUser1ChildRTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//TCHAR str[MAX_PATH];
	dType index = (dType)wParam;
		
	if(index==HostPC) {	
		ShowWindow(hHostPC, SW_SHOW);
		ShowWindow(hCoordi, SW_HIDE);
		ShowWindow(hInverter, SW_HIDE);
		ShowWindow(hEtc, SW_HIDE);
	}
	else if (index == CoordinatorIoT) {
		ShowWindow(hHostPC, SW_HIDE);
		ShowWindow(hCoordi, SW_SHOW);
		ShowWindow(hInverter, SW_HIDE);
		ShowWindow(hEtc, SW_HIDE);
	}
	else if (index == Inverter) {
		ShowWindow(hHostPC, SW_HIDE);
		ShowWindow(hCoordi, SW_HIDE);
		ShowWindow(hInverter, SW_SHOW);
		ShowWindow(hEtc, SW_HIDE);
	}
	else if (index == etcDevice) {
		ShowWindow(hHostPC, SW_HIDE);
		ShowWindow(hCoordi, SW_HIDE);
		ShowWindow(hInverter, SW_HIDE);
		ShowWindow(hEtc, SW_SHOW);
	}
	return 0;
}

INT OnPaintChildRTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	INT i;
	HDC hdc;
	PAINTSTRUCT ps;
	//HBRUSH MyBrush, OldBrush;
	TCHAR str[10];
	hdc = BeginPaint(hWnd, &ps);
	//SetBkMode(hdc, TRANSPARENT);
	for (i = 0; i < SPACE_DISTANCE; i++) {
		wsprintf(str, TEXT("%d"), i);
		TextOut(hdc, 0, i* SPACE_DISTANCE - yPos, str, lstrlen(str));
	}
	//TextOut(hdc, 10, 50, Mes, lstrlen(Mes));
	//MyBrush = (HBRUSH)GetStockObject(RGB(0, 200, 0));
	//OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);	
	EndPaint(hWnd, &ps);
	return 0;
}
