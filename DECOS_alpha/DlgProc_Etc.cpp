// 외부 MFC 오버헤더를 포함하지 않는 지시자_SDK를 사용
#define WIN32_LEAN_AND_MEAN 

#include "DlgProc_Etc.h"
//#include <WinSock2.h>
//#include <WS2tcpip.h>
#include <CommCtrl.h>
#include "MyTcp.h"
#include "resource.h"

#pragma comment(lib, "ws2_32.lib") 

//#define BUFF_SIZE	1024

static HWND hDlgCurrent;
extern HWND hCRT;
BOOL flagIsConnectedCLNT = 0;
BOOL flagIsConnectedSV;

MyTcpServer *TcpSV;
MyTcpClient *TcpCLNT;

// 스레드 핸들
//static HANDLE hEtcSVThread, hEtcCLNTThread;

//DWORD WINAPI EtcSVThreadFunc(LPVOID prc);
//DWORD WINAPI EtcCLNTThreadFunc(LPVOID prc);

BOOL CALLBACK EtcDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	INT index;
	INT strLen;
	TCHAR str[1024];
	switch (iMessage) {
	case WM_INITDIALOG:
		//DWORD ThreadID;
		hDlgCurrent = hDlg;
		SendDlgItemMessage(hDlgCurrent, IDC_IPADDRESS_CLIENT, IPM_SETADDRESS, 0, (LPARAM)MAKEIPADDRESS(127,0,0,1));
		SetDlgItemInt(hDlg, IDC_EDIT_ETC_SPORTN, 5000, TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_ETC_CPORTN, 5000, TRUE);
		/*hEtcSVThread = CreateThread(NULL, 0, EtcSVThreadFunc, &serverInfo, 0, &ThreadID);
		CloseHandle(hEtcSVThread);
		hEtcCLNTThread = CreateThread(NULL, 0, EtcCLNTThreadFunc, 0, 0, &ThreadID);
		CloseHandle(hEtcCLNTThread);*/
		//TcpSV = new MyTcpServer(MAKEIPADDRESS(127, 0, 0, 1), 5000, 1000);
		TcpCLNT = new MyTcpClient(MAKEIPADDRESS(127, 0, 0, 1), 5000, 1000);
		return TRUE;
	case WM_MOUSEWHEEL:
		index = (INT)wParam;
		if (index > 0) {
			if (LOWORD(wParam) && MK_SHIFT) SendMessage(hCRT, WM_HSCROLL, (WPARAM)LOWORD(SB_LINELEFT), 0);
			else SendMessage(hCRT, WM_VSCROLL, (WPARAM)LOWORD(SB_LINEUP), 0);
		}
		else if (index < 0) {
			if (LOWORD(wParam) && MK_SHIFT) SendMessage(hCRT, WM_HSCROLL, (WPARAM)LOWORD(SB_LINERIGHT), 0);
			else SendMessage(hCRT, WM_VSCROLL, (WPARAM)LOWORD(SB_LINEDOWN), 0);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_ETC_SCONN:

			/*if (serverInfo.flagConnected == FALSE && OpenSever(&serverInfo) == 1) {
				serverInfo.flagConnected = TRUE;
				SetWindowText(GetDlgItem(hDlgCurrent, IDC_BUTTON_ETC_SCONN), "Connected");
			}
			else {
				CloseSever(&serverInfo);
				serverInfo.flagConnected = FALSE;
				SetWindowText(GetDlgItem(hDlgCurrent, IDC_BUTTON_ETC_SCONN), "Opened");
			}*/
			return TRUE;
		case IDC_BUTTON_ETC_SSEND:
			//if (serverInfo.flagConnected == FALSE) return TRUE;
			return TRUE;
		case IDC_BUTTON_ETC_CCONN:
			if (flagIsConnectedCLNT == FALSE && TcpCLNT->Connect()) {
				SetWindowText(GetDlgItem(hDlgCurrent, IDC_BUTTON_ETC_CCONN), "Connected");
				
			}
			else {
				SetWindowText(GetDlgItem(hDlgCurrent, IDC_BUTTON_ETC_CCONN), "Opened");
				if (TcpCLNT->IsConnected()) TcpCLNT->Close();
			}
			strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
			SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
			SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)TcpCLNT->GetErrorMSG());
			return TRUE;
		case IDC_BUTTON_ETC_CSEND:
			if (TcpCLNT->IsConnected() != TRUE) return TRUE;
			strLen = GetDlgItemText(hDlg, IDC_EDIT_ETC_CSEND, str, sizeof(str));
			if (TcpCLNT->Send(str, strLen)<=0) {
				strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)TcpCLNT->GetErrorMSG());
			}
			return TRUE;
			//if (clientInfo.flagConnected == FALSE) break;
			//GetDlgItemText(hDlg, IDC_EDIT_ETC_CSEND, str, sizeof(str));
			//send(clientInfo.hSocket, str, strlen(str), 0);
			//strLen = recv(clientInfo.hSocket, str, BUFF_SIZE - 1, 0);
			//str[strLen] = 0;
			////wsprintf(msgErr, "Message from server : %s \r\n", str);
			//strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
			//SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
			//SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
			return TRUE;
		case IDC_BUTTON_ETC_CRECV:
			if (TcpCLNT->IsConnected() != TRUE) return TRUE;
			strLen = TcpCLNT->Recv(str, 20);
			if (strLen > 0) {
				strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)str);
			}
			else {
				strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)TcpCLNT->GetErrorMSG());
			}
			return TRUE;
		}	
		break;
	case WM_DESTROY:
		delete(TcpCLNT);
		break;
	}
	return FALSE;
}

//INT ConnectClient(struct tagClientInfo *Info)
//{
//	WSADATA wsaData;
//	SOCKADDR_IN svAddr;
//	INT strLen;
//	WORD wVersionRequested = MAKEWORD(2, 2); // 버전 정보 2.2
//	DWORD ipAddr;
//	INT err;
//	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
//		wsprintf((LPSTR)msgErr, "WSAStartup failed with error : %d \r\n", err);		
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		return 0;
//	}
//	Info->hSocket = socket(PF_INET, SOCK_STREAM, 0);
//	if (Info->hSocket == INVALID_SOCKET) {
//		wsprintf((LPSTR)msgErr, "socket() failed with error : %d \r\n", err);
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		WSACleanup();
//		return 0;
//	}
//	memset(&svAddr, 0, sizeof(svAddr));
//	svAddr.sin_family = AF_INET;
//	SendDlgItemMessage(hDlgCurrent, IDC_IPADDRESS_CLIENT, IPM_GETADDRESS, 0, (LPARAM)&ipAddr);	
//	svAddr.sin_addr.S_un.S_addr = htonl(ipAddr);
//	svAddr.sin_port = htons(GetDlgItemInt(hDlgCurrent, IDC_EDIT_ETC_CPORTN, NULL, TRUE));
//	if (connect(Info->hSocket, (SOCKADDR*)&svAddr, sizeof(svAddr)) == SOCKET_ERROR) {
//		wsprintf((LPSTR)msgErr, "connect() failed with error : %d \r\n", err);
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		closesocket(Info->hSocket);
//		WSACleanup();
//		return 0;
//	}	
//	wsprintf((LPSTR)msgErr, "Connected.......... \r\n");
//	strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//	SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//	SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//	ULONG nonblocking = 0;
//	if (ioctlsocket(Info->hSocket, FIONBIO, &nonblocking) == SOCKET_ERROR) {
//		wsprintf((LPSTR)msgErr, "ioctlsocket() failed with error \r\n");
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		closesocket(Info->hSocket);
//		WSACleanup();
//		return 0;
//	}
//	INT optval = 1; // TRUE
//	INT timeout = 1000;
//	if (setsockopt(Info->hSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, sizeof(optval)) == SOCKET_ERROR ||
//		setsockopt(Info->hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout))==SOCKET_ERROR ||
//		setsockopt(Info->hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout))) {
//		wsprintf((LPSTR)msgErr, "setsockopt() failed with error \r\n");
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		closesocket(Info->hSocket);
//		WSACleanup();
//		return 0;
//	}	
//	return 1;
//}
//
//INT DisconnectClient(struct tagClientInfo *Info)
//{
//	INT strLen;
//	closesocket(Info->hSocket);
//	WSACleanup();
//	wsprintf((LPSTR)msgErr, "Disconnected.......... \r\n");
//	strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//	SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//	SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//	return 1;
//}
//
//INT OpenSever(struct tagServerInfo *Info)
//{
//	WSADATA wsaData;	
//	INT strLen;
//	SOCKADDR_IN svAddr;
//	WORD wVersionRequested = MAKEWORD(2, 2); // 버전 정보 2.2	
//	INT err;
//	
//	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
//		wsprintf((LPSTR)msgErr, "WSAStartup failed with error : %d \r\n", err);
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		return 0;
//	}
//	Info->hSVsocket = socket(PF_INET, SOCK_STREAM, 0);
//	if (Info->hSVsocket == INVALID_SOCKET) {
//		wsprintf((LPSTR)msgErr, "socket() failed with error : %d \r\n", err);
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		WSACleanup();
//		return 0;
//	}
//	memset(&svAddr, 0, sizeof(svAddr));
//	svAddr.sin_family = AF_INET;
//	svAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//	svAddr.sin_port = htons(GetDlgItemInt(hDlgCurrent, IDC_EDIT_ETC_SPORTN, NULL, TRUE));
//	if (bind(Info->hSVsocket, (SOCKADDR*)&svAddr, sizeof(svAddr)) == SOCKET_ERROR) {
//		wsprintf((LPSTR)msgErr, "bind() failed with error : %d \r\n", err);
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		closesocket(Info->hSVsocket);
//		WSACleanup();
//		return 0;
//	}
//	ULONG nonblocking = 0;
//	if (ioctlsocket(Info->hSVsocket, FIONBIO, &nonblocking) == SOCKET_ERROR) {
//		wsprintf((LPSTR)msgErr, "ioctlsocket() failed with error \r\n");
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		closesocket(Info->hSVsocket);
//		WSACleanup();
//		return 0;
//	}
//	INT optval = 1;
//	if (setsockopt(Info->hSVsocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, sizeof(optval)) == SOCKET_ERROR) {
//		wsprintf((LPSTR)msgErr, "setsockopt() failed with error \r\n");
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_CRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		closesocket(Info->hSVsocket);
//		WSACleanup();
//		return 0;
//	}
//
//	if (listen(Info->hSVsocket, 1) == SOCKET_ERROR) {
//		wsprintf((LPSTR)msgErr, "listen() failed with error : %d \r\n", err);
//		strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//		SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//		closesocket(Info->hSVsocket);
//		WSACleanup();
//		return 0;
//	}
//	wsprintf((LPSTR)msgErr, "Connected.......... \r\n", err);
//	strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//	SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//	SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//	return  1;
//}
//
//INT CloseSever(struct tagServerInfo *Info)
//{
//	INT strLen;
//	closesocket(Info->hClntSocket);
//	closesocket(Info->hSVsocket);
//	wsprintf((LPSTR)msgErr, "Disconnected.......... \r\n");
//	strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//	SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//	SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//	WSACleanup();
//	return 1;
//}

//DWORD WINAPI EtcSVThreadFunc(LPVOID prc)
//{	
//	INT strLen;	
//	TCHAR buffOutput[1024];
//	SOCKADDR_IN  clntAddr;
//	INT clntAddrSize;	
//	struct tagServerInfo *Info = (struct tagServerInfo *)prc;
//	for (;;) {
//		//Info = (struct tagServerInfo *)prc;
//		if (serverInfo.flagConnected == TRUE) {
//			clntAddrSize = sizeof(clntAddr);
//			serverInfo.hClntSocket = accept(serverInfo.hSVsocket, (SOCKADDR*)&clntAddr, &clntAddrSize);
//			if (serverInfo.hClntSocket == -1) {
//				wsprintf((LPSTR)msgErr, "accept() failed with error \r\n");
//				strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);				
//			}
//			else {
//				wsprintf((LPSTR)msgErr, "Connected client \r\n");
//				strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//				SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msgErr);
//			}
//			while (serverInfo.flagConnected == TRUE) {
//				if ((strLen = recv(serverInfo.hClntSocket, msgErr, BUFF_SIZE, 0)) > 0) {
//					send(serverInfo.hClntSocket, msgErr, strLen, 0);
//					strcpy_s(buffOutput, TEXT("클라이언트로부터 받은 데이터 - "));
//					strncat_s(buffOutput, msgErr, strLen);
//					strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//					SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//					SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)buffOutput);
//				}
//			}
//			//return 1;
//		}
//	}
//	return 1;
//}
//
//DWORD WINAPI EtcCLNTThreadFunc(LPVOID prc)
//{
//	INT strLen;
//	TCHAR buffR[1024];
//	static BOOL flagClntConnected;
//	static SOCKET hSocket;
//
//	//static struct tagClientInfo Info;// = (struct tagClientInfo *)prc;
//	for (;;) {
//		//Info = (struct tagClientInfo *)prc;
//		//Info = GetClntInfo();
//		flagClntConnected = GetClntFlag();
//		hSocket = GetClnthSocket();
//		if (flagClntConnected == TRUE) {
//			while (flagClntConnected  == TRUE) {
//				if ((strLen = recv(hSocket, msgErr, 10, 0)) > 0) {
//					strcpy_s(buffR, TEXT("서버로부터 받은 데이터 - "));
//					strncat_s(buffR, msgErr, strLen);
//					strLen = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT));
//					SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_SETSEL, strLen, strLen);
//					SendMessage(GetDlgItem(hDlgCurrent, IDC_EDIT_ETC_SRESULT), EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)buffR);
//				}
//			}
//			//return 1;
//		}
//	}
//	return 1;
//}
