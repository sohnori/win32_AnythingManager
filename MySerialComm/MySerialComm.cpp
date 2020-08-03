// 외부 MFC 오버헤더를 포함하지 않는 지시자_SDK를 사용
#define WIN32_LEAN_AND_MEAN 

#include <Windows.h>
#include "MySerialComm.h"
#include "SerialComm.h"
#include "SerialCommHandler.h"
#include "MyTcp.h"
#include "MyUdp.h"
#include "DlgProc_ConnectPort.h"
#include "MyFunc.h"
#include <CommCtrl.h>
#include <stdlib.h>
#include "resource.h"
#include "MySockCommon.h"
#include "CRC.h"

// need link with Ws2_32.lib
//#pragma comment(lib, "Ws2_32.lib") 

// 시리얼 포트 연결 클래스
SerialCommHandler serialCommHandler;

// 네트워크 클래스
MyTcpClient *myTcpClient = 0;
MyTcpServer *myTcpServer = 0;
MyUdpServer *myUdpServer = 0;
MyUdpClient *myUdpClient = 0;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	//WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(225, 225, 225)); // 회색
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		WND_INITSIZE_W, WND_INITSIZE_H, NULL, (HMENU)NULL, hInstance, NULL);
	hWndMain = hWnd;
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_CREATE: return OnCreate(hWnd, wParam, lParam);
	case WM_COMMAND: OnCommand(hWnd, wParam, lParam); break;
	case WM_DEVICECHANGE: OnDeviceChange(hWnd, wParam, lParam); return 0;
	case WM_USER_THREAD_OUTPUT:	OnUserThreadOutput(hWnd, wParam, lParam); return 0;
	case WM_SIZE: OnSize(hWnd, wParam, lParam); return 0;
	case WM_TIMER: OnTimer(hWnd, wParam, lParam); return 0;
	case WM_PAINT: OnPaint(hWnd, wParam, lParam); return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		SetWindowLongPtr(hEditSend, GWLP_WNDPROC, (LONG_PTR)OldWndProc);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	hBtnTerminal = CreateWindow(TEXT("button"), TEXT("Open"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN1_INIT_POSX, BTN1_INIT_POSY, BTN1_SIZEW, BTN1_SIZEH, hWnd, (HMENU)CONNECT_PORT, g_hInst, NULL);
	hBtnSave = CreateWindow(TEXT("button"), TEXT("출력저장"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)SAVE_OUTPUT, g_hInst, NULL);
	hBtnClear = CreateWindow(TEXT("button"), TEXT("지우기"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)CLEAR_OUTPUT, g_hInst, NULL);
	hBtnSend = CreateWindow(TEXT("button"), TEXT("보내기"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)SEND_CMD, g_hInst, NULL);

	hGBtn1 = CreateWindow(TEXT("button"), TEXT("보내는 입력"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		0, 0, 0, 0, hWnd, (HMENU)1, g_hInst, NULL);
	hStatic1 = CreateWindow(TEXT("static"), strNull, WS_CHILD | WS_VISIBLE,
		STT1_INIT_POSX, STT1_INIT_POSY, STT1_SIZEW, STT1_SIZEH, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic2 = CreateWindow(TEXT("static"), strNull, WS_CHILD | WS_VISIBLE,
		STT2_INIT_POSX, STT2_INIT_POSY, STT2_SIZEW, STT2_SIZEH, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic3 = CreateWindow(TEXT("static"), strNull, WS_CHILD | WS_VISIBLE,
		STT3_INIT_POSX, STT3_INIT_POSY, STT3_SIZEW, STT3_SIZEH, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic4 = CreateWindow(TEXT("static"), strNull, WS_CHILD | WS_VISIBLE,
		STT4_INIT_POSX, STT4_INIT_POSY, STT4_SIZEW, STT4_SIZEH, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic5 = CreateWindow(TEXT("static"), strNull, WS_CHILD | WS_VISIBLE,
		STT5_INIT_POSX, STT5_INIT_POSY, STT5_SIZEW, STT5_SIZEH, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic6 = CreateWindow(TEXT("static"), strNull, WS_CHILD | WS_VISIBLE,
		STT6_INIT_POSX, STT6_INIT_POSY, STT6_SIZEW, STT6_SIZEH, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic7 = CreateWindow(TEXT("static"), strNull, WS_CHILD | WS_VISIBLE,
		STT7_INIT_POSX, STT7_INIT_POSY, STT7_SIZEW, STT7_SIZEH, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic8 = CreateWindow(TEXT("static"), strNull, WS_CHILD | WS_VISIBLE,
		STT8_INIT_POSX, STT8_INIT_POSY, STT8_SIZEW, STT8_SIZEH, hWnd, (HMENU)-1, g_hInst, NULL);
	hEditOutput = CreateWindow(TEXT("edit"), NULL,
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER | WS_VSCROLL | WS_HSCROLL,
		0, 0, 0, 0, hWnd, (HMENU)-1, g_hInst, NULL);
	// 100k로 설정(입력 문자수 제한 기본 32k)
	SendMessage(hEditOutput, EM_LIMITTEXT, (WPARAM)102400, 0); 
	hEditSend = CreateWindow(TEXT("edit"), NULL,
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER,
		0, 0, 0, 0, hWnd, (HMENU)-1, g_hInst, NULL);
	hStatic9 = CreateWindow(TEXT("static"), TEXT("출력표시 :"), WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0, hWnd, (HMENU)-1, g_hInst, NULL);

	hRBtnHex1 = CreateWindow(TEXT("button"), TEXT("HEX"), 
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		0, 0, 0, 0, hWnd, (HMENU)ID_R101, g_hInst, NULL);
	hRBtnAscii1 = CreateWindow(TEXT("button"), TEXT("ASCII"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)ID_R102, g_hInst, NULL);
	hRBtnHex2 = CreateWindow(TEXT("button"), TEXT("HEX"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		0, 0, 0, 0, hWnd, (HMENU)ID_R103, g_hInst, NULL);
	hRBtnAscii2 = CreateWindow(TEXT("button"), TEXT("ASCII (Escape Seq. 처리)"),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)ID_R104, g_hInst, NULL);
	CheckRadioButton(hWnd, ID_R101, ID_R102, ID_R102);
	CheckRadioButton(hWnd, ID_R103, ID_R104, ID_R104);
	flagEditSendASCII = TRUE;
	hCBtnSend = CreateWindow(TEXT("button"), TEXT("송신표시"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, (HMENU)-1, g_hInst, NULL);
	hCBtnReceive = CreateWindow(TEXT("button"), TEXT("수신표시"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, (HMENU)-1, g_hInst, NULL);
	
	hCBtnCksum = CreateWindow(TEXT("button"), TEXT("체크썸 붙이기(자동)"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		0, 0, 0, 0, hWnd, (HMENU)ID_ADD_CKSUM, g_hInst, NULL);
	hBtnCRCmake = CreateWindow(TEXT("button"), TEXT("생성"), WS_CHILD | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)ID_BTN_MAKECKSUM, g_hInst, NULL);
	hComboCRC = CreateWindow(TEXT("combobox"), NULL, WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
		0, 0, 0, 0, hWnd, (HMENU)ID_COMBO_CRC, g_hInst, NULL);
	hRBtnCRC8 = CreateWindow(TEXT("button"), TEXT("CRC8"),
		WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
		0, 0, 0, 0, hWnd, (HMENU)ID_RCRC8, g_hInst, NULL);
	hRBtnCRC16 = CreateWindow(TEXT("button"), TEXT("CRC16"),
		WS_CHILD | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)ID_RCRC16, g_hInst, NULL);
	hRBtnCRC32 = CreateWindow(TEXT("button"), TEXT("CRC32"),
		WS_CHILD | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)ID_RCRC32, g_hInst, NULL);
	hRBtnUser = CreateWindow(TEXT("button"), TEXT("사용자정의"),
		WS_CHILD | BS_AUTORADIOBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)ID_RUSER, g_hInst, NULL);
	hEditCksum = CreateWindow(TEXT("edit"), NULL,
		WS_CHILD | WS_BORDER,
		0, 0, 0, 0, hWnd, (HMENU)ID_EDIT_CKSUM, g_hInst, NULL);
	CheckRadioButton(hWnd, ID_RCRC8, ID_RUSER, ID_RCRC8);
	SendMessage(hCBtnSend, BM_SETCHECK, TRUE, 0);
	SendMessage(hCBtnReceive, BM_SETCHECK, TRUE, 0);
	OldWndProc = (WNDPROC)SetWindowLongPtr(hEditSend, GWLP_WNDPROC, (LONG_PTR)SendEditProc);
	SetTimer(hWnd, 1, 1000, NULL);	
	return 0;
}

INT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	tagDCBInfoStr DCBInfoStr;
	tagDCBInfo DCBInfo;
	tagNetworkInfo networkInfo;
	TCHAR strBuff[MAX_PATH*2];
	INT index;
	SYSTEMTIME st;
	HANDLE hFile;
	INT cbWrite;
	TCHAR *fileBuff;
	DWORD dwWritten;
	DWORD ThreadID;	
	TCHAR retMes[MAX_PATH] = "";
	switch (LOWORD(wParam)) {
	case ID_SET_ORIGIN:
		MoveWindow(hWnd, 0, 0, WND_INITSIZE_W, WND_INITSIZE_H, TRUE);
		break;
	case CONNECT_PORT: // button 1
		// 기존 포트 초기화 및 연결해제하고 새로운 포트 연결한다.		
		portNum = 0;
		flagConnectedType = 0;		
		serialCommHandler.ClosePorts();
		// 텍스트들을 초기화한다.
		SetWindowText(hBtnTerminal, TEXT("Open"));
		SetWindowText(hStatic1, strNull);
		SetWindowText(hStatic2, strNull);
		SetWindowText(hStatic3, strNull);
		SetWindowText(hStatic4, strNull);
		SetWindowText(hStatic5, strNull);
		SetWindowText(hStatic6, strNull);
		SetWindowText(hStatic7, strNull);
		SetWindowText(hStatic8, strNull);
		index = DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_CONNECT), hWnd, ConnectPortDlgProc);
		if (index == IDSERIAL) {
			DCBInfoStr = GetDCBInfoStr();
			strcpy_s(DCBInfo.portName, DCBInfoStr.portName);
			DCBInfo.baudrate = atoi(DCBInfoStr.baudrate);
			DCBInfo.dataBits = atoi(DCBInfoStr.dataBits);
			if (strcmp("None", DCBInfoStr.parity) == 0) DCBInfo.parity = 0;
			if (strcmp("Odd", DCBInfoStr.parity) == 0) DCBInfo.parity = 1;
			if (strcmp("Even", DCBInfoStr.parity) == 0) DCBInfo.parity = 2;
			if (strcmp("1", DCBInfoStr.stopBits) == 0) DCBInfo.stopBits = 0;
			if (strcmp("1.5", DCBInfoStr.stopBits) == 0) DCBInfo.stopBits = 1;
			if (strcmp("2", DCBInfoStr.stopBits) == 0) DCBInfo.stopBits = 2;
			if (strcmp("None", DCBInfoStr.flowControl) == 0) DCBInfo.flowControl = 0;
			if (strcmp("Xon/Xoff", DCBInfoStr.flowControl) == 0) DCBInfo.flowControl = 1;
			if (strcmp("RTS/CTS", DCBInfoStr.flowControl) == 0) DCBInfo.flowControl = 2;
			DCBInfo.timeout = atoi(DCBInfoStr.timeout);
			index = atoi(&DCBInfo.portName[7]);
			if (serialCommHandler.ConnectPort(DCBInfo, index) != TRUE) {
				MessageBox(hWnd, serialCommHandler.GetErrMSGPtr(), TEXT("알림"), MB_OK);
				break;
			}
			portNum = index;			
			SetWindowText(hBtnTerminal, TEXT("Connecting"));
			wsprintf(strBuff, "%s%s", strPort, DCBInfoStr.portName);
			SetWindowText(hStatic1, strBuff);
			wsprintf(strBuff, "%s%s", strBR, DCBInfoStr.baudrate);
			SetWindowText(hStatic2, strBuff);
			wsprintf(strBuff, "%s%s", strDB, DCBInfoStr.dataBits);
			SetWindowText(hStatic3, strBuff);
			wsprintf(strBuff, "%s%s", strParity, DCBInfoStr.parity);
			SetWindowText(hStatic4, strBuff);
			wsprintf(strBuff, "%s%s", strStop, DCBInfoStr.stopBits);
			SetWindowText(hStatic5, strBuff);
			wsprintf(strBuff, "%s%s", strFlow, DCBInfoStr.flowControl);
			SetWindowText(hStatic6, strBuff);
			wsprintf(strBuff, "%s%s", strTimeOut, DCBInfoStr.timeout);
			SetWindowText(hStatic7, strBuff);
			CloseHandle(CreateThread(NULL, 0, ThreadSerialFunc, &portNum, 0, &ThreadID));
			flagConnectedType = IDSERIAL;
		}
		else if (index == IDTCPCNT) {
			networkInfo = GetNetworkInfo();			
			myTcpClient = new MyTcpClient(networkInfo.remoteIPlong, networkInfo.portNumShort, 1000);
			if (myTcpClient == NULL) {
				MessageBox(hWnd, TEXT("TCP client 동적할당 실패!"), TEXT("알림"), MB_OK);
				flagConnectedType = NULL;
				break;
			}
			SetWindowText(hBtnTerminal, TEXT("Connecting"));
			SetWindowText(hStatic1, TEXT("[TCP Client]"));
			SetWindowText(hStatic7, strRemoteIP);
			SetWindowText(hStatic8, networkInfo.remoteIPstr);
			wsprintf(strBuff, "%s%s", strNport, networkInfo.portNumStr);
			SetWindowText(hStatic4, strBuff);
			flagConnectedType = IDTCPCNT;
			CloseHandle(CreateThread(NULL, 0, ThreadNetworkFunc, &flagConnectedType, 0, &ThreadID));												
		}
		else if (index == IDTCPSERVER) {
			networkInfo = GetNetworkInfo();			
			myTcpServer = new MyTcpServer(networkInfo.localIPlong, networkInfo.portNumShort, 1000);
			if (myTcpServer == NULL) {
				MessageBox(hWnd, TEXT("TCP client 동적할당 실패!"), TEXT("알림"), MB_OK);
				flagConnectedType = NULL;
				break;
			}
			SetWindowText(hBtnTerminal, TEXT("Connecting"));
			SetWindowText(hStatic1, TEXT("[TCP SERVER]"));
			SetWindowText(hStatic5, strLocalIP);
			SetWindowText(hStatic6, networkInfo.localIPstr);
			SetWindowText(hStatic7, strRemoteIP);			
			wsprintf(strBuff, "%s%s", strNport, networkInfo.portNumStr);
			SetWindowText(hStatic4, strBuff);
			flagConnectedType = IDTCPSERVER;
			CloseHandle(CreateThread(NULL, 0, ThreadNetworkFunc, &flagConnectedType, 0, &ThreadID));
		}
		else if (index == IDUDPSERVER) {
			networkInfo = GetNetworkInfo();
			if (strlen(networkInfo.localIPstr) < 7) {
				MessageBox(hWnd, TEXT("Local IP를 입력하세요!"), TEXT("알림"), MB_OK);
				break;
			}
			myUdpServer = new MyUdpServer(networkInfo.localIPlong, networkInfo.remoteIPlong, networkInfo.portNumShort, 1000);
			if (myUdpServer == NULL) {
				MessageBox(hWnd, TEXT("UDP 동적할당 실패!"), TEXT("알림"), MB_OK);
				flagConnectedType = NULL;
				break;
			}
			SetWindowText(hBtnTerminal, TEXT("Connecting"));
			SetWindowText(hStatic1, TEXT("[UDP SERVER]"));
			SetWindowText(hStatic5, strLocalIP);
			SetWindowText(hStatic6, networkInfo.localIPstr);
			SetWindowText(hStatic7, strRemoteIP);
			SetWindowText(hStatic8, networkInfo.remoteIPstr);
			wsprintf(strBuff, "%s%s", strNport, networkInfo.portNumStr);
			SetWindowText(hStatic4, strBuff);
			flagConnectedType = IDUDPSERVER;
			CloseHandle(CreateThread(NULL, 0, ThreadNetworkFunc, &flagConnectedType, 0, &ThreadID));
		}
		else if (index == IDUDPCNT) {
			networkInfo = GetNetworkInfo();			
			myUdpClient = new MyUdpClient(networkInfo.localIPlong, networkInfo.remoteIPlong, networkInfo.portNumShort, 1000);
			if (myUdpClient == NULL) {
				MessageBox(hWnd, TEXT("UDP 동적할당 실패!"), TEXT("알림"), MB_OK);
				flagConnectedType = NULL;
				break;
			}
			SetWindowText(hBtnTerminal, TEXT("Connecting"));
			SetWindowText(hStatic1, TEXT("[UDP CLIENT]"));			
			SetWindowText(hStatic7, strRemoteIP);
			SetWindowText(hStatic8, networkInfo.remoteIPstr);
			wsprintf(strBuff, "%s%s", strNport, networkInfo.portNumStr);
			SetWindowText(hStatic4, strBuff);
			flagConnectedType = IDUDPCNT;
			CloseHandle(CreateThread(NULL, 0, ThreadNetworkFunc, &flagConnectedType, 0, &ThreadID));
		}
		break;
	case SAVE_OUTPUT: // button 2
		GetLocalTime(&st);
		wsprintf(strBuff, TEXT("%d%02d%02d_%d시%d분%d초_%s.txt"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, TEXT("MySerialComm"));
		hFile = CreateFile(strBuff, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			MessageBox(hWnd, TEXT("파일 생성 실패!!"), TEXT("알림"), MB_OK);
			break;
		}
		fileBuff = (TCHAR*)malloc(102400);
		cbWrite = GetWindowText(hEditOutput, fileBuff, 102400);
		if (WriteFile(hFile, fileBuff, cbWrite, &dwWritten, NULL) == FALSE) {
			MessageBox(hWnd, TEXT("파일 쓰기 실패!!"), TEXT("알림"), MB_OK);
			CloseHandle(hFile);
			free(fileBuff);
			break;
		}
		wsprintf(fileBuff, TEXT("현재 폴더에 %s 파일을 생성"), strBuff);
		MessageBox(hWnd, fileBuff, TEXT("알림"), MB_OK);
		CloseHandle(hFile);
		free(fileBuff);
		break;
	case CLEAR_OUTPUT: // button 3
		SendMessage(hEditOutput, EM_SETSEL, 0, -1);
		SendMessage(hEditOutput, WM_CLEAR, 0, 0);
		break;
	case SEND_CMD: // button 4
		INT cnt;			
		dwBytesWrite = SendMessage(hEditSend, WM_GETTEXT, (WPARAM)MAX_PATH, (LPARAM)strBuff);
		SendMessage(hEditSend, EM_SETSEL, 0, -1);
		SendMessage(hEditSend, WM_CLEAR, 0, 0);
		strcpy_s(txBuff, strBuff);
		if (flagConnectedType==0) {
			MessageBox(hWnd, TEXT("소켓이나 포트가 열리지 않았습니다!"), TEXT("Notice"), MB_OK);
			break;
		}
		if (flagConnectedType==IDSERIAL && portNum == 0) {
			MessageBox(hWnd, TEXT("포트가 열리지 않았습니다!"), TEXT("Notice"), MB_OK);
			break;
		}
		if ((flagConnectedType == IDTCPSERVER && myTcpServer->IsConnected() == FALSE) ||
			(flagConnectedType == IDTCPCNT && myTcpClient->IsConnected() == FALSE) || 
			(flagConnectedType == IDUDPSERVER && myUdpServer->IsConnected() == FALSE) || 
			(flagConnectedType == IDUDPCNT && myUdpClient->IsConnected() == FALSE)) {
			MessageBox(hWnd, TEXT("소켓이 연결되지 않았습니다!"), TEXT("Notice"), MB_OK);
			break;
		}
		if (SendMessage(hRBtnAscii2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			dwBytesWrite = ConvertAsciiArrEscapeSeq(txBuff, txBuff, dwBytesWrite);
			if (SendMessage(hCBtnCksum, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				// 체크썸 삽입 조건일 경우 계산된 체크썸 붙이기
				index = SendMessage(hComboCRC, CB_GETCURSEL, 0, 0);
				SendMessage(hComboCRC, CB_GETLBTEXT, index, (LPARAM)strBuff);
				if (SendMessage(hRBtnCRC8, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					UCHAR crc8;
					index = GetCRCTypeIndex(strBuff, 8);
					crc8 = get_CRC8((UCHAR*)txBuff, dwBytesWrite, index);
					txBuff[dwBytesWrite] = crc8;
					dwBytesWrite++;
				}
				else if (SendMessage(hRBtnCRC16, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					USHORT crc16;
					index = GetCRCTypeIndex(strBuff, 16);
					crc16 = get_CRC16((UCHAR*)txBuff, dwBytesWrite, index);
					txBuff[dwBytesWrite] = (TCHAR)crc16;
					dwBytesWrite++;
					txBuff[dwBytesWrite] = (TCHAR)(crc16 >> 8);
					dwBytesWrite++;
				}
				else if (SendMessage(hRBtnCRC32, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					LONG crc32;
					SHORT value;
					index = GetCRCTypeIndex(strBuff, 32);
					crc32 = get_CRC32((UCHAR*)txBuff, dwBytesWrite, index);
					value = (SHORT)crc32;
					txBuff[dwBytesWrite] = (TCHAR)value;
					dwBytesWrite++;
					txBuff[dwBytesWrite] = (TCHAR)(value >> 8);
					dwBytesWrite++;
					value = (SHORT)(crc32 >> 16);
					txBuff[dwBytesWrite] = (TCHAR)value;
					dwBytesWrite++;
					txBuff[dwBytesWrite] = (TCHAR)(value >> 8);
					dwBytesWrite++;
				}
				else if (SendMessage(hRBtnUser, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					UCHAR cksum;
					index = GetUserCksumIndex(strBuff);
					cksum = GetUserCksum(txBuff, dwBytesWrite, index);
					txBuff[dwBytesWrite] = cksum;
					dwBytesWrite++;
				}
			}
			if (flagConnectedType == IDSERIAL) cnt = serialCommHandler.SerialPort[portNum]->Send(txBuff, dwBytesWrite, retMes);
			else if (flagConnectedType == IDTCPSERVER) cnt = myTcpServer->Send(txBuff, dwBytesWrite);
			else if (flagConnectedType == IDTCPCNT) cnt = myTcpClient->Send(txBuff, dwBytesWrite);
			else if (flagConnectedType == IDUDPSERVER) cnt = myUdpServer->SendTo(txBuff, dwBytesWrite);
			else if (flagConnectedType == IDUDPCNT) cnt = myUdpClient->SendTo(txBuff, dwBytesWrite);
		}
		else if (SendMessage(hRBtnHex2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			dwBytesWrite = HexAsciiArrToHarr(txBuff, strBuff, dwBytesWrite);
			if (SendMessage(hCBtnCksum, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				// 체크썸 삽입 조건일 경우 계산된 체크썸 붙이기
				index = SendMessage(hComboCRC, CB_GETCURSEL, 0, 0);
				SendMessage(hComboCRC, CB_GETLBTEXT, index, (LPARAM)strBuff);
				if (SendMessage(hRBtnCRC8, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					UCHAR crc8;
					index = GetCRCTypeIndex(strBuff, 8);
					crc8 = get_CRC8((UCHAR*)txBuff, dwBytesWrite, index);
					txBuff[dwBytesWrite] = crc8;
					dwBytesWrite++;
				}
				else if (SendMessage(hRBtnCRC16, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					USHORT crc16;
					index = GetCRCTypeIndex(strBuff, 16);
					crc16 = get_CRC16((UCHAR*)txBuff, dwBytesWrite, index);
					txBuff[dwBytesWrite] = (TCHAR)crc16;
					dwBytesWrite++;
					txBuff[dwBytesWrite] = (TCHAR)(crc16 >> 8);
					dwBytesWrite++;
				}
				else if (SendMessage(hRBtnCRC32, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					LONG crc32;
					SHORT value;
					index = GetCRCTypeIndex(strBuff, 32);
					crc32 = get_CRC32((UCHAR*)txBuff, dwBytesWrite, index);
					value = (SHORT)crc32;
					txBuff[dwBytesWrite] = (TCHAR)value;
					dwBytesWrite++;
					txBuff[dwBytesWrite] = (TCHAR)(value >> 8);
					dwBytesWrite++;
					value = (SHORT)(crc32 >> 16);
					txBuff[dwBytesWrite] = (TCHAR)value;
					dwBytesWrite++;
					txBuff[dwBytesWrite] = (TCHAR)(value >> 8);
					dwBytesWrite++;
				}
				else if (SendMessage(hRBtnUser, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					UCHAR cksum;
					index = GetUserCksumIndex(strBuff);
					cksum = GetUserCksum(txBuff, dwBytesWrite, index);
					txBuff[dwBytesWrite] = cksum;
					dwBytesWrite++;
				}
			}
			if (flagConnectedType == IDSERIAL) cnt = serialCommHandler.SerialPort[portNum]->Send(txBuff, dwBytesWrite, retMes);
			else if (flagConnectedType == IDTCPSERVER) cnt = myTcpServer->Send(txBuff, dwBytesWrite);
			else if (flagConnectedType == IDTCPCNT) cnt = myTcpClient->Send(txBuff, dwBytesWrite);
			else if (flagConnectedType == IDUDPSERVER) cnt = myUdpServer->SendTo(txBuff, dwBytesWrite);
			else if (flagConnectedType == IDUDPCNT) cnt = myUdpClient->SendTo(txBuff, dwBytesWrite);
	
		}
		if (dwBytesWrite <= 0) break;		
		SendMessage(hWnd, WM_USER_THREAD_OUTPUT, (WPARAM)MY_THREAD_TX, (LPARAM)txBuff);
		break;
	case ID_R103: // 보내는 입력 HEX 라디오 버튼
		if (flagEditSendASCII != TRUE) break;
		cbWrite = SendMessage(hEditSend, WM_GETTEXT, (WPARAM)MAX_PATH, (LPARAM)strBuff);
		SendMessage(hEditSend, EM_SETSEL, 0, -1);
		SendMessage(hEditSend, WM_CLEAR, 0, 0);
		cbWrite = ConvertAsciiArrEscapeSeq(strBuff, strBuff, cbWrite);
		cbWrite = AsciiArrToHexAsciiArr(retMes, strBuff, cbWrite);
		SendMessage(hEditSend, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)retMes);
		break;
	case ID_R104: // 보내는 입력 ASCII 라디오 버튼
		flagEditSendASCII = 1;
		cbWrite = SendMessage(hEditSend, WM_GETTEXT, (WPARAM)MAX_PATH, (LPARAM)strBuff);
		SendMessage(hEditSend, EM_SETSEL, 0, -1);
		SendMessage(hEditSend, WM_CLEAR, 0, 0);
		cbWrite = HexAsciiArrToHarr(retMes, strBuff, cbWrite);
		SendMessage(hEditSend, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)retMes);
		break;
	case ID_BTN_MAKECKSUM:						
		SendMessage(hEditCksum, CB_RESETCONTENT, 0, 0);
		dwBytesWrite = SendMessage(hEditSend, WM_GETTEXT, (WPARAM)MAX_PATH*2, (LPARAM)strBuff);
		if (SendMessage(hRBtnAscii2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			dwBytesWrite = ConvertAsciiArrEscapeSeq(strBuff, strBuff, dwBytesWrite);			
		}
		else if (SendMessage(hRBtnHex2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			dwBytesWrite = HexAsciiArrToHarr(strBuff, strBuff, dwBytesWrite);
		}
		index = SendMessage(hComboCRC, CB_GETCURSEL, 0, 0);
		SendMessage(hComboCRC, CB_GETLBTEXT, index, (LPARAM)retMes);
		if (SendMessage(hRBtnCRC8, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			UCHAR crc8;
			index = GetCRCTypeIndex(retMes, 8);
			crc8 = get_CRC8((UCHAR*)strBuff, dwBytesWrite, index);
			wsprintf(retMes, "%02X", crc8);			
		}
		else if (SendMessage(hRBtnCRC16, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			USHORT crc16;
			index = GetCRCTypeIndex(retMes, 16);
			crc16 = get_CRC16((UCHAR*)strBuff, dwBytesWrite, index);
			wsprintf(retMes, "%04X", crc16);
		}
		else if (SendMessage(hRBtnCRC32, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			LONG crc32;
			index = GetCRCTypeIndex(retMes, 32);
			crc32 = get_CRC32((UCHAR*)strBuff, dwBytesWrite, index);
			wsprintf(retMes, "%08X", crc32);
		}
		else if (SendMessage(hRBtnUser, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			UCHAR cksum;
			index = GetUserCksumIndex(retMes);
			cksum = GetUserCksum(strBuff, dwBytesWrite, index);
			wsprintf(retMes, "%02X", cksum);
		}
		SetWindowText(hEditCksum, retMes);
		break;
	case ID_ADD_CKSUM:
		if (SendMessage(hCBtnCksum, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			ShowWindow(hRBtnCRC8, SW_SHOW);
			ShowWindow(hRBtnCRC16, SW_SHOW);
			ShowWindow(hRBtnCRC32, SW_SHOW);
			ShowWindow(hRBtnUser, SW_SHOW);
			ShowWindow(hComboCRC, SW_SHOW);
			ShowWindow(hEditCksum, SW_SHOW);
			ShowWindow(hBtnCRCmake, SW_SHOW);
		}
		else {
			ShowWindow(hRBtnCRC8, SW_HIDE);
			ShowWindow(hRBtnCRC16, SW_HIDE);
			ShowWindow(hRBtnCRC32, SW_HIDE);
			ShowWindow(hRBtnUser, SW_HIDE);
			ShowWindow(hComboCRC, SW_HIDE);
			ShowWindow(hEditCksum, SW_HIDE);
			ShowWindow(hBtnCRCmake, SW_HIDE);
		}		
		break;
	case ID_COMBO_CRC:
		switch (HIWORD(wParam)) {
		case CBN_SETFOCUS:
			SendMessage(hComboCRC, CB_RESETCONTENT, 0, 0);
			if (SendMessage(hRBtnCRC8, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_CDMA2000");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_DARC");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_DVB_S2");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_EBU");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_I_CODE");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_ITU");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_MAXIM");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_ROHC");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_WCDMA");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_AUTOSAR");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_BLUETOOTH");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC8_LTE");
			}
			else if (SendMessage(hRBtnCRC16, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_CCITT_FALSE");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_ARC");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_AUR_CCITT");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_BUYPASS");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_CDMA2000");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_DDS_110");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_DECT_R");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_DECT_X");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_DNP");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_EN_13757");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_GENIBUS");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_MAXIM");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_MCRF4XX");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_RIELLO");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_T10_DIF");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_TELEDISK");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_TMS37157");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_USB");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC_A");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_KERMIT");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_MODBUS");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_X_25");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_XMODEM");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC16_TEST");
			}
			else if (SendMessage(hRBtnCRC32, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32_BZIP2");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32_C");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32_D");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32_MPEG2");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32_POSIX");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32_Q");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32_JAMCRC");
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"CRC32_XFER");
			}
			else if (SendMessage(hRBtnUser, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				SendMessage(hComboCRC, CB_ADDSTRING, 0, (LPARAM)"PROTOCOL_DE");
			}
			break;			
		}
		break;
	case ID_EDIT_CKSUM:
		break;
	case ID_RCRC8:
		SendMessage(hComboCRC, CB_RESETCONTENT, 0, 0);
		SendMessage(hEditCksum, CB_RESETCONTENT, 0, 0);
		break;
	case ID_RCRC16:
		SendMessage(hComboCRC, CB_RESETCONTENT, 0, 0);
		SendMessage(hEditCksum, CB_RESETCONTENT, 0, 0);
		break;
	case ID_RCRC32:
		SendMessage(hComboCRC, CB_RESETCONTENT, 0, 0);
		SendMessage(hEditCksum, CB_RESETCONTENT, 0, 0);
		break;
	case ID_RUSER:
		SendMessage(hComboCRC, CB_RESETCONTENT, 0, 0);
		SendMessage(hEditCksum, CB_RESETCONTENT, 0, 0);
		break;
	default:
		break;
	}
	return 0;
}

INT OnDeviceChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

INT OnUserThreadOutput(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	INT length;
	INT cnt;	
	SYSTEMTIME st;
	TCHAR sTime[50];
	TCHAR portName[33] = "";	
	TCHAR *outputBuff = (TCHAR *)malloc(1024);
	if (portNum != 0) serialCommHandler.SerialPort[portNum]->GetPortName(portName, sizeof(portName));
	GetLocalTime(&st);
	switch (LOWORD(wParam)) {
	case MY_THREAD_RX:
		if (SendMessage(hCBtnReceive, BM_GETCHECK, 0, 0) == BST_UNCHECKED) break;		
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_RX_%s]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, portName);
		length = GetWindowTextLength(hEditOutput);
		SendMessage(hEditOutput, EM_SETSEL, length, length);		
		SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
		if (flagConnectedType == IDTCPCNT) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)myTcpClient->GetErrorMSG());
		else if (flagConnectedType == IDTCPSERVER) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)myTcpServer->GetErrorMSG());
		else if (flagConnectedType == IDUDPSERVER) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)myUdpServer->GetErrorMSG());
		else if (flagConnectedType == IDUDPCNT) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)myUdpClient->GetErrorMSG());
		length = GetWindowTextLength(hEditOutput);
		SendMessage(hEditOutput, EM_SETSEL, length, length);
		*(rxBuff + dwBytesRead) = '\r';
		*(rxBuff + dwBytesRead + 1) = '\n';
		*(rxBuff + dwBytesRead + 2) = NULL;
		if (SendMessage(hRBtnAscii1, BM_GETCHECK, 0, 0) == BST_CHECKED) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)rxBuff);
		else if (SendMessage(hRBtnHex1, BM_GETCHECK, 0, 0) == BST_CHECKED) {			
			INT cnt;
			cnt = HarrToHexASCIIstr(outputBuff, rxBuff, dwBytesRead);
			cnt = lstrlen(outputBuff);
			*(outputBuff + cnt) = '\r';
			*(outputBuff + cnt + 1) = '\n';
			*(outputBuff + cnt + 2) = NULL;
			SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)outputBuff);
		}
		break;
	case MY_THREAD_TX:
		if (SendMessage(hCBtnSend, BM_GETCHECK, 0, 0) == BST_UNCHECKED) break;
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_TX_%s]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, portName);
		length = GetWindowTextLength(hEditOutput);
		SendMessage(hEditOutput, EM_SETSEL, length, length);
		SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
		if (flagConnectedType == IDTCPCNT) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)myTcpClient->GetErrorMSG());
		else if (flagConnectedType == IDTCPSERVER) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)myTcpServer->GetErrorMSG());
		else if (flagConnectedType == IDUDPSERVER) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)myUdpServer->GetErrorMSG());
		else if (flagConnectedType == IDUDPCNT) SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)myUdpClient->GetErrorMSG());
		length = GetWindowTextLength(hEditOutput);
		SendMessage(hEditOutput, EM_SETSEL, length, length);		
		if (SendMessage(hRBtnAscii1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			lstrcpy(outputBuff, (TCHAR*)lParam);
			cnt = lstrlen(outputBuff);			
		}
		else if (SendMessage(hRBtnHex1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			length = dwBytesWrite;			
			cnt = HarrToHexASCIIstr(outputBuff, (TCHAR*)lParam, length);			
		}
		else break;

		*(outputBuff + cnt) = '\r';
		*(outputBuff + cnt + 1) = '\n';
		*(outputBuff + cnt + 2) = NULL;
		SendMessage(hEditOutput, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)outputBuff);
		break;
	}
	free(outputBuff);
	return 0;
}

INT OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintf(sTime, TEXT("%d-%02d-%02d %02d:%02d:%02d"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	InvalidateRect(hWnd, &rtTime, TRUE);  // 한정된 구역만 무효화한다.
	return 1;
}

INT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{			
	RECT rt;
	GetClientRect(hWnd, &rt);
	rtTime.left = rt.right - 140;
	rtTime.right = rt.right;
	rtTime.top = rt.bottom - 20;
	rtTime.bottom = rt.bottom;
	MoveWindow(hStatic9, 20, HIWORD(lParam) - 170, 80, 15, TRUE);
	
	MoveWindow(hEditOutput, CONTROL_INIT_X, 60, LOWORD(lParam) - 40, HIWORD(lParam) - 240, TRUE);

	MoveWindow(hGBtn1, CONTROL_INIT_X, HIWORD(lParam) - 140, LOWORD(lParam) - 40, 110, TRUE);
	MoveWindow(hEditSend, 20, HIWORD(lParam) - 115, LOWORD(lParam) - 140, 20, TRUE);
	MoveWindow(hBtnSend, LOWORD(lParam) - 100, HIWORD(lParam) - 115, 60, 20, TRUE);
	
	MoveWindow(hRBtnHex1, 110, HIWORD(lParam) - 170, 55, 20, TRUE);
	MoveWindow(hRBtnAscii1, 180, HIWORD(lParam) - 170, 55, 20, TRUE);
	MoveWindow(hCBtnSend, 260, HIWORD(lParam) - 170, 90, 20, TRUE);
	MoveWindow(hCBtnReceive, 360, HIWORD(lParam) - 170, 90, 20, TRUE);
	MoveWindow(hBtnSave, 560, HIWORD(lParam) - 170, 80, 20, TRUE);
	MoveWindow(hBtnClear, 660, HIWORD(lParam) - 170, 80, 20, TRUE);

	MoveWindow(hRBtnHex2, 110, HIWORD(lParam) - 90, 55, 20, TRUE);
	MoveWindow(hRBtnAscii2, 180, HIWORD(lParam) - 90, 190, 20, TRUE);
	MoveWindow(hCBtnCksum, 390, HIWORD(lParam) - 90, 160, 20, TRUE);

	MoveWindow(hRBtnCRC8, 110, HIWORD(lParam) - 60, 60, 20, TRUE);
	MoveWindow(hRBtnCRC16, 180, HIWORD(lParam) - 60, 70, 20, TRUE);
	MoveWindow(hRBtnCRC32, 260, HIWORD(lParam) - 60, 70, 20, TRUE);
	MoveWindow(hRBtnUser, 340, HIWORD(lParam) - 60, 100, 20, TRUE);
	MoveWindow(hComboCRC, 450, HIWORD(lParam) - 60, 160, 100, TRUE);
	MoveWindow(hEditCksum, 620, HIWORD(lParam) - 60, 80, 20, TRUE);
	MoveWindow(hBtnCRCmake, 705, HIWORD(lParam) - 60, 40, 20, TRUE);
	return 0;
}

INT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	TextOut(hdc, rtTime.left, rtTime.top, sTime, lstrlen(sTime));
	EndPaint(hWnd, &ps);
	return 0;
}

DWORD WINAPI ThreadSerialFunc(LPVOID value)
{
	TCHAR retMes[MAX_PATH];
	INT recvCheck;
	unsigned char *ptr = (unsigned char*) value;
	while ((*ptr != 0 && serialCommHandler.SerialPort[*ptr]->IsConnected())) {		
		recvCheck = serialCommHandler.SerialPort[*ptr]->CountRx(retMes);		
		if (recvCheck > 0) {
			Sleep(255);
			dwBytesRead = serialCommHandler.SerialPort[*ptr]->Receive(rxBuff, sizeof(rxBuff), retMes);
			SendMessage(hWndMain, WM_USER_THREAD_OUTPUT, MY_THREAD_RX, 0);
			dwBytesRead = 0;
		}
		else if (recvCheck == 0) Sleep(200);
		else break; // 통신에 오류 발생, 주로 연결이 끊어진 경우
		//serialCommHandler.SerialPort[*ptr]->Purge(retMes);		
	}
	serialCommHandler.ReflashConnectedPort();
	portNum = 0;
	SetWindowText(hBtnTerminal, TEXT("OpenPort"));
	SetWindowText(hStatic1, strPort);
	SetWindowText(hStatic2, strBR);
	SetWindowText(hStatic3, strDB);
	SetWindowText(hStatic4, strParity);
	SetWindowText(hStatic5, strStop);
	SetWindowText(hStatic6, strFlow);
	SetWindowText(hStatic7, strTimeOut);
	return 0;
}

DWORD WINAPI ThreadNetworkFunc(LPVOID value)
{
	TCHAR str[MAX_PATH];
	WORD port;
	INT *ptr = (INT*)value;
	INT recvCheck;
	while(flagConnectedType== IDTCPCNT || 
		flagConnectedType==IDTCPSERVER ||
		flagConnectedType==IDUDPSERVER ||
		flagConnectedType == IDUDPCNT) {
		Sleep(250);
		if (flagConnectedType == IDTCPCNT) {
			while (myTcpClient->IsConnected() == FALSE && flagConnectedType!=0) {
				SetWindowText(hStatic2, TEXT("Waiting connection"));
				myTcpClient->Connect();
				Sleep(250);
			}
			SetWindowText(hStatic2, TEXT("Connected"));
			recvCheck = myTcpClient->CountRx();
			if (recvCheck > 0) {
				dwBytesRead = myTcpClient->Recv(rxBuff, sizeof(rxBuff));
				SendMessage(hWndMain, WM_USER_THREAD_OUTPUT, MY_THREAD_RX, 0);
				dwBytesRead = 0;
			}
			else if (recvCheck == 0) Sleep(100);
			else if (recvCheck < 0) break;
		}
		else if (flagConnectedType == IDTCPSERVER) {
			while (myTcpServer->IsConnected() == FALSE && flagConnectedType != 0) {
				SetWindowText(hStatic2, TEXT("Waiting connection"));
				SetWindowText(hStatic8, "");
				myTcpServer->Connect();
				Sleep(250);
			}
			SetWindowText(hStatic2, TEXT("Connected"));
			IPaddrLongToStr(str, myTcpServer->GetRemoteIP(&port), MY_ADDR_HOST);
			SetWindowText(hStatic8, str);			
			recvCheck = myTcpServer->CountRx();
			if (recvCheck > 0) {
				dwBytesRead = myTcpServer->Recv(rxBuff, sizeof(rxBuff));
				SendMessage(hWndMain, WM_USER_THREAD_OUTPUT, MY_THREAD_RX, 0);
				dwBytesRead = 0;
			}
			else if (recvCheck == 0) Sleep(100);
			else if (recvCheck < 0) break;
		}
		else if (flagConnectedType == IDUDPSERVER) {
			while (myUdpServer->IsConnected() == FALSE && flagConnectedType != 0) {
				SetWindowText(hStatic2, TEXT("Waiting connection"));
				myUdpServer->Connect();
				Sleep(250);
			}
			SetWindowText(hStatic2, TEXT("Connected"));
			//IPaddrLongToStr(str, myTcpServer->GetRemoteIP(&port), MY_ADDR_HOST);
			//SetWindowText(hStatic8, str);			
			recvCheck = myUdpServer->CountRx();
			if (recvCheck > 0) {
				dwBytesRead = myUdpServer->RecvFrom(rxBuff, sizeof(rxBuff));
				SendMessage(hWndMain, WM_USER_THREAD_OUTPUT, MY_THREAD_RX, 0);
				dwBytesRead = 0;
			}
			else if (recvCheck == 0) Sleep(100);
			else if (recvCheck < 0) break;
		}
		else if (flagConnectedType == IDUDPCNT) {
			while (myUdpClient->IsConnected() == FALSE && flagConnectedType != 0) {
				SetWindowText(hStatic2, TEXT("Waiting connection"));
				myUdpClient->ConnectUnconnected();
				Sleep(250);
			}
			SetWindowText(hStatic2, TEXT("Connected"));			
			recvCheck = myUdpClient->CountRx();
			if (recvCheck > 0) {
				dwBytesRead = myUdpClient->RecvFrom(rxBuff, sizeof(rxBuff));
				SendMessage(hWndMain, WM_USER_THREAD_OUTPUT, MY_THREAD_RX, 0);
				dwBytesRead = 0;
			}
			else if (recvCheck == 0) Sleep(100);
			else if (recvCheck < 0) break;
		}
	}	
	SetWindowText(hBtnTerminal, TEXT("Open"));
	SetWindowText(hStatic1, strNull);
	SetWindowText(hStatic2, strNull);
	SetWindowText(hStatic3, strNull);
	SetWindowText(hStatic4, strNull);
	SetWindowText(hStatic5, strNull);
	SetWindowText(hStatic6, strNull);
	SetWindowText(hStatic7, strNull);
	SetWindowText(hStatic8, strNull);
	flagConnectedType = 0;
	if (myTcpClient != NULL) delete(myTcpClient);
	if (myTcpServer != NULL) delete(myTcpServer);
	if (myUdpServer != NULL) delete(myUdpServer);
	if (myUdpClient != NULL) delete(myUdpClient);
	myTcpClient = NULL;
	myTcpServer = NULL;
	myUdpServer = NULL;
	myUdpClient = NULL;
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
