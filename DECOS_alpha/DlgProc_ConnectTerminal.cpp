#include "DlgProc_ConnectTerminal.h"
#include "SerialCommHandler.h"
#include "resource.h"

static HWND hWndSerial;

extern HWND hCRB;
extern HWND hComboTerminal;
extern HINSTANCE g_hInst;
extern SerialCommHandler MySerial;

// 통신 연결 대화상자 메시지 프로시저 처리 함수
INT OnInitDialogConnectDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT OnCommandConnectDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam);

// 시리얼 통신 대화상자 메시지 프로시저 처리 함수
INT OnInitDialogSerialDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT OnCommandSerialDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam);

// 터미널 추가 대화상자 메시지 프로시저
BOOL CALLBACK ConnectDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG: OnInitDialogConnectDlgProc(hDlg, wParam, lParam); return TRUE;
	case WM_COMMAND: OnCommandConnectDlgProc(hDlg, wParam, lParam); break;
	}
	return FALSE;
}

// 터미널 추가 대화상자 하위 시리얼 포트 상세 대화상자 메시지 프로시저
BOOL CALLBACK SerialDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG:	OnInitDialogSerialDlgProc(hDlg, wParam, lParam); return TRUE;
	case WM_COMMAND: OnCommandSerialDlgProc(hDlg, wParam, lParam); return TRUE;
	}
	return FALSE;
}

INT OnInitDialogConnectDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	hWndSerial = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_SERIAL), hDlg, SerialDlgProc);
	SetWindowPos(hWndSerial, NULL, 0, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//SetWindowPos(GetDlgItem(hWndSerial, IDD_DIALOG_SERIAL), HWND_TOP, 2, 52, 0, 0, NULL);
	ShowWindow(hWndSerial, SW_HIDE);
	return TRUE;
}

INT OnCommandConnectDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	INT index;
	tagDCBInfo dcbInfoDlg;
	TCHAR strBuff[MAX_PATH];
	INT cntTerminal;
	if (IsDlgButtonChecked(hDlg, IDC_RADIO_SERIAL) == BST_CHECKED) {
		ShowWindow(hWndSerial, SW_SHOW);
	}
	else {
		ShowWindow(hWndSerial, SW_HIDE);
	}
	switch (LOWORD(wParam)) {
	case IDOK:		
		if ((cntTerminal=SendMessage(hComboTerminal, CB_GETCOUNT, 0, 0))>= SERIALPORT_COUNT_MAX) {
			MessageBox(hDlg, TEXT("더이상 터미널을 연결할 수 없습니다."), TEXT("경고"), MB_OK);
			EndDialog(hWndSerial, IDCANCEL);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}				
		if (IsDlgButtonChecked(hDlg, IDC_RADIO_SERIAL) == BST_CHECKED) {
			index = SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_PORT), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_PORT), CB_GETLBTEXT, index, (LPARAM)dcbInfoDlg.portName);
			index = SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_BR), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_BR), CB_GETLBTEXT, index, (LPARAM)strBuff);
			dcbInfoDlg.baudrate = atoi(strBuff);
			index = SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_DBN), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_DBN), CB_GETLBTEXT, index, (LPARAM)strBuff);
			dcbInfoDlg.dataBits = atoi(strBuff);
			index = SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_PB), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_PB), CB_GETLBTEXT, index, (LPARAM)strBuff);
			if (strcmp("None", strBuff) == 0) dcbInfoDlg.parity = 0;
			if (strcmp("Odd", strBuff) == 0) dcbInfoDlg.parity = 1;
			if (strcmp("Even", strBuff) == 0) dcbInfoDlg.parity = 2;
			index = SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_SB), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_SB), CB_GETLBTEXT, index, (LPARAM)strBuff);
			if (strcmp("1", strBuff) == 0) dcbInfoDlg.stopBits = 0;
			if (strcmp("1.5", strBuff) == 0) dcbInfoDlg.stopBits = 1;
			if (strcmp("2", strBuff) == 0) dcbInfoDlg.stopBits = 2;
			index = SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_FC), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hWndSerial, IDC_COMBO_FC), CB_GETLBTEXT, index, (LPARAM)strBuff);
			if (strcmp("None", strBuff) == 0) dcbInfoDlg.flowControl = 0;
			if (strcmp("Xon/Xoff", strBuff) == 0) dcbInfoDlg.flowControl = 1;
			if (strcmp("RTS/CTS", strBuff) == 0) dcbInfoDlg.flowControl = 2;
			dcbInfoDlg.timeout = 250;
			if (MySerial.ConnectPort(dcbInfoDlg) != TRUE) {
				MessageBox(hDlg, TEXT("장치를 연결할 수 없습니다!!"), TEXT("경고"), MB_OK);
				return TRUE;
			}
		}
		MySerial.SerialPort[atoi(&dcbInfoDlg.portName[7])]->SetPortAttribute(cntTerminal+1);
		SendMessage(hComboTerminal, CB_ADDSTRING, 0, (LPARAM)dcbInfoDlg.portName);
		// 로그 에디트 창 탭 추가 - 연결된 터미널 리스트 콤보창의 인덱스 번호를 넘긴다.
		SendMessage(hCRB, WM_USER + 1, (WPARAM)1, (LPARAM)cntTerminal); 
		EndDialog(hWndSerial, IDOK);
		EndDialog(hDlg, IDOK);
		return TRUE;
	case IDCANCEL:
		EndDialog(hWndSerial, IDCANCEL);
		EndDialog(hDlg, IDCANCEL);
		return TRUE;
	}
	return TRUE;
}

INT OnInitDialogSerialDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	//1200;2400;4800;9600;19200;38400;57600;115200;230400;460800;921600;	
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("1200"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("2400"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("4800"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("9600"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("19200"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("38400"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("57600"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("115200"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("230400"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("460800"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_ADDSTRING, 0, (LPARAM)TEXT("921600"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_BR), CB_SETCURSEL, 3, 0); // 3번째 항목 선택	
	//5; 6; 7; 8;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DBN), CB_ADDSTRING, 0, (LPARAM)TEXT("5"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DBN), CB_ADDSTRING, 0, (LPARAM)TEXT("6"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DBN), CB_ADDSTRING, 0, (LPARAM)TEXT("7"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DBN), CB_ADDSTRING, 0, (LPARAM)TEXT("8"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DBN), CB_SETCURSEL, 3, 0);	
	//None;Odd;Even;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_PB), CB_ADDSTRING, 0, (LPARAM)TEXT("None"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_PB), CB_ADDSTRING, 0, (LPARAM)TEXT("Odd"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_PB), CB_ADDSTRING, 0, (LPARAM)TEXT("Even"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_PB), CB_SETCURSEL, 0, 0);	
	//1; 1.5; 2;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SB), CB_ADDSTRING, 0, (LPARAM)TEXT("1"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SB), CB_ADDSTRING, 0, (LPARAM)TEXT("1.5"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SB), CB_ADDSTRING, 0, (LPARAM)TEXT("2"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SB), CB_SETCURSEL, 0, 0);	
	//None;Xon/Xoff;RTS/CTS;
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_FC), CB_ADDSTRING, 0, (LPARAM)TEXT("None"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_FC), CB_ADDSTRING, 0, (LPARAM)TEXT("Xon/Xoff"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_FC), CB_ADDSTRING, 0, (LPARAM)TEXT("RTS/CTS"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_FC), CB_SETCURSEL, 0, 0);	
	return TRUE;
}

INT OnCommandSerialDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam)) {
	case IDC_COMBO_PORT:
		switch (HIWORD(wParam)) {
		case CBN_CLOSEUP:
		case CBN_DBLCLK:
		case CBN_DROPDOWN:
		case CBN_EDITCHANGE:
		case CBN_EDITUPDATE:
		case CBN_ERRSPACE:
			break;
		case CBN_KILLFOCUS:
			break;
		case CBN_SELCHANGE:			
			break;
		case CBN_SELENDCANCEL:
		case CBN_SELENDOK:
			break;
		case CBN_SETFOCUS:
			TCHAR str[MAX_PATH];
			TCHAR portN[MAX_PATH];
			INT cnt;
			INT cntPort;
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_PORT), CB_RESETCONTENT, 0, 0);			
			cntPort = MySerial.GetAvailablePortNum(portN, sizeof(portN));
			for (cnt = 0; cnt < cntPort; cnt++) {
				wsprintf(str, "\\\\.\\COM%d", portN[cnt]);
				SendMessage(GetDlgItem(hDlg, IDC_COMBO_PORT), CB_ADDSTRING, 0, (LPARAM)str);				
			}
			break;
		}		
		break;
	case IDC_COMBO_BR:		
		break;
	case IDC_COMBO_DBN:		
		break;
	case IDC_COMBO_PB:		
		break;
	case IDC_COMBO_SB:		
		break;
	case IDC_COMBO_FC:		
		break;
	}
	return TRUE;
}