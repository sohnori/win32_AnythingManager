#include "Procedure_CRB.h"
#include <stdio.h>
#include <CommCtrl.h>
#include "SerialComm.h"
#include "SerialCommHandler.h"
#include "MyFunc.h"
#include "MyDevice.h"
#include "MyDeviceHandler.h"
#include "CoordinatorReal.h"
#include "DlgProc_Inverter.h"

extern HINSTANCE g_hInst;

extern MyDeviceHandler MyDev;
extern SerialCommHandler MySerial;
extern HWND hCmdEdit;
extern HWND hCRB, hCoordi;
extern HWND hComboTerminal;

static HWND hLogEdit[SERIALPORT_COUNT_MAX];
static HWND  hTab;
static HWND hGroup1, hGroup2, hR1, hR2, hCK1, hCK2, hCK4, hBtn1, hBtn2, hR3, hR4, hBtn3;
static WORD posLogW;
static WORD posLogH;
static TCHAR connectedPort[SERIALPORT_COUNT_MAX];

TCHAR rxBuff[1024] = { 0 };
DWORD dwBytesRead = 0;
TCHAR txBuff[1024] = { 0 };
INT dwBytesWrite = 0;

CoordinatorReal *CoordiR;

// 스레드 함수
DWORD WINAPI ThreadSerialFunc(LPVOID value);

// 오른아랫쪽 차일드의 메시지 프로시저 처리 함수
INT OnCreateChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnCommandChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnNotifyChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnSizeChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnUser1ChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnUser2ChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnPaintChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 오른아랫쪽 차일드의 메시지 프로시저
LRESULT CALLBACK ChildRBProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_CREATE: OnCreateChildRBProc(hWnd, wParam, lParam); return 0;	
	case WM_COMMAND:OnCommandChildRBProc(hWnd, wParam, lParam); break;
	//case WM_KEYDOWN:
	case WM_RBUTTONDOWN:
		if (wParam == VK_RETURN) {
			//SetFocus(hWnd);
			SendMessage(hWnd, WM_COMMAND, 3, 0);
		}
		break;
	case WM_NOTIFY:	OnNotifyChildRBProc(hWnd, wParam, lParam); return 0;
	case WM_SIZE: OnSizeChildRBProc(hWnd, wParam, lParam); return 0;
	case WM_USER+1: OnUser1ChildRBProc(hWnd, wParam, lParam); return 0;
	case WM_USER+2:	OnUser2ChildRBProc(hWnd, wParam, lParam); return 0;
	case WM_PAINT: OnPaintChildRBProc(hWnd, wParam, lParam); return 0;	
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT OnCreateChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	TCITEM tie;
	hLogEdit[0] = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);	
	ShowWindow(hLogEdit[0], SW_SHOW);
	SendMessage(hLogEdit[0], EM_LIMITTEXT, (WPARAM)102400, 0); // 입력 문자수 제한 기본 32k
	//SetWindowText(hLogEdit0, TEXT("에디트 윈도우0입니다."));
	//InitCommonControls();
	hTab = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_FORCELABELLEFT,
		0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
	hGroup1 = CreateWindow(TEXT("button"), TEXT("표시창 :"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hR1 = CreateWindow(TEXT("button"), TEXT("HEX"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hR2 = CreateWindow(TEXT("button"), TEXT("ASCII"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hCK1 = CreateWindow(TEXT("button"), TEXT("Send 표시"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hCK2 = CreateWindow(TEXT("button"), TEXT("Receive 표시"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);	
	hBtn1 = CreateWindow(TEXT("button"), TEXT("저장"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)1, g_hInst, NULL);
	hBtn2 = CreateWindow(TEXT("button"), TEXT("삭제"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)2, g_hInst, NULL);

	hGroup2 = CreateWindow(TEXT("button"), TEXT("명령창 :"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hCmdEdit = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		0, 0, 0, 0, hWnd, (HMENU)10, g_hInst, NULL);
	hR3 = CreateWindow(TEXT("button"), TEXT("HEX"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hR4 = CreateWindow(TEXT("button"), TEXT("ASCII"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hCK4 = CreateWindow(TEXT("button"), TEXT("\\r 자동삽입"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hBtn3 = CreateWindow(TEXT("button"), TEXT("보내기"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)3, g_hInst, NULL);
	SetFocus(hBtn3);
	SendMessage(hCK1, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(hCK2, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(hR2, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(hR4, BM_SETCHECK, BST_CHECKED, 0);
	tie.mask = TCIF_TEXT;
	tie.pszText = (LPSTR)("HostPC");
	TabCtrl_InsertItem(hTab, 0, &tie);
	/*tie.pszText = (LPSTR)("two");
	TabCtrl_InsertItem(hTab, 1, &tie);
	tie.pszText = (LPSTR)("three");
	TabCtrl_InsertItem(hTab, 2, &tie);*/
	
	DWORD ThreadID;
	CloseHandle(CreateThread(NULL, 0, ThreadSerialFunc, 0, 0, &ThreadID));
	return 0;
}

INT OnCommandChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	TCITEM tie;
	INT cnt = 0;
	TCHAR *rxHEXBuff = (TCHAR*)malloc(1024);
	INT length;
	INT index;
	INT indexSerial;	
	TCHAR strBuff[MAX_PATH];
	TCHAR retMes[MAX_PATH];
	TCHAR tabName[33];
	SYSTEMTIME st;
	TCHAR sTime[50];
	HANDLE hFile;
	TCHAR *buffSaveFile = (TCHAR*)malloc(32768);
	switch (LOWORD(wParam)) {
	case 0:		
		break;
	case 1: // 저장 버튼 누름
		DWORD dwWritten;
		//TCHAR *buffSaveFile = (TCHAR*)malloc(32768);
		INT cbWrite;
		tie.mask = TCIF_TEXT;
		tie.pszText = tabName;
		tie.cchTextMax = sizeof(tabName);
		index = TabCtrl_GetCurSel(hTab);
		TabCtrl_GetItem(hTab, index, &tie);
		GetLocalTime(&st);
		if (index == 0) {
			wsprintf(sTime, TEXT("%d%02d%02d_%d시%d분%d초_%s.txt"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, tabName);
		}
		else {
			wsprintf(sTime, TEXT("%d%02d%02d_%d시%d분%d초_%s.txt"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, &tabName[4]);
		}		
		hFile = CreateFile(sTime, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			MessageBox(hWnd, TEXT("파일 생성 실패!!"), TEXT("알림"), MB_OK);
			//free(buffSaveFile);
			break;
		}
		cbWrite = GetWindowText(hLogEdit[index], buffSaveFile, 32768);
		if (WriteFile(hFile, buffSaveFile, cbWrite, &dwWritten, NULL) == FALSE) {
			MessageBox(hWnd, TEXT("파일 쓰기 실패!!"), TEXT("알림"), MB_OK);
			CloseHandle(hFile);
			//free(buffSaveFile);
			break;
		}
		CloseHandle(hFile);
		//free(buffSaveFile);
		wsprintf(retMes, TEXT("현재 폴더에 %s 파일을 생성"), sTime);
		MessageBox(hWnd, retMes, TEXT("알림"), MB_OK);
		break;
	case 2: // 삭제 버튼 누름
		index = TabCtrl_GetCurSel(hTab);
		SendMessage(hLogEdit[index], EM_SETSEL, 0, -1);
		SendMessage(hLogEdit[index], WM_CLEAR, 0, 0);		
		break;
	case 3: // 명령 보내기 버튼 누름
		// 명령창에 입력 명령을 버퍼로 읽어들어 해당 장치로 보내고 출력 여부에 따라 에디트 창에 출력한다.		
		//cnt = SendMessage(hCmdEdit, EM_GETLINE, (WPARAM)0, (LPARAM)strBuff);
		cnt = SendMessage(hCmdEdit, WM_GETTEXT, (WPARAM)128, (LPARAM)strBuff);
		if(SendMessage(hCK4, BM_GETCHECK, 0, 0)!=BST_CHECKED) strBuff[cnt] = NULL;
		else { strBuff[cnt] = '\r'; strBuff[cnt + 1] = NULL; }
		// // 명령창 16진수 값 입력 여부 체크박스 확인 후 TRUE이면 문자열을 어레이 배열에 hex 값으로 변환 저장
		if (SendMessage(hR3, BM_GETCHECK, 0, 0) == BST_CHECKED) cnt = AsciiHextoHarr(rxHEXBuff, strBuff, cnt);		
		SendMessage(hCmdEdit, EM_SETSEL, 0, -1);
		SendMessage(hCmdEdit, WM_CLEAR, 0, 0);
		// 프로퍼티 시트 탭 네임 읽기
		tie.mask = TCIF_TEXT;
		tie.pszText = tabName;
		tie.cchTextMax = sizeof(tabName);
		index = TabCtrl_GetCurSel(hTab);
		if (index == 0 || index == -1) break;
		TabCtrl_GetItem(hTab, index, &tie);
		GetLocalTime(&st);
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_TX_%s]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, tabName);
		// 시리얼 포트 유효성 확인 아니면 브레이크
		if ((indexSerial = MySerial.GetConnectedPortNum(tie.pszText)) == -1) { TabCtrl_DeleteItem(hTab, index); break; }

		if (SendMessage(hR3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			// 명령창 16진수 값 입력 여부 체크박스 확인
			cnt = MySerial.SerialPort[indexSerial]->Send(rxHEXBuff, cnt, retMes);
		}
		else {
			cnt = MySerial.SerialPort[indexSerial]->Send(strBuff, lstrlen(strBuff), retMes);
		}		
		if (SendMessage(hCK1, BM_GETCHECK, 0, 0) == BST_UNCHECKED) break; // send 표시 안함
		if (SendMessage(hR3, BM_GETCHECK, 0, 0) == BST_CHECKED) { // hex 값으로 출력 체크 여부 확인
			// hex 값으로 어레이에 저장된 데이터를 문자열화 하여 에디트 창에 출력한다.
			cnt = AtoHstr(strBuff, rxHEXBuff, cnt);						
			*(strBuff + cnt) = '\r';
			*(strBuff + cnt + 1) = '\n';
			*(strBuff + cnt + 2) = NULL;
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strBuff);
			length = GetWindowTextLength(hLogEdit[index]);
			SendMessage(hLogEdit[index], EM_SETSEL, length, length);
			SendMessage(hLogEdit[index], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[index]);
			SendMessage(hLogEdit[index], EM_SETSEL, length, length);
			SendMessage(hLogEdit[index], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strBuff);
		}
		else {
			// 문자열 값을 에디트 창에 출력한다.
			strcat_s(strBuff, "\r\n");
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strBuff);
			length = GetWindowTextLength(hLogEdit[index]);
			SendMessage(hLogEdit[index], EM_SETSEL, length, length);
			SendMessage(hLogEdit[index], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[index]);
			SendMessage(hLogEdit[index], EM_SETSEL, length, length);
			SendMessage(hLogEdit[index], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strBuff);
		}					
		break;
	case 10:
		switch (HIWORD(wParam)) {
		case EN_SETFOCUS:
			//SetFocus(hBtn3);
//			DrawFocusRect(hdc, &rt);
			break;
		}
		break;
	case 11:
		// 명령창에 입력 명령을 버퍼로 읽어들어 해당 장치로 보내고 출력 여부에 따라 에디트 창에 출력한다.
		CopyData(strBuff, txBuff, dwBytesWrite);
		strBuff[dwBytesWrite] = NULL;			
		// 프로퍼티 시트 탭 네임 읽기
		tie.mask = TCIF_TEXT;
		tie.pszText = tabName;
		tie.cchTextMax = sizeof(tabName);
		index = TabCtrl_GetCurSel(hTab);
		if (index == -1) break;
		TabCtrl_GetItem(hTab, index, &tie);
		GetLocalTime(&st);
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_TX_%s]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, tabName);
		// 시리얼 포트 유효성 확인 아니면 브레이크
		if (MySerial.IsConnectedPort(tie.pszText) != TRUE) break;		
			
		if (SendMessage(hCK1, BM_GETCHECK, 0, 0) == BST_UNCHECKED) break; // send 표시 안함

		if (SendMessage(hR1, BM_GETCHECK, 0, 0) == BST_CHECKED) { // hex 값으로 출력 체크 여부 확인
			// hex 값으로 어레이에 저장된 데이터를 문자열화 하여 에디트 창에 출력한다.
			cnt = AtoHstr(rxHEXBuff, strBuff, dwBytesWrite);						
			*(rxHEXBuff + cnt) = '\r';
			*(rxHEXBuff + cnt + 1) = '\n';
			*(rxHEXBuff + cnt + 2) = NULL;
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxHEXBuff);
			length = GetWindowTextLength(hLogEdit[index]);
			SendMessage(hLogEdit[index], EM_SETSEL, length, length);
			SendMessage(hLogEdit[index], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[index]);
			SendMessage(hLogEdit[index], EM_SETSEL, length, length);
			SendMessage(hLogEdit[index], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxHEXBuff);
		}
		else {
			// 문자열 값을 에디트 창에 출력한다.
			strcat_s(strBuff, "\r\n");
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strBuff);
			length = GetWindowTextLength(hLogEdit[index]);
			SendMessage(hLogEdit[index], EM_SETSEL, length, length);
			SendMessage(hLogEdit[index], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[index]);
			SendMessage(hLogEdit[index], EM_SETSEL, length, length);
			SendMessage(hLogEdit[index], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strBuff);
		}					
		break;
	case 12:
		// 명령창에 입력 명령을 버퍼로 읽어들어 해당 장치로 보내고 출력 여부에 따라 에디트 창에 출력한다.
		CopyData(strBuff, txBuff, dwBytesWrite);
		strBuff[dwBytesWrite] = NULL;
		MySerial.SerialPort[lParam]->GetPortName(tabName);
		GetLocalTime(&st);
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_TX_%s]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, tabName);
		// 시리얼 포트 유효성 확인 아니면 브레이크
		if (MySerial.IsConnectedPort(tabName) != TRUE) break;

		if (SendMessage(hCK1, BM_GETCHECK, 0, 0) == BST_UNCHECKED) break; // send 표시 안함

		lParam = MySerial.SerialPort[lParam]->GetPortAttribute();

		if (SendMessage(hR1, BM_GETCHECK, 0, 0) == BST_CHECKED) { // hex 값으로 출력 체크 여부 확인
																  // hex 값으로 어레이에 저장된 데이터를 문자열화 하여 에디트 창에 출력한다.
			cnt = AtoHstr(rxHEXBuff, strBuff, dwBytesWrite);
			*(rxHEXBuff + cnt) = '\r';
			*(rxHEXBuff + cnt + 1) = '\n';
			*(rxHEXBuff + cnt + 2) = NULL;
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxHEXBuff);
			length = GetWindowTextLength(hLogEdit[lParam]);
			SendMessage(hLogEdit[lParam], EM_SETSEL, length, length);
			SendMessage(hLogEdit[lParam], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[lParam]);
			SendMessage(hLogEdit[lParam], EM_SETSEL, length, length);
			SendMessage(hLogEdit[lParam], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxHEXBuff);
		}
		else {
			// 문자열 값을 에디트 창에 출력한다.
			strcat_s(strBuff, "\r\n");
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[0]);
			SendMessage(hLogEdit[0], EM_SETSEL, length, length);
			SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strBuff);
			length = GetWindowTextLength(hLogEdit[lParam]);
			SendMessage(hLogEdit[lParam], EM_SETSEL, length, length);
			SendMessage(hLogEdit[lParam], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[lParam]);
			SendMessage(hLogEdit[lParam], EM_SETSEL, length, length);
			SendMessage(hLogEdit[lParam], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)strBuff);
		}
		break;
	}
	free(rxHEXBuff);
	free(buffSaveFile);
	return 0;
}

INT OnNotifyChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	INT index;
	INT cnt;
	switch (((LPNMHDR)lParam)->code) {
	case TCN_SELCHANGE:
		//SetWindowText(hStatic, arNum[TabCtrl_GetCurSel(hTab)]);
		index = TabCtrl_GetCurSel(hTab);
		for (cnt = 0; cnt < SERIALPORT_COUNT_MAX; cnt++) {
			if (cnt == index && hLogEdit[cnt] != INVALID_HANDLE_VALUE) {
				MoveWindow(hLogEdit[cnt], 5, 25, posLogW, posLogH, TRUE);
				ShowWindow(hLogEdit[cnt], SW_SHOW);
			}
		}
		for (cnt = 0; cnt < SERIALPORT_COUNT_MAX; cnt++) {
			if (cnt != index && hLogEdit[cnt] != INVALID_HANDLE_VALUE) ShowWindow(hLogEdit[cnt], SW_HIDE);
		}
		break;
	}
	return 0;
}

INT OnSizeChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	INT index;
	INT cnt;
	posLogW = LOWORD(lParam) - 20;
	posLogH = HIWORD(lParam) - 200;
	MoveWindow(hTab, 0, 0, LOWORD(lParam), HIWORD(lParam) - 160, TRUE);
	for (cnt = 0; cnt < SERIALPORT_COUNT_MAX; cnt++) {
		if(hLogEdit[cnt]!=INVALID_HANDLE_VALUE) MoveWindow(hLogEdit[cnt], 5, 25, LOWORD(lParam) - 20, HIWORD(lParam) - 200, TRUE);
	}
	MoveWindow(hGroup1, 5, HIWORD(lParam) - 155, LOWORD(lParam) - 20, 40, TRUE);
	MoveWindow(hR1, 95, HIWORD(lParam) - 140, 50, 20, TRUE);
	MoveWindow(hR2, 155, HIWORD(lParam) - 140, 60, 20, TRUE);
	MoveWindow(hCK1, 250, HIWORD(lParam) - 140, 90, 20, TRUE);
	MoveWindow(hCK2, 350, HIWORD(lParam) - 140, 120, 20, TRUE);	
	MoveWindow(hBtn1, 660, HIWORD(lParam) - 140, 70, 20, TRUE);
	MoveWindow(hBtn2, 740, HIWORD(lParam) - 140, 70, 20, TRUE);

	MoveWindow(hGroup2, 5, HIWORD(lParam) - 110, LOWORD(lParam) - 20, 60, TRUE);
	MoveWindow(hCmdEdit, 80, HIWORD(lParam) - 100, LOWORD(lParam) - 100, 20, TRUE);
	MoveWindow(hR3, 95, HIWORD(lParam) - 75, 50, 20, TRUE);
	MoveWindow(hR4, 155, HIWORD(lParam) - 75, 60, 20, TRUE);
	MoveWindow(hCK4, 250, HIWORD(lParam) - 75, 100, 20, TRUE);
	MoveWindow(hBtn3, 740, HIWORD(lParam) - 75, 70, 20, TRUE);
	//InvalidateRect(hWnd, NULL, TRUE);
	index = TabCtrl_GetCurSel(hTab);
	for (cnt = 0; cnt < SERIALPORT_COUNT_MAX; cnt++) {
		if (cnt == index && hLogEdit[cnt] != INVALID_HANDLE_VALUE) ShowWindow(hLogEdit[cnt], SW_SHOW);
	}
	for (cnt = 0; cnt < SERIALPORT_COUNT_MAX; cnt++) {
		if (cnt != index && hLogEdit[cnt] != INVALID_HANDLE_VALUE) ShowWindow(hLogEdit[cnt], SW_HIDE);
	}
	return 0;
}

INT OnUser1ChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{ // 터미널 추가 제거 버튼으로 부터 메시지 받는다.
	TCHAR str[MAX_PATH];
	TCHAR index = (TCHAR)lParam;	
	TCITEM tie;
	switch (LOWORD(wParam)) {
	case 1: // 활성화된 포트에 해당하는 탭 추가
		SendMessage(hComboTerminal, CB_GETLBTEXT, (WPARAM)index, (LPARAM)str);						
		tie.mask = TCIF_TEXT;
		tie.pszText = (LPSTR)(str);
		TabCtrl_InsertItem(hTab, index + 1, &tie);
		hLogEdit[index+1] = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL,
			0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
		SendMessage(hLogEdit[index+1], EM_LIMITTEXT, (WPARAM)102400, 0); // 입력 문자수 제한 기본 32k		
		break;
	case 2: // 탭 제거
		TabCtrl_DeleteItem(hTab, index + 1);
		DestroyWindow(hLogEdit[index + 1]);		
		break;
	}
	return 0;
}

INT OnUser2ChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{ // 로그 에디트 창에 수신 데이터를 출력한다.
	INT cnt;
	INT length;
	INT index = (INT)(LOWORD(wParam));	
	TCHAR *rxHEXBuff = (TCHAR *)malloc(1024);
	SYSTEMTIME st;
	TCHAR sTime[50];
	TCHAR portName[33];
	if (index == -1) return 0;
	MySerial.SerialPort[index]->GetPortName(portName);
	GetLocalTime(&st);
	wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_RX_%s]"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, portName);
	if (SendMessage(hCK2, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
		// receive 표시 출력 안함
		free(rxHEXBuff);
		return 0;
	}
	if (SendMessage(hR1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		// 시리얼 입력 버퍼 값을 hex 값 문자열로 변환 후 출력		
		cnt = AtoHstr(rxHEXBuff, rxBuff, dwBytesRead);	
		cnt = lstrlen(rxHEXBuff);
		*(rxHEXBuff + cnt) = '\r';
		*(rxHEXBuff + cnt + 1) = '\n';
		*(rxHEXBuff + cnt + 2) = NULL;
		length = GetWindowTextLength(hLogEdit[0]);
		SendMessage(hLogEdit[0], EM_SETSEL, length, length);
		SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
		length = GetWindowTextLength(hLogEdit[0]);
		SendMessage(hLogEdit[0], EM_SETSEL, length, length);
		SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxHEXBuff);
		if (lParam != 0) {
			length = GetWindowTextLength(hLogEdit[lParam]);
			SendMessage(hLogEdit[lParam], EM_SETSEL, length, length);
			SendMessage(hLogEdit[lParam], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[lParam]);
			SendMessage(hLogEdit[lParam], EM_SETSEL, length, length);
			SendMessage(hLogEdit[lParam], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxHEXBuff);
		}			
	}
	else {
		// 시리얼 입력 버퍼 값을 문자열로 출력
		strcat_s(rxBuff, "\r\n");
		length = GetWindowTextLength(hLogEdit[0]);
		SendMessage(hLogEdit[0], EM_SETSEL, length, length);
		SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
		length = GetWindowTextLength(hLogEdit[0]);
		SendMessage(hLogEdit[0], EM_SETSEL, length, length);
		SendMessage(hLogEdit[0], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxBuff);
		if (lParam != 0) {
			length = GetWindowTextLength(hLogEdit[lParam]);
			SendMessage(hLogEdit[lParam], EM_SETSEL, length, length);
			SendMessage(hLogEdit[lParam], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			length = GetWindowTextLength(hLogEdit[lParam]);
			SendMessage(hLogEdit[lParam], EM_SETSEL, length, length);
			SendMessage(hLogEdit[lParam], EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxBuff);
		}		
	}
	free(rxHEXBuff);
	return 0;
}

INT OnPaintChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	//HBRUSH MyBrush, OldBrush;
	hdc = BeginPaint(hWnd, &ps);
	//MyBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	//MyBrush = CreateSolidBrush(RGB(128, 128, 128));
	//OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);	
	//SelectObject(hdc, OldBrush);
	//DeleteObject(MyBrush);
	EndPaint(hWnd, &ps);
	return 0;
}

DWORD WINAPI ThreadSerialFunc(LPVOID value)
{
	INT index;	
	TCHAR portName[32];
	TCHAR retMes[MAX_PATH];
	TCHAR cmdValue;
	BOOL flagConnectedPort;
	INT portAttribute;	
	for (;;) {
		Sleep(100);
		for(index =0; index<SERIALPORT_COUNT_MAX; index++) {
			flagConnectedPort = MySerial.GetFlagConnectedPort(index);			
			if (flagConnectedPort != TRUE) continue;			
			dwBytesRead = MySerial.SerialPort[index]->Recv(rxBuff, sizeof(rxBuff), retMes);
			if (GetFlagIDScan() == TRUE && GetInvPortNum() == index) SetInvSerialBuff(rxBuff, dwBytesRead);
			MySerial.SerialPort[index]->GetPortName(portName);
			portAttribute = MySerial.SerialPort[index]->GetPortAttribute();
			// 수신 상태 확인
			if (CoordiR->ComparePortName(portName) == 0 && (cmdValue = CoordiR->ProcessING()) != FALSE)
				SendMessage(hCoordi, WM_USER + 1, (WPARAM)cmdValue, 0); // 수신 메시지 처리, 수신 데이터 유무 확인
			if (dwBytesRead != -1 && dwBytesRead != 0)
				SendMessage(hCRB, WM_USER + 2, index, portAttribute); // 로그창에 수신데이터 표시
			if ((cmdValue = CoordiR->GetTXCmd()) != FALSE) // 명령플래그 확인
				SendMessage(hCoordi, WM_USER + 2, (WPARAM)cmdValue, 0); // 명령송신	루프 실행
																		// 명령 모드 종료 메시지 수신 확인하여 처리 루프 실행
			if (strncmp("CMD time out", rxBuff, 12) == 0) SendMessage(hCoordi, WM_USER + 1, (WPARAM)CMD_TIMEOUT, 0);
			PurgeBuff(rxBuff, sizeof(rxBuff));
			PurgeBuff(txBuff, sizeof(txBuff));
			dwBytesRead = 0;
			dwBytesWrite = 0;
			Sleep(500);
			//ClearCommError();
		}
		//Sleep(700);
		//return 1;
	}
	return 0;
}
