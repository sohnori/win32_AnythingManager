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
//	// �������� IP �ּҷ� ��ȯ. ���� ������ addrInfo ����ü, IP�ּҰ� ��ȯ�� addrInfo *�����Ͱ� ���
//	SecureZeroMemory(&addrInfo, sizeof(addrInfo));
//	addrInfo.ai_family = AF_UNSPEC;
//	addrInfo.ai_socktype = SOCK_STREAM;
//	addrInfo.ai_protocol = IPPROTO_TCP;
//
//	getaddrinfo("www.naver.com", "0", &addrInfo, &pAddrInfo);
//	pSockaddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
//	wsprintf(str, "IP address is %s\r\n", inet_ntoa(pSockaddr->sin_addr));
//	freeaddrinfo(pAddrInfo);
//	// IP �ּҸ� ���������� ��ȯ. IP �ּҸ� ��� �ִ� sockaddr_in* ����ü, �������� ��ȯ�� char*�迭,
//	// ���񽺰� ��ȯ�� char* �迭�� ����.
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
	//  �Է� ���ۿ� ����ִ� �ɸ��� ���� ī��Ʈ�Ѵ�.
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
	WORD wVersionRequested = MAKEWORD(2, 2); // ���� ���� 2.2
	BOOL optReuseaddr = TRUE;
	struct sockaddr_in serverAddr;
	INT err;
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: MyTcpServer::Open (): already opened socket \r\n");
		//printf(_msg);
		this->Close();
	}
	// ���� ������ �Է��ϰ� WSADATA ����ü ���� ��´�.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	// ������ ���� : ������ ���� ��ũ����(�ڵ�), ���� �� -1 ��ȯ
	// IPv4 ���ͳ� ��������, ����������, ���� �������� ���� ��üȭ(0 �Ǵ� IPPROTO_TCP)
	this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockfd == INVALID_SOCKET) {
		wsprintf(this->_msg, "ERROR: socket() \r\n");
		goto CLOSE_RET;
	}
	// ���Ͽ��� ����ϴ� Port No�� ���� �����ϵ��� �Ѵ�.
	// listen�ϴ� ������ �ǵ�ġ�ʰ� ���� �� ���� �� ��� �ѵ��� bind ()�� �ȵǴ� ������ �������� �����Ѵ�.	
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuseaddr, sizeof(optReuseaddr) == SOCKET_ERROR)) {
		wsprintf(this->_msg, "ERROR: setsockopt(opt_reuseaddr) \r\n");
		goto CLOSE_RET;
	}
	// �ּ����� �ʱ�ȭ
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	// host to network long : ȣ��Ʈ ������ ����Ʈ ������ ��Ʈ��ũ(�� �����)�� �°� ����
	// INADDR_ANY 0.0.0.0, INADDR_BROADCAST 255.255.255.255, INADDR_LOOPBACK 127.0.0.1	
	serverAddr.sin_addr.S_un.S_addr = htonl(this->_localIp);
	// host to network short
	serverAddr.sin_port = htons(this->_port);

	// ������ IP�ּ� �Ҵ� 
	if (bind(this->_sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: bind() \r\n");
		goto CLOSE_RET;
	}

	// �����û�����·� ����, �ι�° �μ��� Ŭ���̾�Ʈ �����û���� ���ɰ����̴�.
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
	WSACleanup(); // ������  ���� ������ �ʱ�ȭ�Ѵ�.
	return FALSE;	
}

BOOL MyTcpServer::Close()
{
	wsprintf(_msg, "INFO: MyTcpServer::Close (): %d.%d.%d.%d, %d \r\n",
		FIRST_IPADDRESS(this->_remoteIp), SECOND_IPADDRESS(this->_remoteIp), THIRD_IPADDRESS(this->_remoteIp), FOURTH_IPADDRESS(this->_remoteIp),
		(INT)this->_port);
	//printf(_msg);
	if (this->_sockfd != INVALID_SOCKET) {
		shutdown(this->_sockfd, SD_BOTH); //��Ʈ�� ������ ����
		closesocket(this->_sockfd);
		this->_connected = FALSE;
		WSACleanup(); // ������  ���� ������ �ʱ�ȭ�Ѵ�.
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

	// ���������� Ŭ���̾�Ʈ�� ����� ������ ����� ����Ǹ� Ŭ���̾�Ʈ ������ ��´�.
	connfd = accept(this->_sockfd, (struct sockaddr*)&clientAddr, &cliLen);	
	if (connfd == INVALID_SOCKET) {
		error = WSAGetLastError();
		if ((error == WSAETIMEDOUT) || (error == WSAEWOULDBLOCK)) {
			// ���� �������� ����
			// socket�� ��밡���� ������ ��ٸ���.
			SocketReady(this->_sockfd, this->_timeout);
			return FALSE;
		}
		else {
			wsprintf(this->_msg, "ERROR: accept() \r\n");
			goto CLOSE_RET;
		}
	}
	else{
		// ����� Ŭ���̾�Ʈ IP�ּҿ� ��Ʈ��ȣ�� ��ü ������ �����Ѵ�.
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
		// ����Ǹ� blocking ���� �����ϰ� send/recv �ÿ� timeout ���� �����Ͽ� ������ ����ϴ°��� �����Ѵ�.
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

	Ŭ���̾�Ʈ �Լ� ����

*/

INT MyTcpClient::SocketCountRx(INT sockfd)
{
	//  �Է� ���ۿ� ����ִ� �ɸ��� ���� ī��Ʈ�Ѵ�.
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
	WORD wVersionRequested = MAKEWORD(2, 2); // ���� ���� 2.2
	INT nonblocking;
	INT err;
	// ���� ������ �Է��ϰ� WSADATA ����ü ���� ��´�.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: MyTcpClient::Open (): already opened socket \r\n");
		this->Close();
	}

	// ������ ���� : ������ ���� ��ũ����(�ڵ�), ���� �� -1 ��ȯ
	// IPv4 ���ͳ� ��������, ����������, ���� �������� ���� ��üȭ(0 �Ǵ� IPPROTO_TCP)
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
		shutdown(this->_sockfd, SD_BOTH); //��Ʈ�� ������ ����
		closesocket(this->_sockfd);
		this->_connected = FALSE;
		WSACleanup(); // ������  ���� ������ �ʱ�ȭ�Ѵ�.
		return TRUE;
	}
	this->_connected = FALSE;
	return FALSE;
}

BOOL MyTcpClient::Connect()
{
	if (this->_sockfd == INVALID_SOCKET) this->Open();
	if (this->_connected) return TRUE;
	// Open���� nonblocking�� 1�� �����Ǿ��ִ�.
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

	// non-blocking ��忡���� connect () �Լ��� WSAEWOULDBLOCK �����ڵ带 �����ϰ� ����(-1)�� �����Ѵ�.
	// connect �Լ� ȣ�� �� �ڵ����� ���Ͽ� IP�� PORT�� �Ҵ�ȴ�. ���� bind()������ �ʿ����.
	ret = connect(this->_sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret == SOCKET_ERROR) {
		error = WSAGetLastError();
		if (nonblocking && ((error == WSAEINVAL) || (error == WSAEWOULDBLOCK))) {
			// socket�� ��밡���� ������ ��ٸ���.
			if (SocketReady(this->_sockfd, this->_timeout)) {
				// Connected
			}
			else
			{
				return FALSE;
			}
		}
		else if (nonblocking && (error == WSAEALREADY || error == WSAEISCONN)) {// �̹� ����� ����
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
	// ����Ǹ� blocking ���� �����ϰ� send/recv �ÿ� timeout ���� �����Ͽ� ������ ����ϴ°��� �����Ѵ�.
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
	// socket�� opetion (sending/receiving timeout ��) ����
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