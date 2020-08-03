#pragma once
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

//#pragma comment(lib, "ws2_32.lib")

class MyTcpServer
{ // socket()소켓생성 -> bind()소켓주소할당 -> listen()연결요청대기상태 -> accept()연결허용 -> 
	// read()/write()데이터 송수신, close()연결종료
private:
	DWORD _localIp;
	DWORD _remoteIp;
	WORD _port;
	DWORD _timeout;
	SOCKET _sockfd;
	BOOL _connected;
	TCHAR _msg[MAX_PATH] = { 0, };	
private:
	INT SocketCountRx(INT sockfd);
public:
	MyTcpServer() : _localIp(0), _remoteIp(0), _port(0), _timeout(0), _sockfd(INVALID_SOCKET), _connected(FALSE) {}
	MyTcpServer(DWORD localIp, WORD port, DWORD timeout) : 
		_localIp(localIp), _remoteIp(0), _port(port), _timeout(timeout), _sockfd(INVALID_SOCKET),
		_connected(FALSE){}
	~MyTcpServer() { if (this->_sockfd != INVALID_SOCKET) Close(); }
	BOOL Open();
	BOOL Close();
	BOOL Connect();

	INT Send(const TCHAR *msgSrc, INT msgLength);
	INT Recv(TCHAR *msgDest, INT msgLength);
	VOID Purge() {}

	BOOL IsConnected() { return (this->_sockfd != INVALID_SOCKET) && this->_connected; }
	INT CountRx() { return (_sockfd != INVALID_SOCKET) ? SocketCountRx(this->_sockfd) : -1; }
	DWORD GetRemoteIP(WORD *port);	
	TCHAR *GetErrorMSG() { return this->_msg; }
};

class MyTcpClient
{ // socket()소켓생성 -> connect()연결요청 -> read()/write()데이터송수신 -> close()연결종료
private:
	DWORD _localIp;
	DWORD _remoteIp;
	WORD _port;
	DWORD _timeout;
	SOCKET _sockfd;
	BOOL _connected;
	TCHAR _msg[MAX_PATH] = { 0, };
private:	
	INT SocketCountRx(INT sockfd);
public:
	MyTcpClient() : _localIp(0), _remoteIp(0), _port(0), _timeout(0), _sockfd(INVALID_SOCKET), _connected(FALSE) {}
	MyTcpClient(DWORD remoteIP, WORD port, DWORD timeout) :
		_localIp(0), _remoteIp(remoteIP), _port(port), _timeout(timeout), _sockfd(INVALID_SOCKET),
		_connected(FALSE) {}
	~MyTcpClient() { if (this->_sockfd != INVALID_SOCKET) Close(); }
	BOOL Open();
	BOOL Close();
	BOOL Connect();

	INT Send(const TCHAR *msgSrc, INT msgLength);
	INT Recv(TCHAR *msgDest, INT msgLength);
	VOID Purge() {}

	BOOL IsConnected() { return (this->_sockfd != INVALID_SOCKET) && this->_connected; }
	INT CountRx() { return (this->_sockfd != INVALID_SOCKET) ? SocketCountRx(this->_sockfd) : -1; }
	DWORD GetRemoteIP(WORD *port);
	TCHAR *GetErrorMSG() { return this->_msg; }
};