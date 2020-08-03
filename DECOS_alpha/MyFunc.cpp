#include "MyFunc.h"
#include <stdio.h>

// 장치 등록정보파일이름을 검색하여 리스트 박스에 표시한다.
INT FindDeviceFile(HWND hList)
{
	INT cnt = 0;
	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	TCHAR fname[MAX_PATH];
	BOOL bResult = TRUE;
	TCHAR Path[MAX_PATH] = ".\\*.dcs";
	//TCHAR drive[MAX_PATH];
	//TCHAR dir[MAX_PATH];

	//GetWindowsDirectory(Path, MAX_PATH);
	//lstrcat(Path, "\\*.*");
	hSearch = FindFirstFile(Path, &wfd);
	if (hSearch == INVALID_HANDLE_VALUE) return FALSE;
	//_splitpath_s(Path, drive, MAX_PATH, dir, MAX_PATH, fname, MAX_PATH, NULL, NULL);
	while (bResult) {
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			wsprintf(fname, "[%s]", wfd.cFileName);
		}
		else {
			wsprintf(fname, "%s", wfd.cFileName);
		}
		//SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)Path);
		//SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)drive);
		//SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)dir);
		SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)fname);
		bResult = FindNextFile(hSearch, &wfd);
		cnt++;
	}
	FindClose(hSearch);
	return cnt;
}

INT FindDeviceFile(TCHAR *dest)
{
	INT cnt = 0;
	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	TCHAR fname[MAX_PATH];
	BOOL bResult = TRUE;
	TCHAR Path[MAX_PATH] = ".\\*.dcs";
	fname[0] = NULL;
	dest[0] = NULL;
	hSearch = FindFirstFile(Path, &wfd);
	if (hSearch == INVALID_HANDLE_VALUE) return FALSE;

	while (bResult) {
		//wsprintf(fname, "%s", wfd.cFileName);
		strcat_s(fname, wfd.cFileName);
		strcat_s(fname, ";");		
		bResult = FindNextFile(hSearch, &wfd);
		cnt++;
	}
	FindClose(hSearch);
	wsprintf(dest, "%s", fname);
	//lstrcpy(dest, fname);
	return cnt;
}

INT FindDeviceFile(HWND hList, TCHAR *dest)
{
	INT cnt = 0;
	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	TCHAR fname[MAX_PATH];
	BOOL bResult = TRUE;
	TCHAR Path[MAX_PATH] = ".\\*.dcs";
	fname[0] = NULL;
	dest[0] = NULL;
	hSearch = FindFirstFile(Path, &wfd);
	if (hSearch == INVALID_HANDLE_VALUE) return FALSE;

	while (bResult) {
		//wsprintf(fname, "%s", wfd.cFileName);
		strcat_s(fname, wfd.cFileName);
		SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)wfd.cFileName);
		strcat_s(fname, ";");
		bResult = FindNextFile(hSearch, &wfd);
		cnt++;
	}
	FindClose(hSearch);
	wsprintf(dest, "%s", fname);
	//lstrcpy(dest, fname);
	return cnt;
}

INT FindDeviceFileEX(HWND hList, TCHAR *externName)
{
	INT cnt = 0;
	HANDLE hSearch;
	WIN32_FIND_DATA wfd;	
	BOOL bResult = TRUE;
	TCHAR Path[MAX_PATH] = ".\\*.";
	strncat_s(Path, externName, 3);	
	hSearch = FindFirstFile(Path, &wfd);
	if (hSearch == INVALID_HANDLE_VALUE) return FALSE;

	while (bResult) {		
		SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)wfd.cFileName);		
		bResult = FindNextFile(hSearch, &wfd);
		cnt++;
	}
	FindClose(hSearch);	
	return cnt;
}

BOOL PurgeBuff(TCHAR *buf, INT count)
{
	INT cnt;
	for (cnt = 0; cnt < count; cnt++) {
		if (cnt > 10000) return FALSE;
		buf[cnt] = NULL;
	}
	return TRUE;
}

INT AstrToHstr(TCHAR *dest, TCHAR *src)
{
	INT cnt;
	INT cnt1 = 0;
	TCHAR oneChar[6];
	for (cnt = 0; *(src + cnt) != NULL; cnt++) {
		if (cnt > 1023) return -1;
		wsprintf(oneChar, TEXT("%02X "), *(src+cnt));
		*(dest + cnt1) = oneChar[0];
		*(dest + cnt1+1) = oneChar[1];
		*(dest + cnt1+2) = oneChar[2];		
		cnt1 += 3;
	}
	*(dest + cnt1) = NULL;
	return cnt;
}

INT AstrToHstr(TCHAR *dest, TCHAR *src, INT dataCount)
{
	INT cnt;
	INT cnt1 = 0;
	TCHAR oneChar[6];
	for (cnt = 0; cnt<dataCount; cnt++) {
		if (cnt > 1023) return -1;
		sprintf_s(oneChar, TEXT("%02X "), *(src + cnt));
		*(dest + cnt1) = oneChar[0];
		*(dest + cnt1 + 1) = oneChar[1];
		*(dest + cnt1 + 2) = oneChar[2];
		cnt1 += 3;
	}
	*(dest + cnt1) = NULL;
	return cnt1;
}

USHORT HtoA2(TCHAR value)
{
	TCHAR hex_H = (value >> 4) & 0x0F;
	TCHAR hex_L = value & 0x0F;
	USHORT ret;
	if (hex_H > 9) hex_H += 0x37;
	else hex_H += 0x30;
	if (hex_L > 9) hex_L += 0x37;
	else hex_L += 0x30;
	ret = hex_H;
	ret = (ret << 8) & 0xff00;
	ret |= hex_L;
	return ret;
}

BOOL HtoA2(TCHAR value, TCHAR *retHexHigh, TCHAR *retHexLow)
{
	*retHexHigh = (value >> 4) & 0x0F;
	*retHexLow = value & 0x0F;	
	if (*retHexHigh > 9) *retHexHigh += 0x37;
	else *retHexHigh += 0x30;
	if (*retHexLow > 9) *retHexLow += 0x37;
	else *retHexLow += 0x30;	
	return TRUE;
}

USHORT H2ValuetoShort(TCHAR high, TCHAR low, BOOL option)
{	
	USHORT valueH;
	USHORT valueL;
	if (option == 0) {
		valueH = high;		
		valueL = 0x00ff & low;
		valueH <<= 8;
		valueH |= valueL;
		return valueH;
	}
	else if (option == 1) {
		valueH = low;
		valueL = 0x00ff & high;
		valueH <<= 8;
		valueH |= valueL;
		return valueH;
	}
	return 0;
}

INT AtoHstr(TCHAR *dest, TCHAR *src, INT dataCount)
{
	INT cnt;
	INT cnt1 = 0;
	TCHAR oneChar[4];
	for (cnt = 0; cnt<dataCount; cnt++) {
		if (cnt > 1023) return -1;
		//sprintf_s(oneChar, TEXT("%02X "), *(src + cnt));
		HtoA2(*(src + cnt), &oneChar[0], &oneChar[1]);
		*(dest + cnt1) = oneChar[0];
		*(dest + cnt1 + 1) = oneChar[1];
		*(dest + cnt1 + 2) = ' ';
		cnt1 += 3;
	}
	*(dest + cnt1) = NULL;
	return cnt1;
}

TCHAR AtoHex(TCHAR ascii)
{
	if ((ascii <= 0x39) && (ascii >= 0x30))	ascii -= 0x30;
	else if ((ascii >= 0x41) && (ascii <= 0x46)) ascii -= 0x37;
	else if ((ascii >= 0x61) && (ascii <= 0x66)) ascii -= 0x57;
	else return 0xFF;
	return ascii;
}

TCHAR A2CtoHex(TCHAR srcHigh, TCHAR srcLow)
{
	if ((srcHigh <= 0x39) && (srcHigh >= 0x30))	srcHigh -= 0x30;
	else if ((srcHigh >= 0x41) && (srcHigh <= 0x46)) srcHigh -= 0x37;
	else if ((srcHigh >= 0x61) && (srcHigh <= 0x66)) srcHigh -= 0x57;
	else return 0xFF;

	if ((srcLow <= 0x39) && (srcLow >= 0x30))	srcLow -= 0x30;
	else if ((srcLow >= 0x41) && (srcLow <= 0x46)) srcLow -= 0x37;
	else if ((srcLow >= 0x61) && (srcLow <= 0x66)) srcLow -= 0x57;
	else return 0xFF;

	srcLow |= (srcHigh << 4) & 0xf0;
	return srcLow;
}

INT AsciiHextoHarr(TCHAR *dest, TCHAR *src, INT dataCount)
{
	INT cnt;
	INT cnt1 = 0;
	INT cnt2 = 0;
	for (cnt = 0; cnt<dataCount; cnt++) {
		if (cnt > 1023) return -1;
		if (*(src + cnt) != 0x20) {
			if (cnt2 == 0) {
				*(dest + cnt1) = AtoHex(*(src + cnt));
				cnt2++;
			}
			else {
				*(dest + cnt1) <<= 4;
				*(dest + cnt1) |= AtoHex(*(src + cnt));
				cnt2 = 0;
				cnt1++;
			}						
		}				
	}
	*(dest + cnt1) = NULL;
	return cnt1;
}

BOOL CopyData(TCHAR *dest, TCHAR *src, TCHAR cnt)
{
	INT cnt2;
	for (cnt2 = 0; cnt2 < cnt; cnt2++) {
		if (cnt > 254) return FALSE;
		dest[cnt2] = src[cnt2];
	}
	return TRUE;
}

BOOL ConvertStrOrigin(TCHAR * str)
{
	INT size = lstrlen(str);
	INT buff[MAX_PATH];
	INT cnt = 0;
	INT cnt2 = 0;
	if (size > MAX_PATH) return FALSE;
	for (cnt = 0; str[cnt] != NULL; cnt++) {
		buff[cnt2] = str[cnt];
		if (str[cnt]==0x5C && str[cnt+1]==0x5C) cnt2--;
		cnt2++;
	}
	for (cnt = 0; cnt < cnt2; cnt++) str[cnt] = buff[cnt];
	str[cnt] = NULL;
	return TRUE;
}
