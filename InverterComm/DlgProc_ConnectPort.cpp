#include "DlgProc_ConnectPort.h"
#include "SerialCommHandler.h"
#include "resource.h"

extern SerialCommHandler Serial;

// ��Ʈ ���� ��ȭ���� �޽��� ���ν��� ó�� �Լ�
INT OnInitDialogConnectPortProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT OnCommandConnectPortProc(HWND hDlg, WPARAM wParam, LPARAM lParam);

tagDCBInfoStr DCBInfoStr;

BOOL CALLBACK ConnectPortDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG: OnInitDialogConnectPortProc(hDlg, wParam, lParam); return TRUE;
	case WM_COMMAND: OnCommandConnectPortProc(hDlg, wParam, lParam); return TRUE;
	}
	// ��ȭ���� �޽����� ó������ ���ϸ� FALSE�� �����Ͽ� �ü���� ����Ʈ ó���ϵ��� �Ѵ�.
	return FALSE;
}

INT OnInitDialogConnectPortProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	Serial.ClosePorts();
	//1200;2400;4800;9600;19200;38400;57600;115200;230400;460800;921600;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("1200"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("2400"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("4800"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("9600"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("19200"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("38400"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("57600"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("115200"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("230400"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("460800"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_ADDSTRING, 0, (LPARAM)TEXT("921600"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_SETCURSEL, 3, 0); // 3��° �׸� ����
	//5; 6; 7; 8;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SDB), CB_ADDSTRING, 0, (LPARAM)TEXT("5"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SDB), CB_ADDSTRING, 0, (LPARAM)TEXT("6"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SDB), CB_ADDSTRING, 0, (LPARAM)TEXT("7"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SDB), CB_ADDSTRING, 0, (LPARAM)TEXT("8"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SDB), CB_SETCURSEL, 3, 0);
	//None;Odd;Even;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPB), CB_ADDSTRING, 0, (LPARAM)TEXT("None"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPB), CB_ADDSTRING, 0, (LPARAM)TEXT("Odd"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPB), CB_ADDSTRING, 0, (LPARAM)TEXT("Even"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPB), CB_SETCURSEL, 0, 0);
	//1; 1.5; 2;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SSB), CB_ADDSTRING, 0, (LPARAM)TEXT("1"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SSB), CB_ADDSTRING, 0, (LPARAM)TEXT("1.5"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SSB), CB_ADDSTRING, 0, (LPARAM)TEXT("2"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SSB), CB_SETCURSEL, 0, 0);
	//None;Xon/Xoff;RTS/CTS;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SFC), CB_ADDSTRING, 0, (LPARAM)TEXT("None"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SFC), CB_ADDSTRING, 0, (LPARAM)TEXT("Xon/Xoff"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SFC), CB_ADDSTRING, 0, (LPARAM)TEXT("RTS/CTS"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SFC), CB_SETCURSEL, 0, 0);
	return TRUE;
}

INT OnCommandConnectPortProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	INT cnt;
	INT index;
	TCHAR portList[256];
	TCHAR portName[20];
	tagDCBInfo DCBInfo;	
	switch (LOWORD(wParam)) {
	case IDC_COMBO_SPORT:
		switch (HIWORD(wParam)) {
		case CBN_CLOSEUP:
		case CBN_DBLCLK:
		case CBN_DROPDOWN:
		case CBN_EDITCHANGE:
		case CBN_EDITUPDATE:
		case CBN_ERRSPACE:		
		case CBN_KILLFOCUS:
		case CBN_SELCHANGE:
		case CBN_SELENDCANCEL:
		case CBN_SELENDOK:
			break;
		case CBN_SETFOCUS:
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPORT), CB_RESETCONTENT, 0, 0);
			Serial.GetAvailablePortNum(portList, sizeof(portList));
			for (cnt = 0; portList[cnt] != NULL; cnt++) {
				wsprintf(portName, "\\\\.\\COM%d", portList[cnt]);
				SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPORT), CB_ADDSTRING, 0, (LPARAM)portName);
			}
		}
		break;
	case IDC_COMBO_SBR:
		switch (HIWORD(wParam)) {
		}
		break;
	case IDC_COMBO_SDB:
		switch (HIWORD(wParam)) {
		}
		break;
	case IDC_COMBO_SPB:
		switch (HIWORD(wParam)) {
		}
		break;
	case IDC_COMBO_SSB:
		switch (HIWORD(wParam)) {
		}
		break;
	case IDC_COMBO_SFC:
		switch (HIWORD(wParam)) {
		}
		break;	
	case IDOK:		
		// ��Ʈ �̸�
		index = SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPORT), CB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPORT), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.portName);
		strcpy_s(DCBInfo.portName, DCBInfoStr.portName);
		// ������Ʈ
		index = SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.baudrate);
		DCBInfo.baudrate = atoi(DCBInfoStr.baudrate);
		// ������ ��Ʈ
		index = SendMessage(GetDlgItem(hDlg, IDC_COMBO_SDB), CB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO_SDB), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.dataBits);
		DCBInfo.dataBits = atoi(DCBInfoStr.dataBits);
		// �з�Ƽ ��Ʈ
		index = SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPB), CB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPB), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.parity);
		if (strcmp("None", DCBInfoStr.parity) == 0) DCBInfo.parity = 0;
		if (strcmp("Odd", DCBInfoStr.parity) == 0) DCBInfo.parity = 1;
		if (strcmp("Even", DCBInfoStr.parity) == 0) DCBInfo.parity = 2;
		// ��ž ��Ʈ
		index = SendMessage(GetDlgItem(hDlg, IDC_COMBO_SSB), CB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO_SSB), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.stopBits);
		if (strcmp("1", DCBInfoStr.stopBits) == 0) DCBInfo.stopBits = 0;
		if (strcmp("1.5", DCBInfoStr.stopBits) == 0) DCBInfo.stopBits = 1;
		if (strcmp("2", DCBInfoStr.stopBits) == 0) DCBInfo.stopBits = 2;
		// �÷ο� ��Ʈ��
		index = SendMessage(GetDlgItem(hDlg, IDC_COMBO_SFC), CB_GETCURSEL, 0, 0);
		SendMessage(GetDlgItem(hDlg, IDC_COMBO_SFC), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.flowControl);
		if (strcmp("None", DCBInfoStr.flowControl) == 0) DCBInfo.flowControl = 0;
		if (strcmp("Xon/Xoff", DCBInfoStr.flowControl) == 0) DCBInfo.flowControl = 1;
		if (strcmp("RTS/CTS", DCBInfoStr.flowControl) == 0) DCBInfo.flowControl = 2;
		index = atoi(&DCBInfo.portName[7]);
		if (Serial.ConnectPort(DCBInfo, index) != TRUE) { MessageBox(hDlg, Serial.GetErrMSGPtr(), TEXT("�˸�"), MB_OK); break; }
		EndDialog(hDlg, IDOK);
		break;
	case IDCANCEL:
		EndDialog(hDlg, IDCANCEL);
		break;
	}
	return TRUE;
}