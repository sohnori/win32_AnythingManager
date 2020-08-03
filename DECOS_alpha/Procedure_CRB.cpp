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

// ������ �Լ�
DWORD WINAPI ThreadSerialFunc(LPVOID value);

// �����Ʒ��� ���ϵ��� �޽��� ���ν��� ó�� �Լ�
INT OnCreateChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnCommandChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnNotifyChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnSizeChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnUser1ChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnUser2ChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnPaintChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam);

// �����Ʒ��� ���ϵ��� �޽��� ���ν���
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
	SendMessage(hLogEdit[0], EM_LIMITTEXT, (WPARAM)102400, 0); // �Է� ���ڼ� ���� �⺻ 32k
	//SetWindowText(hLogEdit0, TEXT("����Ʈ ������0�Դϴ�."));
	//InitCommonControls();
	hTab = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_FORCELABELLEFT,
		0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
	hGroup1 = CreateWindow(TEXT("button"), TEXT("ǥ��â :"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hR1 = CreateWindow(TEXT("button"), TEXT("HEX"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hR2 = CreateWindow(TEXT("button"), TEXT("ASCII"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hCK1 = CreateWindow(TEXT("button"), TEXT("Send ǥ��"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hCK2 = CreateWindow(TEXT("button"), TEXT("Receive ǥ��"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);	
	hBtn1 = CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)1, g_hInst, NULL);
	hBtn2 = CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)2, g_hInst, NULL);

	hGroup2 = CreateWindow(TEXT("button"), TEXT("���â :"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hCmdEdit = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		0, 0, 0, 0, hWnd, (HMENU)10, g_hInst, NULL);
	hR3 = CreateWindow(TEXT("button"), TEXT("HEX"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hR4 = CreateWindow(TEXT("button"), TEXT("ASCII"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hCK4 = CreateWindow(TEXT("button"), TEXT("\\r �ڵ�����"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
	hBtn3 = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
	case 1: // ���� ��ư ����
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
			wsprintf(sTime, TEXT("%d%02d%02d_%d��%d��%d��_%s.txt"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, tabName);
		}
		else {
			wsprintf(sTime, TEXT("%d%02d%02d_%d��%d��%d��_%s.txt"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, &tabName[4]);
		}		
		hFile = CreateFile(sTime, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			MessageBox(hWnd, TEXT("���� ���� ����!!"), TEXT("�˸�"), MB_OK);
			//free(buffSaveFile);
			break;
		}
		cbWrite = GetWindowText(hLogEdit[index], buffSaveFile, 32768);
		if (WriteFile(hFile, buffSaveFile, cbWrite, &dwWritten, NULL) == FALSE) {
			MessageBox(hWnd, TEXT("���� ���� ����!!"), TEXT("�˸�"), MB_OK);
			CloseHandle(hFile);
			//free(buffSaveFile);
			break;
		}
		CloseHandle(hFile);
		//free(buffSaveFile);
		wsprintf(retMes, TEXT("���� ������ %s ������ ����"), sTime);
		MessageBox(hWnd, retMes, TEXT("�˸�"), MB_OK);
		break;
	case 2: // ���� ��ư ����
		index = TabCtrl_GetCurSel(hTab);
		SendMessage(hLogEdit[index], EM_SETSEL, 0, -1);
		SendMessage(hLogEdit[index], WM_CLEAR, 0, 0);		
		break;
	case 3: // ��� ������ ��ư ����
		// ���â�� �Է� ����� ���۷� �о��� �ش� ��ġ�� ������ ��� ���ο� ���� ����Ʈ â�� ����Ѵ�.		
		//cnt = SendMessage(hCmdEdit, EM_GETLINE, (WPARAM)0, (LPARAM)strBuff);
		cnt = SendMessage(hCmdEdit, WM_GETTEXT, (WPARAM)128, (LPARAM)strBuff);
		if(SendMessage(hCK4, BM_GETCHECK, 0, 0)!=BST_CHECKED) strBuff[cnt] = NULL;
		else { strBuff[cnt] = '\r'; strBuff[cnt + 1] = NULL; }
		// // ���â 16���� �� �Է� ���� üũ�ڽ� Ȯ�� �� TRUE�̸� ���ڿ��� ��� �迭�� hex ������ ��ȯ ����
		if (SendMessage(hR3, BM_GETCHECK, 0, 0) == BST_CHECKED) cnt = AsciiHextoHarr(rxHEXBuff, strBuff, cnt);		
		SendMessage(hCmdEdit, EM_SETSEL, 0, -1);
		SendMessage(hCmdEdit, WM_CLEAR, 0, 0);
		// ������Ƽ ��Ʈ �� ���� �б�
		tie.mask = TCIF_TEXT;
		tie.pszText = tabName;
		tie.cchTextMax = sizeof(tabName);
		index = TabCtrl_GetCurSel(hTab);
		if (index == 0 || index == -1) break;
		TabCtrl_GetItem(hTab, index, &tie);
		GetLocalTime(&st);
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_TX_%s]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, tabName);
		// �ø��� ��Ʈ ��ȿ�� Ȯ�� �ƴϸ� �극��ũ
		if ((indexSerial = MySerial.GetConnectedPortNum(tie.pszText)) == -1) { TabCtrl_DeleteItem(hTab, index); break; }

		if (SendMessage(hR3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			// ���â 16���� �� �Է� ���� üũ�ڽ� Ȯ��
			cnt = MySerial.SerialPort[indexSerial]->Send(rxHEXBuff, cnt, retMes);
		}
		else {
			cnt = MySerial.SerialPort[indexSerial]->Send(strBuff, lstrlen(strBuff), retMes);
		}		
		if (SendMessage(hCK1, BM_GETCHECK, 0, 0) == BST_UNCHECKED) break; // send ǥ�� ����
		if (SendMessage(hR3, BM_GETCHECK, 0, 0) == BST_CHECKED) { // hex ������ ��� üũ ���� Ȯ��
			// hex ������ ��̿� ����� �����͸� ���ڿ�ȭ �Ͽ� ����Ʈ â�� ����Ѵ�.
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
			// ���ڿ� ���� ����Ʈ â�� ����Ѵ�.
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
		// ���â�� �Է� ����� ���۷� �о��� �ش� ��ġ�� ������ ��� ���ο� ���� ����Ʈ â�� ����Ѵ�.
		CopyData(strBuff, txBuff, dwBytesWrite);
		strBuff[dwBytesWrite] = NULL;			
		// ������Ƽ ��Ʈ �� ���� �б�
		tie.mask = TCIF_TEXT;
		tie.pszText = tabName;
		tie.cchTextMax = sizeof(tabName);
		index = TabCtrl_GetCurSel(hTab);
		if (index == -1) break;
		TabCtrl_GetItem(hTab, index, &tie);
		GetLocalTime(&st);
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_TX_%s]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, tabName);
		// �ø��� ��Ʈ ��ȿ�� Ȯ�� �ƴϸ� �극��ũ
		if (MySerial.IsConnectedPort(tie.pszText) != TRUE) break;		
			
		if (SendMessage(hCK1, BM_GETCHECK, 0, 0) == BST_UNCHECKED) break; // send ǥ�� ����

		if (SendMessage(hR1, BM_GETCHECK, 0, 0) == BST_CHECKED) { // hex ������ ��� üũ ���� Ȯ��
			// hex ������ ��̿� ����� �����͸� ���ڿ�ȭ �Ͽ� ����Ʈ â�� ����Ѵ�.
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
			// ���ڿ� ���� ����Ʈ â�� ����Ѵ�.
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
		// ���â�� �Է� ����� ���۷� �о��� �ش� ��ġ�� ������ ��� ���ο� ���� ����Ʈ â�� ����Ѵ�.
		CopyData(strBuff, txBuff, dwBytesWrite);
		strBuff[dwBytesWrite] = NULL;
		MySerial.SerialPort[lParam]->GetPortName(tabName);
		GetLocalTime(&st);
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_TX_%s]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, tabName);
		// �ø��� ��Ʈ ��ȿ�� Ȯ�� �ƴϸ� �극��ũ
		if (MySerial.IsConnectedPort(tabName) != TRUE) break;

		if (SendMessage(hCK1, BM_GETCHECK, 0, 0) == BST_UNCHECKED) break; // send ǥ�� ����

		lParam = MySerial.SerialPort[lParam]->GetPortAttribute();

		if (SendMessage(hR1, BM_GETCHECK, 0, 0) == BST_CHECKED) { // hex ������ ��� üũ ���� Ȯ��
																  // hex ������ ��̿� ����� �����͸� ���ڿ�ȭ �Ͽ� ����Ʈ â�� ����Ѵ�.
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
			// ���ڿ� ���� ����Ʈ â�� ����Ѵ�.
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
{ // �͹̳� �߰� ���� ��ư���� ���� �޽��� �޴´�.
	TCHAR str[MAX_PATH];
	TCHAR index = (TCHAR)lParam;	
	TCITEM tie;
	switch (LOWORD(wParam)) {
	case 1: // Ȱ��ȭ�� ��Ʈ�� �ش��ϴ� �� �߰�
		SendMessage(hComboTerminal, CB_GETLBTEXT, (WPARAM)index, (LPARAM)str);						
		tie.mask = TCIF_TEXT;
		tie.pszText = (LPSTR)(str);
		TabCtrl_InsertItem(hTab, index + 1, &tie);
		hLogEdit[index+1] = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL,
			0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);
		SendMessage(hLogEdit[index+1], EM_LIMITTEXT, (WPARAM)102400, 0); // �Է� ���ڼ� ���� �⺻ 32k		
		break;
	case 2: // �� ����
		TabCtrl_DeleteItem(hTab, index + 1);
		DestroyWindow(hLogEdit[index + 1]);		
		break;
	}
	return 0;
}

INT OnUser2ChildRBProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{ // �α� ����Ʈ â�� ���� �����͸� ����Ѵ�.
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
		// receive ǥ�� ��� ����
		free(rxHEXBuff);
		return 0;
	}
	if (SendMessage(hR1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		// �ø��� �Է� ���� ���� hex �� ���ڿ��� ��ȯ �� ���		
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
		// �ø��� �Է� ���� ���� ���ڿ��� ���
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
			// ���� ���� Ȯ��
			if (CoordiR->ComparePortName(portName) == 0 && (cmdValue = CoordiR->ProcessING()) != FALSE)
				SendMessage(hCoordi, WM_USER + 1, (WPARAM)cmdValue, 0); // ���� �޽��� ó��, ���� ������ ���� Ȯ��
			if (dwBytesRead != -1 && dwBytesRead != 0)
				SendMessage(hCRB, WM_USER + 2, index, portAttribute); // �α�â�� ���ŵ����� ǥ��
			if ((cmdValue = CoordiR->GetTXCmd()) != FALSE) // ����÷��� Ȯ��
				SendMessage(hCoordi, WM_USER + 2, (WPARAM)cmdValue, 0); // ��ɼ۽�	���� ����
																		// ��� ��� ���� �޽��� ���� Ȯ���Ͽ� ó�� ���� ����
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
