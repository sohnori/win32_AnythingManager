#pragma once
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

//#pragma comment(lib, "ws2_32.lib")

class MyUdpServer
{
private:
	DWORD _localIp;
	DWORD _remoteIp;
	WORD _port;
	DWORD _timeout;
	SOCKET _sockfd;
	BOOL _connected;
	TCHAR _msg[MAX_PATH] = { 0, };
public:
	DWORD _recvIpAddr;
	WORD _recvPort;
private:
	INT SocketCountRx(INT sockfd);
public:
	MyUdpServer() : _localIp(0), _remoteIp(0), _port(0), _timeout(0), _sockfd(INVALID_SOCKET), _connected(FALSE) {}
	MyUdpServer(DWORD localIp, DWORD remoteIp, WORD port, DWORD timeout) :
		_localIp(localIp), _remoteIp(remoteIp), _port(port), _timeout(timeout), _sockfd(INVALID_SOCKET),
		_connected(FALSE) {}
	~MyUdpServer() { if (this->_sockfd != INVALID_SOCKET) Close(); }
	BOOL Open();
	BOOL Close();
	BOOL Connect();

	INT SendTo(const TCHAR *msgSrc, INT msgLength);
	INT RecvFrom(TCHAR *msgDest, INT msgLength);
	VOID Purge() {}

	BOOL IsConnected() { return (this->_sockfd != INVALID_SOCKET) && this->_connected; }
	INT CountRx() { return (_sockfd != INVALID_SOCKET) ? SocketCountRx(this->_sockfd) : -1; }
	DWORD GetRemoteIP(WORD *port);
	TCHAR *GetErrorMSG() { return this->_msg; }
};

class MyUdpClient
{
private:
	DWORD _localIp;
	DWORD _remoteIp;
	WORD _port;
	DWORD _timeout;
	SOCKET _sockfd;
	BOOL _connected;
	TCHAR _msg[MAX_PATH] = { 0, };
public:	
	DWORD _recvIpAddr;
	WORD _recvPort;
private:
	INT SocketCountRx(INT sockfd);
public:
	MyUdpClient() : _localIp(0), _remoteIp(0), _port(0), _timeout(0), _sockfd(INVALID_SOCKET), _connected(FALSE) {}
	MyUdpClient(DWORD localIp, DWORD remoteIp, WORD port, DWORD timeout) :
		_localIp(localIp), _remoteIp(remoteIp), _port(port), _timeout(timeout), _sockfd(INVALID_SOCKET),
		_connected(FALSE) {}
	~MyUdpClient() { if (this->_sockfd != INVALID_SOCKET) Close(); }
	BOOL OpenConnected();
	BOOL OpenUnconnected();
	BOOL Close();
	BOOL ConnectConnected();
	BOOL ConnectUnconnected();

	INT Send(const TCHAR *msgSrc, INT msgLength);
	INT SendTo(const TCHAR *msgSrc, INT msgLength);
	INT Recv(TCHAR *msgDest, INT msgLength);
	INT RecvFrom(TCHAR *msgDest, INT msgLength);
	VOID Purge() {}

	BOOL IsConnected() { return (this->_sockfd != INVALID_SOCKET) && this->_connected; }
	INT CountRx() { return (_sockfd != INVALID_SOCKET) ? SocketCountRx(this->_sockfd) : -1; }
	DWORD GetRemoteIP(WORD *port);
	TCHAR *GetErrorMSG() { return this->_msg; }
};