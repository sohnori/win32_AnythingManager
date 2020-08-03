#pragma once
#include <Windows.h>

INT FindDeviceFile(HWND hList);
INT FindDeviceFile(TCHAR *dest);
INT FindDeviceFile(HWND hList, TCHAR *dest);
INT FindDeviceFileEX(HWND hList, TCHAR *externName);
//INT GetFileName(TCHAR *dest, TCHAR *src, UINT option);
BOOL PurgeBuff(TCHAR *buf, INT count);
INT AstrToHstr(TCHAR *dest, TCHAR *src);
INT AstrToHstr(TCHAR *dest, TCHAR *src, INT dataCount);
USHORT HtoA2(TCHAR value);
BOOL HtoA2(TCHAR value, TCHAR *retHexHigh, TCHAR *retHexLow);
USHORT H2ValuetoShort(TCHAR high, TCHAR low, BOOL option);
INT AtoHstr(TCHAR *dest, TCHAR *src, INT dataCount);
TCHAR AtoHex(TCHAR ascii);
TCHAR A2CtoHex(TCHAR srcHigh, TCHAR srcLow);
INT AsciiHextoHarr(TCHAR *dest, TCHAR *src, INT dataCount);
BOOL CopyData(TCHAR *dest, TCHAR *src, TCHAR cnt);
BOOL ConvertStrOrigin(TCHAR * str);