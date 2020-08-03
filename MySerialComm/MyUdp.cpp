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
	//  입력 버퍼에 들어있는 케릭터 수를 카운트한다.
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
	WORD wVersionRequested = MAKEWORD(2, 2); // 버전 정보 2.2
	BOOL optReuseaddr = TRUE;
	struct sockaddr_in serverAddr;
	INT err;
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: UdpServer::Open (): already opened socket \r\n");
		//printf(_msg);
		this->Close();
	}
	// 버전 정보를 입력하고 WSADATA 구조체 값을 얻는다.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	// 소켓의 생성 : 성공시 파일 디스크립터(핸들), 실패 시 -1 반환
	// IPv4 인터넷 프로토콜, 연결지향형, 최종 프로토콜 형태 구체화(0 또는 IPPROTO_UDP)
	this->_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (this->_sockfd == INVALID_SOCKET) {
		wsprintf(this->_msg, "ERROR: socket() \r\n");
		goto CLOSE_RET;
	}
	if (this->_remoteIp == INADDR_BROADCAST) {
		// UDP socket을 broadcasting 으로 설정한다.
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
	// 소켓에서 사용하던 Port No를 재사용 가능하도록 한다.
	// listen하던 소켓이 의도치않게 닫힌 후 새로 연 경우 한동안 bind ()가 안되는 현상을 막기위해 설정한다.	
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuseaddr, sizeof(optReuseaddr) == SOCKET_ERROR)) {
		wsprintf(this->_msg, "ERROR: setsockopt(opt_reuseaddr) \r\n");
		goto CLOSE_RET;
	}
	// socket의 opetion (sending/receiving timeout 값) 설정
	if (setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_RCVTIMEO):");
		goto CLOSE_RET;
	}
	if (setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_SNDTIMEO):");
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
	
	// UDP에서는 소켓 핸들을 얻어오면 연결된 것(_connected == true)으로 간주한다.
	this->_connected = TRUE;
	return TRUE;

CLOSE_RET:
	if (this->_sockfd != INVALID_SOCKET) {
		closesocket(this->_sockfd);
	}
	this->_sockfd = INVALID_SOCKET;
	WSACleanup(); // 윈도우  소켓 정보를 초기화한다.
	return FALSE;
}

BOOL MyUdpServer::Close()
{
	wsprintf(_msg, "INFO: MyUdp::Close (): %d.%d.%d.%d, %d \r\n",
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
	//  입력 버퍼에 들어있는 케릭터 수를 카운트한다.
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
	WORD wVersionRequested = MAKEWORD(2, 2); // 버전 정보 2.2
	INT nonblocking;
	BOOL optReuseaddr = TRUE;	
	INT err;
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: UdpServer::Open (): already opened socket \r\n");
		//printf(_msg);
		this->Close();
	}
	// 버전 정보를 입력하고 WSADATA 구조체 값을 얻는다.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	// 소켓의 생성 : 성공시 파일 디스크립터(핸들), 실패 시 -1 반환
	// IPv4 인터넷 프로토콜, 연결지향형, 최종 프로토콜 형태 구체화(0 또는 IPPROTO_UDP)
	this->_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
	// socket의 opetion (sending/receiving timeout 값) 설정
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
	// UDP에서는 소켓 핸들을 얻어오면 연결된 것(_connected == true)으로 간주한다.
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

BOOL MyUdpClient::OpenUnconnected()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2); // 버전 정보 2.2
	BOOL optReuseaddr = TRUE;	
	INT err;
	if (this->_sockfd != INVALID_SOCKET) {
		wsprintf(this->_msg, "WARNING: UdpServer::Open (): already opened socket \r\n");
		//printf(_msg);
		this->Close();
	}
	// 버전 정보를 입력하고 WSADATA 구조체 값을 얻는다.
	if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0) {
		wsprintf(this->_msg, "WSAStartup failed with error : %d \r\n", err);
		goto CLOSE_RET;
	}
	// 소켓의 생성 : 성공시 파일 디스크립터(핸들), 실패 시 -1 반환
	// IPv4 인터넷 프로토콜, 연결지향형, 최종 프로토콜 형태 구체화(0 또는 IPPROTO_UDP)
	this->_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (this->_sockfd == INVALID_SOCKET) {
		wsprintf(this->_msg, "ERROR: socket() \r\n");
		goto CLOSE_RET;
	}

	if (this->_remoteIp == INADDR_BROADCAST) {
		// UDP socket을 broadcasting 으로 설정한다.
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

	// 소켓에서 사용하던 Port No를 재사용 가능하도록 한다.
	// listen하던 소켓이 의도치않게 닫힌 후 새로 연 경우 한동안 bind ()가 안되는 현상을 막기위해 설정한다.	
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuseaddr, sizeof(optReuseaddr) == SOCKET_ERROR)) {
		wsprintf(this->_msg, "ERROR: setsockopt(opt_reuseaddr) \r\n");
		goto CLOSE_RET;
	}
	// socket의 opetion (sending/receiving timeout 값) 설정
	if (setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_RCVTIMEO):");
		goto CLOSE_RET;
	}
	if (setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&_timeout, sizeof(_timeout)) == SOCKET_ERROR) {
		wsprintf(this->_msg, "ERROR: setsockopt(SO_SNDTIMEO):");
		goto CLOSE_RET;
	}

	// UDP에서는 소켓 핸들을 얻어오면 연결된 것(_connected == true)으로 간주한다.
	this->_connected = TRUE;
	return TRUE;

CLOSE_RET:
	if (this->_sockfd != INVALID_SOCKET) {
		closesocket(this->_sockfd);
	}
	this->_sockfd = INVALID_SOCKET;
	WSACleanup(); // 윈도우  소켓 정보를 초기화한다.
	return FALSE;
}

BOOL MyUdpClient::Close()
{
	wsprintf(_msg, "INFO: MyUdp::Close (): %d.%d.%d.%d, %d \r\n",
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

BOOL MyUdpClient::ConnectConnected()
{
	_recvIpAddr = 0;
	_recvPort = 0;

	if (this->_sockfd == INVALID_SOCKET) OpenConnected();
	if (this->_connected) return TRUE;

	// Open에서 nonblocking이 1로 설정되어있다.
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