#define WIN32_LEAN_AND_MEAN
#include "MyTcp.h"
#include <WS2tcpip.h>
#include <assert.h>
#include <mstcpip.h>
#include <CommCtrl.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

//void test()
//{
//	ADDRINFO addrInfo;
//	ADDRINFO *pAddrInfo;
//	SOCKADDR_IN sockAddr;
//	SOCKADDR_IN *pSockaddr;
//	CHAR szAddress[512];
//	CHAR str[MAX_PATH];
//	WSADATA wsaData;
//	CHAR szBuffer1[512], szBuffer2[512];
//	WSAStartup(MAKEWORD(2, 2), &wsaData);
//	// 도메인을 IP 주소로 변환. 접속 조건인 addrInfo 구조체, IP주소가 반환될 addrInfo *포인터가 사용
//	SecureZeroMemory(&addrInfo, sizeof(addrInfo));
//	addrInfo.ai_family = AF_UNSPEC;
//	addrInfo.ai_socktype = SOCK_STREAM;
//	addrInfo.ai_protocol = IPPROTO_TCP;
//
//	getaddrinfo("www.naver.com", "0", &addrInfo, &pAddrInfo);
//	pSockaddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
//	wsprintf(str, "IP address is %s\r\n", inet_ntoa(pSockaddr->sin_addr));
//	freeaddrinfo(pAddrInfo);
//	// IP 주소를 도메인으로 변환. IP 주소를 담고 있는 sockaddr_in* 구조체, 도메인이 반환될 char*배열,
//	// 서비스가 반환될 char* 배열이 사용됨.
//	strcpy(szAddress, "173.194.127.53");
//	SecureZeroMemory(&sockAddr, sizeof(sockAddr));
//	sockAddr.sin_family = AF_INET;
//	sockAddr.sin_addr.S_un.S_addr = inet_addr(szAddress);
//	sockAddr.sin_port = 80;
//
//	SecureZeroMemory(szBuffer1, sizeof(szBuffer1));
//	SecureZeroMemory(szBuffer2, sizeof(szBuffer2));
//	getnameinfo((const SOCKADDR*)&sockAddr, sizeof(sockAddr), szBuffer1, sizeof(szBuffer1), szBuffer2, sizeof(szBuffer2), NI_NUMERICSERV);
//	wsprintf(str, "Node Name is %s Service is %s", szBuffer1, szBuffer2);
//
//	WSACleanup();
//}

static BOOL SocketReady(INT sockfd, INT timeout)
{
	struct timeval to = { 0, timeout * 1000 };
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);

	INT ready = select(1, NULL, &fds, NULL, &to);	// ready to send
													// int ready = select(1, &fds, NULL, NULL, &to); // ready to receive
	return (0 < ready);
}

INT MyTcpServer::SocketCountRx(INT sockfd)
{
	//  입력 버퍼에 들어있는 케릭터 수를 카운트한다.
	DWORD count = 0;
	if (ioctlsocket(sockfd, FIONREAD, &count) == SOCKET_ERROR) {	// handle error
		return -1;
	}
	else {
		return count;
	}
}

BOOL MyTcpServer::Open()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2); // 버전 정보 2.2
	BOOL optReuseaddr = TRUE;
	struct sockaddr_in serverAddr;
	INT err;
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: MyTcpServer::Open (): already opened socket \r\n");
		//printf(_msg);
		this->Close();
	}
	// 버전 정보를 입력하고 WSADATA 구조체 값을 얻는다.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	// 소켓의 생성 : 성공시 파일 디스크립터(핸들), 실패 시 -1 반환
	// IPv4 인터넷 프로토콜, 연결지향형, 최종 프로토콜 형태 구체화(0 또는 IPPROTO_TCP)
	this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockfd == INVALID_SOCKET) {
		wsprintf(this->_msg, "ERROR: socket() \r\n");
		goto CLOSE_RET;
	}
	// 소켓에서 사용하던 Port No를 재사용 가능하도록 한다.
	// listen하던 소켓이 의도치않게 닫힌 후 새로 연 경우 한동안 bind ()가 안되는 현상을 막기위해 설정한다.	
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuseaddr, sizeof(optReuseaddr) == SOCKET_ERROR)) {
		wsprintf(this->_msg, "ERROR: setsockopt(opt_reuseaddr) \r\n");
		goto CLOSE_RET;
	}
	// 주소정보 초기화
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	// host to network long : 호스트 데이터 바이트 순서를 네트워크(빅 엔디언)에 맞게 변경
	// INADDR_ANY 0.0.0.0, INADDR_BROADCAST 255.255.255.255, INADDR_LOOPBACK 127.0.0.1	
	serverAddr.sin_addr.S_un.S_addr = htonl(this->_localIp);
	// host to network short
	serverAddr.sin_port = htons(this->_port);

	// 서버의 IP주소 할당 
	if (bind(this->_sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: bind() \r\n");
		goto CLOSE_RET;
	}

	// 연결요청대기상태로 진입, 두번째 인수는 클라이언트 연결요청수락 가능갯수이다.
	if (listen(this->_sockfd, 1) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: listen() \r\n");
		goto CLOSE_RET;
	}
	this->_connected = FALSE;
	return TRUE;

CLOSE_RET:
	if (this->_sockfd != INVALID_SOCKET) {
		closesocket(this->_sockfd);
	}
	this->_sockfd = INVALID_SOCKET;
	WSACleanup(); // 윈도우  소켓 정보를 초기화한다.
	return FALSE;	
}

BOOL MyTcpServer::Close()
{
	wsprintf(_msg, "INFO: MyTcpServer::Close (): %d.%d.%d.%d, %d \r\n",
		FIRST_IPADDRESS(this->_remoteIp), SECOND_IPADDRESS(this->_remoteIp), THIRD_IPADDRESS(this->_remoteIp), FOURTH_IPADDRESS(this->_remoteIp),
		(INT)this->_port);
	//printf(_msg);
	if (this->_sockfd != INVALID_SOCKET) {
		shutdown(this->_sockfd, SD_BOTH); //스트림 선택적 종료
		closesocket(this->_sockfd);
		this->_connected = FALSE;
		WSACleanup(); // 윈도우  소켓 정보를 초기화한다.
		return TRUE;
	}
	this->_connected = FALSE;
	return FALSE;
}

BOOL MyTcpServer::Connect()
{
	struct sockaddr_in clientAddr;
	INT cliLen = sizeof(clientAddr);
	struct timeval to = { 0, this->_timeout * 1000 };	
	INT connfd;
	INT error;
	DWORD dwRet = 0;
	struct tcp_keepalive ka;
	INT optval;

	if (this->_sockfd == INVALID_SOCKET) this->Open();
	if (this->_connected) return TRUE;

	// Set the socket to non-blocking mode.
	INT nonblocking = 1;
	if (ioctlsocket(this->_sockfd, FIONBIO, (ULONG*)&nonblocking) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: ioctlsocket(FIONBIO) \r\n");
		goto CLOSE_RET;
	}

	// 실제적으로 클라이언트와 연결될 소켓을 만들고 연결되면 클라이언트 정보를 얻는다.
	connfd = accept(this->_sockfd, (struct sockaddr*)&clientAddr, &cliLen);	
	if (connfd == INVALID_SOCKET) {
		error = WSAGetLastError();
		if ((error == WSAETIMEDOUT) || (error == WSAEWOULDBLOCK)) {
			// 아직 연결하지 못함
			// socket이 사용가능할 때까지 기다린다.
			SocketReady(this->_sockfd, this->_timeout);
			return FALSE;
		}
		else {
			wsprintf(this->_msg, "ERROR: accept() \r\n");
			goto CLOSE_RET;
		}
	}
	else{
		// 연결된 클라이언트 IP주소와 포트번호를 객체 정보에 저장한다.
		this->_remoteIp = ntohl(clientAddr.sin_addr.S_un.S_addr);
		this->_port = ntohs(clientAddr.sin_port);		
		closesocket(this->_sockfd);
		this->_sockfd = connfd;
		ka.onoff = 1;
		ka.keepalivetime = 1000;
		ka.keepaliveinterval = 1000;
		if (WSAIoctl(this->_sockfd, SIO_KEEPALIVE_VALS, &ka, sizeof(ka), NULL, 0, &dwRet, NULL, NULL) == SOCKET_ERROR) {
			wsprintf(this->_msg, "ERROR: WSAIoctl(SIO_KEEPALIVE_VALS) \r\n");
			goto CLOSE_RET;
		}
		// 연결되면 blocking 모드로 설정하고 send/recv 시에 timeout 값을 설정하여 무한히 대기하는것을 방지한다.
		// Set the socket to blocking mode
		nonblocking = 0;
		if (ioctlsocket(this->_sockfd, FIONBIO, (ULONG*)&nonblocking) == SOCKET_ERROR) {
			wsprintf(this->_msg, "ERROR: ioctlsocket(FIONBIO) \r\n");
			goto CLOSE_RET;
		}
		// Set the status for the keepalive option
		optval = 1;
		if (setsockopt(this->_sockfd, SOL_SOCKET, SO_KEEPALIVE, (TCHAR*)&optval, sizeof(optval)) == SOCKET_ERROR) {
			wsprintf(this->_msg, "ERROR: setsockopt(SO_KEEPALIVE) \r\n");
			goto CLOSE_RET;
		}
		// socket option (sending or receiving timeout value) setting
		if (setsockopt(this->_sockfd, SOL_SOCKET, SO_RCVTIMEO, (TCHAR*)&this->_timeout, sizeof(this->_timeout)) == SOCKET_ERROR) {
			wsprintf(this->_msg, "ERROR: setsockopt(SO_RCVTIMEO) \r\n");
			goto CLOSE_RET;
		}
		if (setsockopt(this->_sockfd, SOL_SOCKET, SO_SNDTIMEO, (TCHAR*)&this->_timeout, sizeof(this->_timeout)) == SOCKET_ERROR) {
			wsprintf(this->_msg, "ERROR: setsockopt(SO_SNDTIMEO) \r\n");
			goto CLOSE_RET;
		}
		this->_connected = TRUE;		
		wsprintf(this->_msg, "INFO: MyTcpServer::Connect(): Connect to TCP client: %d.%d.%d.%d, %d \r\n",
			FIRST_IPADDRESS(this->_remoteIp), SECOND_IPADDRESS(this->_remoteIp), THIRD_IPADDRESS(this->_remoteIp), FOURTH_IPADDRESS(this->_remoteIp),
			(INT)this->_port);
		printf(this->_msg);
		return TRUE;
	}	
CLOSE_RET:
	if (this->_sockfd != INVALID_SOCKET) {
		closesocket(_sockfd);
	}
	this->_sockfd = INVALID_SOCKET;
	return FALSE;
}

INT MyTcpServer::Send(const TCHAR *msgSrc, INT msgLength)
{
	if (this->_sockfd == INVALID_SOCKET) return -1;
	if (!this->_connected) return -1;
	INT ret = send(this->_sockfd, msgSrc, msgLength, 0);
	if (ret == 0) {
		this->Close();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;
		wsprintf(this->_msg, "ERROR: send() \r\n");
		this->Close();
		return -1;
	}
	return ret;
}

INT MyTcpServer::Recv(TCHAR *msgDest, INT msgLength)
{
	if (this->_sockfd == INVALID_SOCKET) return -1;
	if (!this->_connected) return -1;
	INT ret = recv(this->_sockfd, msgDest, msgLength, 0);
	if (ret == 0) {
		this->Close();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;
		wsprintf(this->_msg, "ERROR: recv() \r\n");
		this->Close();
		return -1;
	}
	return ret;
}

DWORD MyTcpServer::GetRemoteIP(WORD *port)
{
	struct sockaddr_in clientAddr;
	INT addrLength = sizeof(clientAddr);

	INT ret = getpeername(this->_sockfd, (struct sockaddr*)&clientAddr, &addrLength);
	if (ret != SOCKET_ERROR) {
		if (port) *port = clientAddr.sin_port;
		return ntohl(clientAddr.sin_addr.S_un.S_addr);
	}
	return 0;
}

/*

	클라이언트 함수 정의

*/

INT MyTcpClient::SocketCountRx(INT sockfd)
{
	//  입력 버퍼에 들어있는 케릭터 수를 카운트한다.
	DWORD count = 0;
	if (ioctlsocket(sockfd, FIONREAD, &count) == SOCKET_ERROR) {	// handle error
		return -1;
	}
	else {
		return count;
	}
}

BOOL MyTcpClient::Open()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2); // 버전 정보 2.2
	INT nonblocking;
	INT err;
	// 버전 정보를 입력하고 WSADATA 구조체 값을 얻는다.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: MyTcpClient::Open (): already opened socket \r\n");
		this->Close();
	}

	// 소켓의 생성 : 성공시 파일 디스크립터(핸들), 실패 시 -1 반환
	// IPv4 인터넷 프로토콜, 연결지향형, 최종 프로토콜 형태 구체화(0 또는 IPPROTO_TCP)
	this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockfd == INVALID_SOCKET) {
		wsprintf(this->_msg, "ERROR: socket() \r\n");
		goto CLOSE_RET;
	}
	// Set the socket to non-blocking mode.
	nonblocking = 1;
	if (ioctlsocket(this->_sockfd, FIONBIO, (ULONG*)&nonblocking) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: ioctlsocket(FIONBIO) \r\n");
		goto CLOSE_RET;
	}
	this->_connected = FALSE;
	return TRUE;
CLOSE_RET:
	if (this->_sockfd != INVALID_SOCKET) {
		closesocket(this->_sockfd);
	}
	this->_sockfd = INVALID_SOCKET;
	WSACleanup();
	return FALSE;
}

BOOL MyTcpClient::Close()
{
	wsprintf(_msg, "INFO: MyTcpClient::Close (): %d.%d.%d.%d, %d \r\n",
		FIRST_IPADDRESS(this->_remoteIp), SECOND_IPADDRESS(this->_remoteIp), THIRD_IPADDRESS(this->_remoteIp), FOURTH_IPADDRESS(this->_remoteIp),
		(INT)this->_port);
	//printf(_msg);
	if (this->_sockfd != INVALID_SOCKET) {
		shutdown(this->_sockfd, SD_BOTH); //스트림 선택적 종료
		closesocket(this->_sockfd);
		this->_connected = FALSE;
		WSACleanup(); // 윈도우  소켓 정보를 초기화한다.
		return TRUE;
	}
	this->_connected = FALSE;
	return FALSE;
}

BOOL MyTcpClient::Connect()
{
	if (this->_sockfd == INVALID_SOCKET) this->Open();
	if (this->_connected) return TRUE;
	// Open에서 nonblocking이 1로 설정되어있다.
	INT nonblocking = 1;
	INT ret;
	INT error;
	DWORD dwRet = 0;
	INT optval;
	struct tcp_keepalive ka;
	struct sockaddr_in serverAddr;	
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = htonl(this->_remoteIp);
	serverAddr.sin_port = htons(this->_port);

	// non-blocking 모드에서는 connect () 함수는 WSAEWOULDBLOCK 에러코드를 세팅하고 실패(-1)을 리턴한다.
	// connect 함수 호출 시 자동으로 소켓에 IP와 PORT가 할당된다. 따라서 bind()절차가 필요없다.
	ret = connect(this->_sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR) {
		error = WSAGetLastError();
		if (nonblocking && ((error == WSAEINVAL) || (error == WSAEWOULDBLOCK))) {
			// socket이 사용가능할 때까지 기다린다.
			if (SocketReady(this->_sockfd, this->_timeout)) {
				// Connected
			}
			else
			{
				return FALSE;
			}
		}
		else if (nonblocking && (error == WSAEALREADY || error == WSAEISCONN)) {// 이미 연결된 소켓
			// Connected
		}
		else {
			wsprintf(this->_msg, "ERROR: connect() \r\n");
			goto CLOSE_RET;
		}
	}
	else {
		// Connected
	}
	ka.onoff = 1;
	ka.keepalivetime = 1000;
	ka.keepaliveinterval = 1000;
	if (WSAIoctl(this->_sockfd, SIO_KEEPALIVE_VALS, &ka, sizeof(ka), NULL, 0, &dwRet, NULL, NULL) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: WSALoctl(SIO_KEEPALIVE_VALS) \r\n");
		goto CLOSE_RET;
	}
	// 연결되면 blocking 모드로 설정하고 send/recv 시에 timeout 값을 설정하여 무한히 대기하는것을 방지한다.
	nonblocking = 0;
	if (ioctlsocket(this->_sockfd, FIONBIO, (ULONG*)&nonblocking) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: ioctlsocket(FIONBIO) \r\n");
		goto CLOSE_RET;
	}
	// Set the status for the keepalie option
	optval = 1;
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_KEEPALIVE, (TCHAR*)&optval, sizeof(optval)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_KEEPALIVE) \r\n");
		goto CLOSE_RET;
	}
	// socket의 opetion (sending/receiving timeout 값) 설정
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_RCVTIMEO, (TCHAR*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_RCVTIMEO) \r\n");
		goto CLOSE_RET;
	}
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_SNDTIMEO, (TCHAR*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_SNDTIMEO) \r\n");
		goto CLOSE_RET;
	}
	this->_connected = TRUE;
	wsprintf(this->_msg, "INFO: MyTcpClient::Connect(): Connect to TCP server: %d.%d.%d.%d, %d \r\n",
		FIRST_IPADDRESS(this->_remoteIp), SECOND_IPADDRESS(this->_remoteIp), THIRD_IPADDRESS(this->_remoteIp), FOURTH_IPADDRESS(this->_remoteIp),
		(INT)this->_port);
	printf(this->_msg);
	return TRUE;

CLOSE_RET:
	if (this->_sockfd != INVALID_SOCKET) {
		closesocket(this->_sockfd);
	}
	this->_sockfd = INVALID_SOCKET;
	return FALSE;
}

INT MyTcpClient::Send(const TCHAR *msgSrc, INT msgLength)
{
	if (this->_sockfd == INVALID_SOCKET) return -1;
	if (!this->_connected) return -1;
	INT ret = send(this->_sockfd, msgSrc, msgLength, 0);
	if (ret == 0) {
		this->Close();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;
		wsprintf(this->_msg, "ERROR: send() \r\n");
		this->Close();
		return -1;
	}
	return ret;
}

INT MyTcpClient::Recv(TCHAR *msgDest, INT msgLength)
{
	if (this->_sockfd == INVALID_SOCKET) return -1;
	if (!this->_connected) return -1;
	INT ret = recv(this->_sockfd, msgDest, msgLength, 0);
	if (ret == 0) {
		this->Close();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;
		wsprintf(this->_msg, "ERROR: recv() \r\n");
		this->Close();
		return -1;
	}
	return ret;
}

DWORD MyTcpClient::GetRemoteIP(WORD *port)
{
	struct sockaddr_in clientAddr;
	INT addrLength = sizeof(clientAddr);

	INT ret = getpeername(this->_sockfd, (struct sockaddr*)&clientAddr, &addrLength);
	if (ret != SOCKET_ERROR) {
		if (port) *port = clientAddr.sin_port;
		return ntohl(clientAddr.sin_addr.S_un.S_addr);
	}
	return 0;
}