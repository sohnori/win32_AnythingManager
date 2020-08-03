#define WIN32_LEAN_AND_MEAN
#include "MyUdp.h"
#include <WS2tcpip.h>
#include <assert.h>
#include <mstcpip.h>
#include <CommCtrl.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

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

INT MyUdpServer::SocketCountRx(INT sockfd)
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

BOOL MyUdpServer::Open()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2); // ���� ���� 2.2
	BOOL optReuseaddr = TRUE;
	struct sockaddr_in serverAddr;
	INT err;
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: UdpServer::Open (): already opened socket \r\n");
		//printf(_msg);
		this->Close();
	}
	// ���� ������ �Է��ϰ� WSADATA ����ü ���� ��´�.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	// ������ ���� : ������ ���� ��ũ����(�ڵ�), ���� �� -1 ��ȯ
	// IPv4 ���ͳ� ��������, ����������, ���� �������� ���� ��üȭ(0 �Ǵ� IPPROTO_UDP)
	this->_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (this->_sockfd == INVALID_SOCKET) {
		wsprintf(this->_msg, "ERROR: socket() \r\n");
		goto CLOSE_RET;
	}
	if (this->_remoteIp == INADDR_BROADCAST) {
		// UDP socket�� broadcasting ���� �����Ѵ�.
		int opt = 1;
		if (setsockopt(_sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
			wsprintf(this->_msg, "ERROR: setsockopt(SO_BROADCAST):");
			goto CLOSE_RET;
		}
		//if (setsockopt (_sockfd, SOL_SOCKET, SO_DONTROUTE, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
		//	OnSocketError ("ERROR: setsockopt(SO_DONTROUTE):");
		//	goto CLOSE_RET;
		//}
	}
	// ���Ͽ��� ����ϴ� Port No�� ���� �����ϵ��� �Ѵ�.
	// listen�ϴ� ������ �ǵ�ġ�ʰ� ���� �� ���� �� ��� �ѵ��� bind ()�� �ȵǴ� ������ �������� �����Ѵ�.	
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuseaddr, sizeof(optReuseaddr) == SOCKET_ERROR)) {
		wsprintf(this->_msg, "ERROR: setsockopt(opt_reuseaddr) \r\n");
		goto CLOSE_RET;
	}
	// socket�� opetion (sending/receiving timeout ��) ����
	if (setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_RCVTIMEO):");
		goto CLOSE_RET;
	}
	if (setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_SNDTIMEO):");
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
	
	// UDP������ ���� �ڵ��� ������ ����� ��(_connected == true)���� �����Ѵ�.
	this->_connected = TRUE;
	return TRUE;

CLOSE_RET:
	if (this->_sockfd != INVALID_SOCKET) {
		closesocket(this->_sockfd);
	}
	this->_sockfd = INVALID_SOCKET;
	WSACleanup(); // ������  ���� ������ �ʱ�ȭ�Ѵ�.
	return FALSE;
}

BOOL MyUdpServer::Close()
{
	wsprintf(_msg, "INFO: MyUdp::Close (): %d.%d.%d.%d, %d \r\n",
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

BOOL MyUdpServer::Connect()
{
	INT index;
	_recvIpAddr = 0;
	_recvPort = 0;

	if (this->_sockfd == INVALID_SOCKET) index = Open();
	if (index = TRUE) wsprintf(this->_msg, "INFO: MyUdpServer::Connect(): \r\n");
	return (this->_sockfd != INVALID_SOCKET) && this->_connected;
}

INT MyUdpServer::SendTo(CONST TCHAR *msg, INT len)
{
	if (this->_sockfd == INVALID_SOCKET) return -1;
	if (!this->_connected) return -1;

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(_remoteIp);
	serverAddr.sin_port = htons(_port);

	INT ret = sendto(_sockfd, msg, len, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret == 0) {
		this->Close();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;

		wsprintf(this->_msg, "ERROR: sendto() \r\n");
		this->Close();
		return -1;
	}
	wsprintf(_msg, "INFO: MyUdpServer::SendTo (): %d.%d.%d.%d, %d \r\n",
		FIRST_IPADDRESS(this->_remoteIp), SECOND_IPADDRESS(this->_remoteIp),
		THIRD_IPADDRESS(this->_remoteIp), FOURTH_IPADDRESS(this->_remoteIp),
		this->_port);
	return ret;
}

INT MyUdpServer::RecvFrom(TCHAR *msgDest, INT msgLength)
{
	if (this->_sockfd == INVALID_SOCKET) return -1;
	if (!this->_connected) return -1;

	struct sockaddr_in clientAddr;
	INT clientAddrSize = sizeof(clientAddr);

	INT ret = recvfrom(_sockfd, msgDest, msgLength, 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
	if (ret == 0) {
		this->Close();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;

		wsprintf(this->_msg, "ERROR: recvfrom() \r\n");
		this->Close();
		return -1;
	}
	_recvIpAddr = ntohl(clientAddr.sin_addr.s_addr);
	_recvPort = ntohs(clientAddr.sin_port);
	wsprintf(_msg, "INFO: MyUdpServer::Recvfrom (): %d.%d.%d.%d, %d \r\n",
		FIRST_IPADDRESS(this->_recvIpAddr), SECOND_IPADDRESS(this->_recvIpAddr),
		THIRD_IPADDRESS(this->_recvIpAddr), FOURTH_IPADDRESS(this->_recvIpAddr),
		_recvPort);
	return ret;
}

DWORD MyUdpServer::GetRemoteIP(WORD *port)
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

INT MyUdpClient::SocketCountRx(INT sockfd)
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

BOOL MyUdpClient::OpenConnected()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2); // ���� ���� 2.2
	INT nonblocking;
	BOOL optReuseaddr = TRUE;	
	INT err;
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: UdpServer::Open (): already opened socket \r\n");
		//printf(_msg);
		this->Close();
	}
	// ���� ������ �Է��ϰ� WSADATA ����ü ���� ��´�.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	// ������ ���� : ������ ���� ��ũ����(�ڵ�), ���� �� -1 ��ȯ
	// IPv4 ���ͳ� ��������, ����������, ���� �������� ���� ��üȭ(0 �Ǵ� IPPROTO_UDP)
	this->_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
	// socket�� opetion (sending/receiving timeout ��) ����
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_RCVTIMEO):");
		goto CLOSE_RET;
	}
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_SNDTIMEO):");
		goto CLOSE_RET;
	}
	// Set the socket to non-blocking mode.
	nonblocking = 1;
	if (ioctlsocket(this->_sockfd, FIONBIO, (ULONG*)&nonblocking) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: ioctlsocket(FIONBIO) \r\n");
		goto CLOSE_RET;
	}
	// UDP������ ���� �ڵ��� ������ ����� ��(_connected == true)���� �����Ѵ�.
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

BOOL MyUdpClient::OpenUnconnected()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2); // ���� ���� 2.2
	BOOL optReuseaddr = TRUE;	
	INT err;
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: UdpServer::Open (): already opened socket \r\n");
		//printf(_msg);
		this->Close();
	}
	// ���� ������ �Է��ϰ� WSADATA ����ü ���� ��´�.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	// ������ ���� : ������ ���� ��ũ����(�ڵ�), ���� �� -1 ��ȯ
	// IPv4 ���ͳ� ��������, ����������, ���� �������� ���� ��üȭ(0 �Ǵ� IPPROTO_UDP)
	this->_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (this->_sockfd == INVALID_SOCKET) {
		wsprintf(this->_msg, "ERROR: socket() \r\n");
		goto CLOSE_RET;
	}

	if (this->_remoteIp == INADDR_BROADCAST) {
		// UDP socket�� broadcasting ���� �����Ѵ�.
		int opt = 1;
		if (setsockopt(_sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
			wsprintf(this->_msg, "ERROR: setsockopt(SO_BROADCAST):");
			goto CLOSE_RET;
		}
		//if (setsockopt (_sockfd, SOL_SOCKET, SO_DONTROUTE, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
		//	OnSocketError ("ERROR: setsockopt(SO_DONTROUTE):");
		//	goto CLOSE_RET;
		//}
	}

	// ���Ͽ��� ����ϴ� Port No�� ���� �����ϵ��� �Ѵ�.
	// listen�ϴ� ������ �ǵ�ġ�ʰ� ���� �� ���� �� ��� �ѵ��� bind ()�� �ȵǴ� ������ �������� �����Ѵ�.	
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuseaddr, sizeof(optReuseaddr) == SOCKET_ERROR)) {
		wsprintf(this->_msg, "ERROR: setsockopt(opt_reuseaddr) \r\n");
		goto CLOSE_RET;
	}
	// socket�� opetion (sending/receiving timeout ��) ����
	if (setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_RCVTIMEO):");
		goto CLOSE_RET;
	}
	if (setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_SNDTIMEO):");
		goto CLOSE_RET;
	}

	// UDP������ ���� �ڵ��� ������ ����� ��(_connected == true)���� �����Ѵ�.
	this->_connected = TRUE;
	return TRUE;

CLOSE_RET:
	if (this->_sockfd != INVALID_SOCKET) {
		closesocket(this->_sockfd);
	}
	this->_sockfd = INVALID_SOCKET;
	WSACleanup(); // ������  ���� ������ �ʱ�ȭ�Ѵ�.
	return FALSE;
}

BOOL MyUdpClient::Close()
{
	wsprintf(_msg, "INFO: MyUdp::Close (): %d.%d.%d.%d, %d \r\n",
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

BOOL MyUdpClient::ConnectConnected()
{
	_recvIpAddr = 0;
	_recvPort = 0;

	if (this->_sockfd == INVALID_SOCKET) OpenConnected();
	if (this->_connected) return TRUE;

	// Open���� nonblocking�� 1�� �����Ǿ��ִ�.
	INT nonblocking = 1;
	INT ret;
	INT error;
	DWORD dwRet = 0;	
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
	wsprintf(this->_msg, "INFO: MyUdpClient::Connect(): Connect to UDP client: %d.%d.%d.%d, %d \r\n",
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

BOOL MyUdpClient::ConnectUnconnected()
{
	INT index;
	_recvIpAddr = 0;
	_recvPort = 0;

	if (this->_sockfd == INVALID_SOCKET) index = OpenUnconnected();
	if (index = TRUE) wsprintf(this->_msg, "INFO: MyUdpClient::Connect(): \r\n");
	return (this->_sockfd != INVALID_SOCKET) && this->_connected;
}

INT MyUdpClient::Send(const TCHAR *msgSrc, INT msgLength)
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

INT MyUdpClient::SendTo(CONST TCHAR *msg, INT len)
{
	if (this->_sockfd == INVALID_SOCKET) return -1;
	if (!this->_connected) return -1;

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(_remoteIp);
	serverAddr.sin_port = htons(this->_port);

	INT ret = sendto(_sockfd, msg, len, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret == 0) {
		this->Close();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT) return 0;

		wsprintf(this->_msg, "ERROR: sendto() \r\n");
		this->Close();
		return -1;
	}
	wsprintf(_msg, "INFO: MyUdpClient::SendTo (): %d.%d.%d.%d, %d \r\n",
		FIRST_IPADDRESS(this->_remoteIp), SECOND_IPADDRESS(this->_remoteIp),
		THIRD_IPADDRESS(this->_remoteIp), FOURTH_IPADDRESS(this->_remoteIp),
		this->_port);
	return ret;
}

INT MyUdpClient::Recv(TCHAR *msgDest, INT msgLength)
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

INT MyUdpClient::RecvFrom(TCHAR *msgDest, INT msgLength)
{
	INT index;
	if (this->_sockfd == INVALID_SOCKET) return -1;
	if (!this->_connected) return -1;

	struct sockaddr_in clientAddr;
	INT clientAddrSize = sizeof(clientAddr);

	INT ret = recvfrom(_sockfd, msgDest, msgLength, 0, (struct sockaddr*)&clientAddr, &clientAddrSize);	
	if (ret == 0) {
		this->Close();
		return 0;
	}
	else if (ret == SOCKET_ERROR) {
		index = WSAGetLastError();
		//if (WSAGetLastError() == WSAETIMEDOUT) return 0;
		if (index == WSAETIMEDOUT) return 0;
		if (index == WSAEINVAL) return 0;

		wsprintf(this->_msg, "ERROR: recvfrom() \r\n");
		this->Close();
		return -1;
	}
	_recvIpAddr = ntohl(clientAddr.sin_addr.s_addr);
	_recvPort = ntohs(clientAddr.sin_port);
	wsprintf(_msg, "INFO: MyUdpClient::Recvfrom (): %d.%d.%d.%d, %d \r\n",
		FIRST_IPADDRESS(this->_recvIpAddr), SECOND_IPADDRESS(this->_recvIpAddr),
		THIRD_IPADDRESS(this->_recvIpAddr), FOURTH_IPADDRESS(this->_recvIpAddr),
		_recvPort);
	return ret;
}

DWORD MyUdpClient::GetRemoteIP(WORD *port)
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