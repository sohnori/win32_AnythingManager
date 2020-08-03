#include "DlgProc_ConnectPort.h"
#include <CommCtrl.h>
#include "MySockCommon.h"
#include "resource.h"


//#pragma comment(lib, "Iphlpapi.lib")

static struct tagDCBInfoStr DCBInfoStr;
static struct tagNetworkInfo networkInfo;

static HWND hDlgSerial, hDlgNetwork;

// 포트 연결 대화상자 메시지 프로시저 처리 함수
INT OnInitDialogConnectPortProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT OnCommandConnectPortProc(HWND hDlg, WPARAM wParam, LPARAM lParam);

// 시리얼 통신 대화상자 메시지 프로시저 처리 함수
INT OnInitDialogSerialDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT OnCommandSerialDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam);

// 네트워크 통신 대화상자 메시지 프로시저 처리 함수
INT OnInitDialogNetworkDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT OnCommandNetworkDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam);

// 함수

// 시리얼통신 관련정보 구조체 반환
tagDCBInfoStr GetDCBInfoStr()
{
	return DCBInfoStr;
}

// 네트워크 관련정보 구조체 반환
tagNetworkInfo GetNetworkInfo()
{
	return networkInfo;
}

static VOID ResetNetworkInfo()
{
	networkInfo.localIPstr[0] = NULL;
	networkInfo.remoteIPstr[0] = NULL;
	networkInfo.portNumStr[0] = NULL;
	networkInfo.localIPlong = 0;
	networkInfo.remoteIPlong = 0;
	networkInfo.portNumShort = 0;
}

BOOL CALLBACK ConnectPortDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG: OnInitDialogConnectPortProc(hDlg, wParam, lParam); return TRUE;
	case WM_COMMAND: OnCommandConnectPortProc(hDlg, wParam, lParam); return TRUE;
	}
	// 대화상자 메시지를 처리하지 못하면 FALSE를 리턴하여 운영체제가 디폴트 처리하도록 한다.
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

// 터미널 추가 대화상자 하위 네트워크 상세 대화상자 메시지 프로시저
BOOL CALLBACK NetworkDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG:	OnInitDialogNetworkDlgProc(hDlg, wParam, lParam); return TRUE;
	case WM_COMMAND: OnCommandNetworkDlgProc(hDlg, wParam, lParam); return TRUE;
	}
	return FALSE;
}

INT OnInitDialogConnectPortProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	hDlgSerial = CreateDialog(0, MAKEINTRESOURCE(IDD_DIALOG_SERIAL), hDlg, SerialDlgProc);
	SetWindowPos(hDlgSerial, NULL, 0, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	hDlgNetwork = CreateDialog(0, MAKEINTRESOURCE(IDD_DIALOG_NETWORK), hDlg, NetworkDlgProc);
	SetWindowPos(hDlgNetwork, NULL, 0, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	CheckDlgButton(hDlg, IDC_RADIO_SERIAL, BST_CHECKED);
	ShowWindow(hDlgSerial, SW_SHOW);
	ShowWindow(hDlgNetwork, SW_HIDE);
	return 0;
}

INT OnCommandConnectPortProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	INT index = 0;	
	if (IsDlgButtonChecked(hDlg, IDC_RADIO_SERIAL) == BST_CHECKED) {
		ShowWindow(hDlgNetwork, SW_HIDE);
		ShowWindow(hDlgSerial, SW_SHOW);		
	}
	else {
		ShowWindow(hDlgSerial, SW_HIDE);
		ShowWindow(hDlgNetwork, SW_SHOW);
	}
	switch (LOWORD(wParam)) {
	case IDC_RADIO_SERIAL:
		break;
	case IDC_RADIO_TCP_CNT:
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), FALSE);
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_IPADDRESS_REMOTE), TRUE);
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_CHECK_BROADCAST), FALSE);
		break;
	case IDC_RADIO_TCP_S:
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), TRUE);
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_IPADDRESS_REMOTE), FALSE);
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_CHECK_BROADCAST), FALSE);
		break;
	case IDC_RADIO_UDP_S:
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), TRUE);
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_IPADDRESS_REMOTE), TRUE);
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_CHECK_BROADCAST), TRUE);
		break;
	case IDC_RADIO_UDP_CNT:
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), FALSE);
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_IPADDRESS_REMOTE), TRUE);
		EnableWindow(GetDlgItem(hDlgNetwork, IDC_CHECK_BROADCAST), TRUE);
		break;
	case IDOK:
		ResetNetworkInfo();
		if (IsDlgButtonChecked(hDlg, IDC_RADIO_SERIAL) == BST_CHECKED) {
			//포트 이름
			index = SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SPORT), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SPORT), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.portName);
			if (index == -1) DCBInfoStr.portName[0] = NULL;
			// 보레이트
			index = SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SBR), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SBR), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.baudrate);
			// 데이터 비트
			index = SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SDB), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SDB), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.dataBits);
			// 패러티 비트
			index = SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SPB), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SPB), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.parity);
			// 스탑 비트
			index = SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SSB), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SSB), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.stopBits);
			// 플로우 컨트롤
			index = SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SFC), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_SFC), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.flowControl);
			// 타임아웃
			index = SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_STO), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgSerial, IDC_COMBO_STO), CB_GETLBTEXT, index, (LPARAM)DCBInfoStr.timeout);
			index = IDSERIAL;
		}
		else if (IsDlgButtonChecked(hDlg, IDC_RADIO_TCP_CNT) == BST_CHECKED) {
			// 원격 IP			
			index = SendMessage(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), CB_GETCURSEL, 0, 0);			
			index = GetDlgItemText(hDlgNetwork, IDC_IPADDRESS_REMOTE, networkInfo.remoteIPstr, sizeof(networkInfo.remoteIPstr));
			index = GetDlgItemText(hDlgNetwork, IDC_EDIT_PORTNUM, networkInfo.portNumStr, sizeof(networkInfo.portNumStr));
			index = HostAddrStrToLong(&networkInfo.remoteIPlong, networkInfo.remoteIPstr, MY_ADDR_HOST);
			networkInfo.portNumShort = atoi(networkInfo.portNumStr);
			index = IDTCPCNT;			
		}
		else if (IsDlgButtonChecked(hDlg, IDC_RADIO_TCP_S) == BST_CHECKED) {
			// 지역 IP
			index = SendMessage(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), CB_GETLBTEXT, index, (LPARAM)networkInfo.localIPstr);			
			index = GetDlgItemText(hDlgNetwork, IDC_EDIT_PORTNUM, networkInfo.portNumStr, sizeof(networkInfo.portNumStr));
			index = HostAddrStrToLong(&networkInfo.localIPlong, networkInfo.localIPstr, MY_ADDR_HOST);
			networkInfo.portNumShort = atoi(networkInfo.portNumStr);
			index = IDTCPSERVER;
		}
		else if (IsDlgButtonChecked(hDlg, IDC_RADIO_UDP_S) == BST_CHECKED) {
			index = SendMessage(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), CB_GETLBTEXT, index, (LPARAM)networkInfo.localIPstr);
			index = GetDlgItemText(hDlgNetwork, IDC_IPADDRESS_REMOTE, networkInfo.remoteIPstr, sizeof(networkInfo.remoteIPstr));
			index = GetDlgItemText(hDlgNetwork, IDC_EDIT_PORTNUM, networkInfo.portNumStr, sizeof(networkInfo.portNumStr));
			index = HostAddrStrToLong(&networkInfo.localIPlong, networkInfo.localIPstr, MY_ADDR_HOST);
			index = HostAddrStrToLong(&networkInfo.remoteIPlong, networkInfo.remoteIPstr, MY_ADDR_HOST);
			networkInfo.portNumShort = atoi(networkInfo.portNumStr);
			index = IDUDPSERVER;
		}
		else if (IsDlgButtonChecked(hDlg, IDC_RADIO_UDP_CNT) == BST_CHECKED) {
			index = SendMessage(GetDlgItem(hDlgNetwork, IDC_COMBO_LOCALIP), CB_GETCURSEL, 0, 0);
			index = GetDlgItemText(hDlgNetwork, IDC_IPADDRESS_REMOTE, networkInfo.remoteIPstr, sizeof(networkInfo.remoteIPstr));
			index = GetDlgItemText(hDlgNetwork, IDC_EDIT_PORTNUM, networkInfo.portNumStr, sizeof(networkInfo.portNumStr));
			index = HostAddrStrToLong(&networkInfo.remoteIPlong, networkInfo.remoteIPstr, MY_ADDR_HOST);
			networkInfo.portNumShort = atoi(networkInfo.portNumStr);
			index = IDUDPCNT;
		}
		else index = IDOK;

		EndDialog(hDlgSerial, IDOK);
		EndDialog(hDlgNetwork, IDOK);
		EndDialog(hDlg, index); // 대화상자를 닫으며 해당 연결 인텍스를 반환
		break;
	case IDCANCEL:
		EndDialog(hDlgSerial, IDCANCEL);
		EndDialog(hDlgNetwork, IDCANCEL);
		EndDialog(hDlg, IDCANCEL);
		break;
	}
	return 0;
}

INT OnInitDialogSerialDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
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
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_SBR), CB_SETCURSEL, 3, 0); // 3번째 항목 선택
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
	//time out
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_STO), CB_ADDSTRING, 0, (LPARAM)TEXT("100"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_STO), CB_ADDSTRING, 0, (LPARAM)TEXT("300"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_STO), CB_ADDSTRING, 0, (LPARAM)TEXT("500"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_STO), CB_ADDSTRING, 0, (LPARAM)TEXT("800"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_STO), CB_ADDSTRING, 0, (LPARAM)TEXT("1000"));
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_STO), CB_SETCURSEL, 1, 0);
	return TRUE;
}

INT OnCommandSerialDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	INT cnt;	
	TCHAR portName[20];	
	HANDLE hSerial;	
	switch (LOWORD(wParam)) {
	case IDC_COMBO_SPORT:
		switch (HIWORD(wParam)) {					
		case CBN_DBLCLK:
		case CBN_CLOSEUP:
		case CBN_KILLFOCUS:		
		case CBN_EDITCHANGE:
		case CBN_EDITUPDATE:
		case CBN_ERRSPACE:			
			break;
		case CBN_SELCHANGE:
		case CBN_SELENDCANCEL:
		case CBN_SELENDOK:
			break;
		case CBN_DROPDOWN:
		case CBN_SETFOCUS:
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPORT), CB_RESETCONTENT, 0, 0);
			for (cnt = 0; cnt < 256; cnt++) {
				wsprintf(portName, "\\\\.\\COM%d", cnt);
				hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (hSerial != INVALID_HANDLE_VALUE) {
					SendMessage(GetDlgItem(hDlg, IDC_COMBO_SPORT), CB_ADDSTRING, 0, (LPARAM)portName);
					CloseHandle(hSerial);
				}
			}
			break;
		}
		break;
	case IDC_COMBO_SBR:
		//switch (HIWORD(wParam)) {}
		break;
	case IDC_COMBO_SDB:
		//switch (HIWORD(wParam)) {}
		break;
	case IDC_COMBO_SPB:
		//switch (HIWORD(wParam)) {}
		break;
	case IDC_COMBO_SSB:
		//switch (HIWORD(wParam)) {}
		break;
	case IDC_COMBO_SFC:
		//switch (HIWORD(wParam)) {}
		break;	
	}
	return TRUE;
}

INT OnInitDialogNetworkDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	DWORD address = MAKEIPADDRESS(127, 0, 0, 1);		
	//SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_REMOTE), IPM_GETADDRESS, 0, (LPARAM)&address);
	SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_REMOTE), IPM_SETADDRESS, 0, address);
	//SendMessage(GetDlgItem(hDlg, IDC_IPADDRESS_REMOTE), IPM_CLEARADDRESS, 0, 0);
	
	
	SetDlgItemText(hDlg, IDC_EDIT_PORTNUM, TEXT("5000"));
	
	return TRUE;
}

INT OnCommandNetworkDlgProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	
	TCHAR str[MAX_PATH];
	INT cnt;
	INT index;
	ULONG hostLocalip[5];
	switch (LOWORD(wParam)) {
	case IDC_COMBO_LOCALIP:
		switch (HIWORD(wParam)) {
		case CBN_DBLCLK:
		case CBN_CLOSEUP:
		case CBN_KILLFOCUS:
		case CBN_EDITCHANGE:
		case CBN_EDITUPDATE:
		case CBN_ERRSPACE:
			break;
		case CBN_SELCHANGE:
		case CBN_SELENDCANCEL:
		case CBN_SELENDOK:
			break;
		case CBN_DROPDOWN:
		case CBN_SETFOCUS:
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_LOCALIP), CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_LOCALIP, CB_ADDSTRING, 0, (LPARAM)"0.0.0.0");	// INADDR_ANY
			SendDlgItemMessage(hDlg, IDC_COMBO_LOCALIP, CB_ADDSTRING, 0, (LPARAM)"127.0.0.1");// INADDR_LOOPBACK
			index = GetLocalIP(hostLocalip, 5);
			if (index == -1) return TRUE;
			for (cnt = 0; cnt < index; cnt++) {
				HostAddrLongToStr(str, hostLocalip[cnt], sizeof(str));
				SendDlgItemMessage(hDlg, IDC_COMBO_LOCALIP, CB_ADDSTRING, 0, (LPARAM)str);	//
			}
			break;
		}
		break;
	case IDC_IPADDRESS_REMOTE:
		//switch (HIWORD(wParam)) {}
		break;	
	case IDC_CHECK_BROADCAST:
		if (IsDlgButtonChecked(hDlg, IDC_CHECK_BROADCAST)) {
			SetDlgItemText(hDlg, IDC_IPADDRESS_REMOTE, TEXT("255.255.255.255"));
		}
		//switch (HIWORD(wParam)) {}
		break;
	case IDC_EDIT_PORTNUM:
		//switch (HIWORD(wParam)) {}
		break;
	}
	return TRUE;
}
