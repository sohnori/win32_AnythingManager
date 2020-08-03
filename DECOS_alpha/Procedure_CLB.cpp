#include "Procedure_CLB.h"

extern HINSTANCE g_hInst;
extern HWND hCLT, hCLB, hCRT, hCRB;

// 좌측하단 차일드 윈도우 메시지 처리 프로시저 함수
static INT OnPaintChildCLB(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 왼아랫쪽 차일드의 메시지 프로시저
LRESULT CALLBACK ChildLBProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{	
	switch (iMessage) {
	case WM_CREATE:
		/*CreateWindow(TEXT("static"), TEXT("Device Type:"), WS_CHILD | WS_VISIBLE,
			10, 10, 100, 20, hWnd, (HMENU)-1, g_hInst, NULL);*/
		return 0;
	case WM_SIZE:
	case WM_COMMAND:
		return 0;
	case WM_PAINT: OnPaintChildCLB(hWnd, wParam, lParam); return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT OnPaintChildCLB(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	SetTextAlign(hdc, TA_RIGHT);
	TextOut(hdc, 120, 10, TEXT("Device Name   :"), 15);
	TextOut(hdc, 120, 30, TEXT("Device Type   :"), 15);
	TextOut(hdc, 120, 50, TEXT("Terminal Type :"), 15);
	TextOut(hdc, 120, 70, TEXT("Terminal ID   :"), 15);
	TextOut(hdc, 120, 90, TEXT("Port Number   :"), 15);
	TextOut(hdc, 120, 110, TEXT("Service Type  :"), 15);
	TextOut(hdc, 120, 130, TEXT("Protocol      :"), 15);
	TextOut(hdc, 120, 150, TEXT("Version       :"), 15);
	TextOut(hdc, 120, 170, TEXT("Coding TX     :"), 15);
	TextOut(hdc, 120, 190, TEXT("Coding RX     :"), 15);
	//TextOut(hdc, 10, 210, TEXT("Attribute :"), 15);
	EndPaint(hWnd, &ps);
	return 1;
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