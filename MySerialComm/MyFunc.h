#pragma once
#include <Windows.h>

//INT GetFileName(TCHAR *dest, TCHAR *src, UINT option);
BOOL PurgeBuff(TCHAR *buf, INT count);
INT AstrToHstr(TCHAR *dest, TCHAR *src);
INT AstrToHstr(TCHAR *dest, TCHAR *src, INT dataCount);
USHORT HtoA2C(TCHAR value);
BOOL HtoA2C(TCHAR value, TCHAR *retHexHigh, TCHAR *retHexLow);
USHORT H2ValueToShort(TCHAR high, TCHAR low, BOOL option);
INT HarrToHexASCIIstr(TCHAR *dest, TCHAR *src, INT dataCount);
TCHAR AtoHex(TCHAR ascii);
TCHAR A2CtoHex(TCHAR srcHigh, TCHAR srcLow);
INT HexAsciiArrToHarr(TCHAR *dest, TCHAR *src, INT dataCount);
INT AsciiArrToHexAsciiArr(TCHAR *dest, TCHAR *src, INT dataCount);
BOOL CopyData(TCHAR *dest, TCHAR *src, TCHAR cnt);
BOOL ConvertStrOrigin(TCHAR * str);
INT ConvertAsciiArrEscapeSeq(TCHAR *dest, TCHAR *src, INT dataCount);