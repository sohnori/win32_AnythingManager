#include "Procedure_CLT.h"
#include "SerialComm.h"
#include "SerialCommHandler.h"
#include "MyFunc.h"
#include "MyDevice.h"
#include "MyDeviceHandler.h"
#include "resource.h"
#include "DlgProc_AddDevice.h"
#include "DlgProc_ConnectTerminal.h"


#define DEVICETEXTXPOS	120
#define DEVICETEXTYPOS	280
#define DEVICETEXTYRATIO	55

extern HINSTANCE g_hInst;

extern MyDeviceHandler MyDev;
extern SerialCommHandler MySerial;

// 차일드 윈도우 핸들
extern HWND hCLT, hCRT, hCRB , hBtnTestA;

HWND hList, hEdit, hBtnAdd, hBtnAdd2, hBtnRemove, hBtnRemove2,
	hComboTerminal, hBtnConn, hBtnDisConn;

// 왼위쪽 차일드의 메시지 프로시저 처리 함수
INT OnCreateChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnTimerChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnSizeChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnCommandChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnPaintChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnUser1ChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 왼위쪽 차일드의 메시지 프로시저
LRESULT CALLBACK ChildLTProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{		
	switch (iMessage) {
	case WM_CREATE: return OnCreateChildLTProc(hWnd, wParam, lParam);
	case WM_SIZE: OnSizeChildLTProc(hWnd, wParam, lParam); return 0;
	case WM_COMMAND: OnCommandChildLTProc(hWnd, wParam, lParam); break;
	case WM_PAINT: OnPaintChildLTProc(hWnd, wParam, lParam); return 0;
	case WM_USER+1: OnUser1ChildLTProc(hWnd, wParam, lParam); return 0; // refresh serial list
	case WM_TIMER: OnTimerChildLTProc(hWnd, wParam, lParam); return 0;
	/*case WM_NCHITTEST: // 타이틀바 없이 작업영역 드래그 이동 가능
	nHit = DefWindowProc(hWnd, iMessage, wParam, lParam);
	if (nHit == HTCLIENT)
	nHit = HTCAPTION;
	return nHit;*/	
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT OnCreateChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	INT cnt;	
	tagDeviceInfo devInfo = { 0, };
	hList = CreateWindow(TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_HSCROLL,
		0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);	
	hBtnAdd = CreateWindow(TEXT("button"), TEXT("장치추가"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		180, 10, 100, 25, hWnd, (HMENU)2, g_hInst, NULL);
	hBtnAdd2 = CreateWindow(TEXT("button"), TEXT("터미널추가"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		180, 40, 100, 25, hWnd, (HMENU)3, g_hInst, NULL);
	hBtnRemove = CreateWindow(TEXT("button"), TEXT("장치제거"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		180, 70, 100, 25, hWnd, (HMENU)4, g_hInst, NULL);
	hBtnRemove2 = CreateWindow(TEXT("button"), TEXT("터미널제거"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		180, 100, 100, 25, hWnd, (HMENU)5, g_hInst, NULL);
	hComboTerminal = CreateWindow(TEXT("combobox"), NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
		180, 130, 100, 100, hWnd, (HMENU)6, g_hInst, NULL);
	hBtnConn = CreateWindow(TEXT("button"), TEXT("연결"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		180, 230, 40, 25, hWnd, (HMENU)7, g_hInst, NULL);
	hBtnDisConn = CreateWindow(TEXT("button"), TEXT("해제"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		230, 230, 40, 25, hWnd, (HMENU)8, g_hInst, NULL);	
	strcpy_s(devInfo.deviceName, TEXT("HostPC"));
	devInfo.deviceType = HostPC;	
	MyDev.NewMyDevice(devInfo, 0);
	strcpy_s(devInfo.deviceName, TEXT("중계기"));
	devInfo.deviceType = CoordinatorIoT;
	MyDev.NewMyDevice(devInfo, 1);
	strcpy_s(devInfo.deviceName, TEXT("인버터"));
	devInfo.deviceType = Inverter;
	MyDev.NewMyDevice(devInfo, 2);
	strcpy_s(devInfo.deviceName, TEXT("Etc장치"));
	devInfo.deviceType = etcDevice;
	MyDev.NewMyDevice(devInfo, 3);
	for (cnt = 0; cnt < MYDEVICE_COUNT_MAX; cnt++) {
		if (MyDev.GetFlagIsDevice(cnt) == TRUE) {
			MyDev.MyDev[cnt]->SetDeviceIndex(SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)MyDev.MyDev[cnt]->GetDeviceName()));
		}
	}
	SetTimer(hWnd, 2, 1000, NULL);
	return 0;	
}

INT OnTimerChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	static TCHAR sTime[128];
	HDC hDc;
	SYSTEMTIME st;
	switch (LOWORD(wParam)) {
	case 2: // 타이머 2번을 사용하여 PC 시간을 표시해준다.
		GetLocalTime(&st);
		wsprintf(sTime, TEXT("시각: %02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
		hDc = GetDC(hWnd);
		TextOut(hDc, 180, 160, sTime, lstrlen(sTime));
		ReleaseDC(hWnd, hDc);
		return 0;
	}
	return 0;
}

INT OnSizeChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	MoveWindow(hList, 10, 10, 160, HIWORD(lParam)/2, TRUE);
	return 0;
}

INT OnCommandChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	INT i;
	TCHAR str[MAX_PATH] = "";
	switch (LOWORD(wParam)) {
	case 0: // 파일목록 리스트박스		
		switch (HIWORD(wParam)) {
		case LBN_SELCHANGE:
			i = SendMessage(hList, LB_GETCURSEL, 0, 0);			
			SendMessage(hList, LB_GETTEXT, i, (LPARAM)str);
			i = MyDev.GetDeviceNum(str);
			if (i < 0 || i >= MYDEVICE_COUNT_MAX) break;
			i = MyDev.MyDev[i]->GetDeviceType();
			SendMessage(hCRT, WM_USER + 1, (WPARAM)i, 0);
			InvalidateRect(hCLT, NULL, TRUE);
			break;
		}
		break;
	case 2: // 장치 추가 버튼		
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD), hWnd, AddDeviceDlgProc);
		break;
	case 3: // 터미널 추가 버튼		
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_CONN), hWnd, ConnectDlgProc);		
		break;
	case 4: // 장치 제거 버튼
		// 리스트에서 장치제거 후 객체, 리스트 정보초기화		
		i = SendMessage(hList, LB_GETCURSEL, 0, 0);
		if (i == -1) {
			MessageBox(hWnd, TEXT("장치를 선택하십시오!!"), TEXT("메시지 박스"), MB_OK);
			break;
		}
		SendMessage(hList, LB_GETTEXT, i, (LPARAM)str);
		i = MyDev.GetDeviceNum(str);
		if (i < 1 || i >= MYDEVICE_COUNT_MAX) {
			MessageBox(hWnd, TEXT("유효한 장치 번호가 아닙니다."), TEXT("메시지 박스"), MB_OK);
			break;
		}
		MyDev.DelMyDevice(i);
		i = SendMessage(hList, LB_GETCURSEL, 0, 0);
		SendMessage(hList, LB_DELETESTRING, i, 0);		
		InvalidateRect(hCLT, NULL, TRUE);
		break;
	case 5: // 터미널 제거 버튼
		i = SendMessage(hComboTerminal, CB_GETCURSEL, 0, 0);
		if (i == -1) {
			MessageBox(hWnd, TEXT("터미널을 선택하십시오!!"), TEXT("메시지 박스"), MB_OK);
			break;
		}
		SendMessage(hComboTerminal, CB_GETLBTEXT, (WPARAM)i, (LPARAM)str);		
		if (MySerial.ClosePort(str) == TRUE) {
			i = SendMessage(hComboTerminal, CB_GETCURSEL, 0, 0);
			SendMessage(hComboTerminal, CB_DELETESTRING, (WPARAM)i, 0);
			SendMessage(hCRB, WM_USER + 1, (WPARAM)2, (LPARAM)i);
			MyDev.ResetMyDeviceTerminals(str);
		}
		InvalidateRect(hCLT, NULL, TRUE);
		break;
	case 6: // 터미널 목록 콤보박스
		break;
	case 7: // 연결 버튼 : 활성화되어 있는 장치와 터미널을 연결시킨다.
		TCHAR strDevName[MAX_PATH];
		i = SendMessage(hList, LB_GETCURSEL, 0, 0);
		if (i == -1) {
			MessageBox(hWnd, TEXT("장치를 선택하십시오!!"), TEXT("메시지 박스"), MB_OK);
			break;
		}
		if (i == 0) {
			MessageBox(hWnd, TEXT("HostPC는 터미널을 가질 수 없습니다!!"), TEXT("메시지 박스"), MB_OK);
			break;
		}
		i = SendMessage(hComboTerminal, CB_GETCURSEL, 0, 0);
		if (i == -1) {
			MessageBox(hWnd, TEXT("터미널을 선택하십시오!!"), TEXT("메시지 박스"), MB_OK);
			break;
		}
		SendMessage(hComboTerminal, CB_GETLBTEXT, (WPARAM)i, (LPARAM)str);
		i = SendMessage(hList, LB_GETCURSEL, 0, 0);
		SendMessage(hList, LB_GETTEXT, i, (LPARAM)strDevName);
		i = MyDev.GetDeviceNum(strDevName);
		if (i < 0 || i >= MYDEVICE_COUNT_MAX) {
			MessageBox(hWnd, TEXT("유효한 장치 번호가 아닙니다."), TEXT("메시지 박스"), MB_OK);
			break;
		}
		MyDev.MyDev[i]->SetDeviceTerminalName(str);
		if (strncmp("\\\\", str, 2) == 0)	MyDev.MyDev[i]->SetDeviceTerminalType(Serial);
		MyDev.MyDev[i]->SetDeviceTerminalFlag(TRUE);		
		InvalidateRect(hCLT, NULL, TRUE);
		break;
	case 8: // 해제 버튼 : 활성화되어 있는 장치와 터미널을 해제시킨다.
		i = SendMessage(hList, LB_GETCURSEL, 0, 0);
		if (i == -1) {
			MessageBox(hWnd, TEXT("파일을 선택하십시오!!"), TEXT("메시지 박스"), MB_OK);
			break;
		}
		SendMessage(hList, LB_GETTEXT, i, (LPARAM)str);
		i = MyDev.GetDeviceNum(str);
		if (i < 0 || i >= MYDEVICE_COUNT_MAX) {
			MessageBox(hWnd, TEXT("유효한 장치 번호가 아닙니다."), TEXT("메시지 박스"), MB_OK);
			break;
		}
		MyDev.MyDev[i]->ResetDeviceTerminal();
		InvalidateRect(hCLT, NULL, TRUE);
		break;
	}
	return 0;
}

INT OnPaintChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR str[MAX_PATH];
	TCHAR *ptrDevValue;
	INT index;	
	INT Height;
	dType devType;
	tType terminalType;
	RECT crt;
	tagDCBInfo dcb;

	hdc = BeginPaint(hWnd, &ps);
	// PC의 해상도를 표시한다.
	wsprintf(str, TEXT("%d, %d"), GetDeviceCaps(hdc, HORZRES), GetDeviceCaps(hdc, VERTRES));
	TextOut(hdc, 180, 180, str, lstrlen(str));
	index = SendMessage(hList, LB_GETCURSEL, 0, 0);
	if (index == -1) { EndPaint(hWnd, &ps); return 0; }
	SendMessage(hList, LB_GETTEXT, index, (LPARAM)str);
	index = MyDev.GetDeviceNum(str);
	GetClientRect(hWnd, &crt);
	Height = crt.bottom*DEVICETEXTYRATIO / 100;
	SetTextAlign(hdc, TA_RIGHT);
	//pDev = (Device*)SendMessage(hList, LB_GETITEMDATA, index, 0);
	//pDev->GetDeviceName(str);
		
	//wsprintf(str, TEXT("Device Name : %s"), pDev->GetDeviceName);	
	TextOut(hdc, DEVICETEXTXPOS, Height, TEXT("Device Name   :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +20, TEXT("Device Type   :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +40, TEXT("Terminal Type :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +60, TEXT("Terminal ID   :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +80, TEXT("Port Name     :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +100, TEXT("Baud Rate     :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +120, TEXT("Data Bits     :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +140, TEXT("Parity Bit    :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +160, TEXT("Stop Bit      :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height +180, TEXT("Flow Control  :"), 15);
	TextOut(hdc, DEVICETEXTXPOS, Height + 200, TEXT("Attribute     :"), 15);
	SetTextAlign(hdc, TA_NOUPDATECP);	
	ptrDevValue = MyDev.MyDev[index]->GetDeviceName();
	TextOut(hdc, DEVICETEXTXPOS+20, Height, ptrDevValue, lstrlen(ptrDevValue));	
	devType = MyDev.MyDev[index]->GetDeviceType();
	if (devType == HostPC) strcpy_s(str, TEXT("HostPC"));
	else if (devType == dNone) strcpy_s(str, TEXT("None"));
	else if (devType==CoordinatorIoT) strcpy_s(str, TEXT("CoordinatorIoT"));
	else if (devType == Inverter) strcpy_s(str, TEXT("Inverter"));
	else if (devType == etcDevice) strcpy_s(str, TEXT("etcDevice"));
	else  strcpy_s(str, TEXT(""));
	TextOut(hdc, DEVICETEXTXPOS + 20, Height + 20, str, lstrlen(str));
	terminalType = MyDev.MyDev[index]->GetDeviceTerminalType();
	if (terminalType == tNone) strcpy_s(str, TEXT("None"));
	else if (terminalType == Serial) strcpy_s(str, TEXT("Serial"));
	else if (terminalType == TCP_IP) strcpy_s(str, TEXT("TCP_IP"));	
	else  strcpy_s(str, TEXT(""));
	TextOut(hdc, DEVICETEXTXPOS + 20, Height + 40, str, lstrlen(str));
	ptrDevValue = MyDev.MyDev[index]->GetDeviceTerminalName();
	TextOut(hdc, DEVICETEXTXPOS + 20, Height + 80, ptrDevValue, lstrlen(ptrDevValue));

	if (MySerial.IsConnectedPort(ptrDevValue) == TRUE)
	{		
		index = MySerial.GetConnectedPortNum(ptrDevValue);
		dcb = MySerial.GetDCBInfo(index);
		_itoa_s(dcb.baudrate, str, 10);
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 100, str, lstrlen(str));
		_itoa_s(dcb.dataBits, str, 10);
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 120, str, lstrlen(str));
		if (dcb.parity == 0) strcpy_s(str, "None");
		else if (dcb.parity == 1) strcpy_s(str, "Odd");
		else if (dcb.parity == 2) strcpy_s(str, "Even");
		else strcpy_s(str, "");
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 140, str, lstrlen(str));
		if (dcb.stopBits == 0) strcpy_s(str, "1");
		else if (dcb.stopBits == 1) strcpy_s(str, "1.5");
		else if (dcb.stopBits == 2) strcpy_s(str, "2");
		else strcpy_s(str, "");
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 160, str, lstrlen(str));
		if (dcb.flowControl == 0) strcpy_s(str, "None");
		else if (dcb.flowControl == 1) strcpy_s(str, "Xon/Xoff");
		else if (dcb.flowControl == 2) strcpy_s(str, "RTS/CTS");
		else strcpy_s(str, "");
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 180, str, lstrlen(str));
	}
	else {
		strcpy_s(str, "");
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 100, str, lstrlen(str));
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 120, str, lstrlen(str));
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 140, str, lstrlen(str));
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 160, str, lstrlen(str));
		TextOut(hdc, DEVICETEXTXPOS + 20, Height + 180, str, lstrlen(str));
	}	
	str[0] = NULL; // attrubute part
	TextOut(hdc, DEVICETEXTXPOS + 20, Height + 200, str, lstrlen(str));

	EndPaint(hWnd, &ps);
	return 0;
}

INT OnUser1ChildLTProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{ // 장치 변화 감지로부터 메시지 받음
	INT index;
	TCHAR str[MAX_PATH];
	INT count = (INT)SendMessage(hComboTerminal, CB_GETCOUNT, 0, 0);
	switch (LOWORD(wParam)) {
	case 1:
		for (index = 0; index < count; index++) {
			if (count > SERIALPORT_COUNT_MAX) return FALSE;
			SendMessage(hComboTerminal, CB_GETLBTEXT, (WPARAM)index, (LPARAM)str);
			if (MySerial.IsConnectedPort(str) != TRUE) {
				SendMessage(hComboTerminal, CB_DELETESTRING, (WPARAM)index, 0);
				SendMessage(hCRB, WM_USER + 1, (WPARAM)2, (LPARAM)index); // 로그 에디트 창  탭 추가 또는 제거
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