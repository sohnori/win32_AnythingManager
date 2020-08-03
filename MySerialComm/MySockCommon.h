#pragma once
#include <Windows.h>

#define MY_ADDR_HOST	1
#define MY_ADDR_NET		2

INT HaddrToNaddr(ULONG *dest, ULONG src);

INT NaddrToHaddr(ULONG *dest, ULONG src);

INT HportToNport(USHORT *dest, USHORT src);

INT NportToHport(USHORT *dest, USHORT src);

INT HostAddrStrToLong(ULONG *dest, CONST TCHAR *src, UCHAR option);

INT HostAddrLongToStr(TCHAR *dest, CONST ULONG src, INT cbDest);

INT IPaddrStrToLong(DWORD *dest, TCHAR *src, TCHAR option);

INT IPaddrLongToStr(TCHAR *dest, CONST ULONG src, UCHAR option);

INT GetLocalIP(ULONG localAddr[], INT cntLocalAddr);

INT GetHostIPAddress();