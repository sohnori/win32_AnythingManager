#include "MySockCommon.h"
#include <iphlpapi.h>
#include <stdlib.h>

#pragma comment(lib, "Iphlpapi.lib") // ip변환 관련 라이브러리 링크

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

INT HaddrToNaddr(ULONG *dest, ULONG src)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

	*dest = htonl(src);
	WSACleanup();
	return 1;
}

INT NaddrToHaddr(ULONG *dest, ULONG src)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

	*dest = ntohl(src);
	WSACleanup();
	return 1;
}

INT HportToNport(USHORT *dest, USHORT src)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

	*dest = htons(src);
	WSACleanup();
	return 1;
}

INT NportToHport(USHORT *dest, USHORT src)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

	*dest = ntohs(src);
	WSACleanup();
	return 1;
}

INT HostAddrStrToLong(ULONG *dest, CONST TCHAR *src, UCHAR option)
{		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;
		
	*dest = inet_addr(src);
	if (option == MY_ADDR_HOST) *dest = ntohl(*dest);
	if (*dest == INADDR_NONE) {
		WSACleanup();
		return -1;
	}
	WSACleanup();
	return 1;
}

INT HostAddrLongToStr(TCHAR *dest, CONST ULONG src, INT cbDest)
{
	INT cnt;
	WSADATA wsaData;
	struct sockaddr_in	addr;
	TCHAR *strPtr;
	TCHAR strArr[20];
	if (cbDest < 20) return -1;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

	addr.sin_addr.s_addr = htonl(src);
	strPtr = inet_ntoa(addr.sin_addr);
	strcpy_s(strArr, strPtr);
	for (cnt = 0; strArr[cnt] != NULL && cnt < 20; cnt++) {		
		dest[cnt] = strArr[cnt];
	}
	dest[cnt] = NULL;
	WSACleanup();
	return cnt;
}

INT IPaddrStrToLong(DWORD *dest, TCHAR *src, TCHAR option)
{
	INT cnt;
	INT cnt1;
	INT cnt2 = 0;
	TCHAR firstAddr[4];
	TCHAR secondAddr[4];
	TCHAR thirdAddr[4];
	TCHAR fourthAddr[4];
	UCHAR firstValue;
	UCHAR secondValue;
	UCHAR thirdValue;
	UCHAR fourthValue;
	for (cnt = 0; src[cnt] != '.'; cnt++) {
		if (cnt > 3) return -1;
		firstAddr[cnt] = src[cnt];
	}	
	firstAddr[cnt] = NULL;
	cnt++;
	cnt1 = cnt;
	for (; src[cnt] != '.'; cnt++) {
		if (cnt > cnt1 + 3) return -1;
		secondAddr[cnt2] = src[cnt];
		cnt2++;
	}	
	secondAddr[cnt2] = NULL;
	cnt++;
	cnt1 = cnt;
	cnt2 = 0;
	for (; src[cnt] != '.'; cnt++) {
		if (cnt > cnt1 + 3) return -1;
		thirdAddr[cnt2] = src[cnt];
		cnt2++;
	}	
	thirdAddr[cnt2] = NULL;
	cnt++;
	cnt1 = cnt;
	cnt2 = 0;
	for (; src[cnt] != NULL; cnt++) {
		if (cnt > cnt1 + 3) return -1;
		fourthAddr[cnt2] = src[cnt];
		cnt2++;
	}
	fourthAddr[cnt2] = NULL;

	cnt = atoi(firstAddr);
	if (cnt > 255) return -1;
	firstValue = (UCHAR)cnt;
	cnt = atoi(secondAddr);
	if (cnt > 255) return -1;
	secondValue = (UCHAR)cnt;
	cnt = atoi(thirdAddr);
	if (cnt > 255) return -1;
	thirdValue = (UCHAR)cnt;
	cnt = atoi(fourthAddr);
	if (cnt > 255) return -1;
	fourthValue = (UCHAR)cnt;
	switch (option) {
	case MY_ADDR_HOST:
		*dest = (DWORD)(firstValue << 24)&0xFF000000;
		*dest |= (DWORD)(secondValue << 16)&0x00FF0000;
		*dest |= (DWORD)(thirdValue << 8)&0x0000FF00;
		*dest |= (DWORD)(fourthValue)&0x000000FF;
		break;
	case MY_ADDR_NET:
		*dest = (DWORD)(fourthValue << 24) & 0xFF000000;
		*dest |= (DWORD)(thirdValue << 16) & 0x00FF0000;
		*dest |= (DWORD)(secondValue << 8) & 0x0000FF00;
		*dest |= (DWORD)(firstValue) & 0x000000FF;
		break;	
	default:
		return 0;
	}
	return 1;
}

INT IPaddrLongToStr(TCHAR *dest, CONST ULONG src, UCHAR option)
{
	switch (option) {
	case MY_ADDR_HOST:
		wsprintf(dest, "%d.%d.%d.%d",
			(UINT)(src >> 24) & 0xFF,
			(UINT)(src >> 16) & 0xFF,
			(UINT)(src >> 8) & 0xFF,
			(UINT)(src) & 0xFF
		);
		break;
	case MY_ADDR_NET:
		wsprintf(dest, "%d.%d.%d.%d",
			(UINT)(src) & 0xFF,
			(UINT)(src >> 8) & 0xFF,
			(UINT)(src >> 16) & 0xFF,
			(UINT)(src >> 24) & 0xFF
		);
		break;
	default:
		return 0;
	}
	return 1;
}

INT GetLocalIP(ULONG localAddr[], INT cntLocalAddr)
{
	WSADATA wsaData;
	struct hostent *hsock;
	char host_name[MAX_PATH];
	INT cnt;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;
	if (gethostname(host_name, MAX_PATH) == SOCKET_ERROR) return -1;
	hsock = gethostbyname(host_name);
	if (hsock == NULL) {
		WSACleanup();
		return -1;
	}
	for (cnt = 0; hsock->h_addr_list[cnt]!=NULL && cnt<cntLocalAddr; cnt++) {
		localAddr[cnt] = ntohl(*(DWORD *)hsock->h_addr_list[cnt]);
	}
	WSACleanup();
	return cnt;
}

INT GetHostIPAddress()
{
	DWORD size = 0;
	DWORD dwGwip;
	DWORD dwLocalip;
	PIP_ADDR_STRING pAddrStr;
	INT ret = GetAdaptersInfo(NULL, &size);
	IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO*)malloc(size);	 		
	ret = GetAdaptersInfo(pAdapterInfo, &size);
	if (ret) {
		free(pAdapterInfo);
		return 1;
	}
	// 아답터 정보를 얻는다. 널이 아닐때까지 아답터 넥스트 포인터를 추종한다.
	// 추후 여유있을때 정보 얻는 함수 코딩 필요
	for (IP_ADAPTER_INFO *pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next) {
		dwGwip = inet_addr(pAdapter->GatewayList.IpAddress.String);
		
		for (pAddrStr = &(pAdapter->IpAddressList); pAddrStr; pAddrStr = pAddrStr->Next) {
			dwLocalip = inet_addr(pAddrStr->IpAddress.String);
		}

	}

	free(pAdapterInfo);
	return 1;
}
