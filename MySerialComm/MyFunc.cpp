#include "MyFunc.h"

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
	if (dataCount > 1023) return -1;
	for (cnt = 0; cnt<dataCount; cnt++) {		
		wsprintf(oneChar, TEXT("%02X "), *(src + cnt));
		*(dest + cnt1) = oneChar[0];
		*(dest + cnt1 + 1) = oneChar[1];
		*(dest + cnt1 + 2) = oneChar[2];
		cnt1 += 3;
	}
	*(dest + cnt1) = NULL;
	return cnt1;
}

USHORT HtoA2C(TCHAR value)
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

BOOL HtoA2C(TCHAR value, TCHAR *retHexHigh, TCHAR *retHexLow)
{
	*retHexHigh = (value >> 4) & 0x0F;
	*retHexLow = value & 0x0F;	
	if (*retHexHigh > 9) *retHexHigh += 0x37;
	else *retHexHigh += 0x30;
	if (*retHexLow > 9) *retHexLow += 0x37;
	else *retHexLow += 0x30;	
	return TRUE;
}

USHORT H2ValueToShort(TCHAR high, TCHAR low, BOOL option)
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

INT HarrToHexASCIIstr(TCHAR *dest, TCHAR *src, INT dataCount)
{
	INT cnt;
	INT cnt1 = 0;
	TCHAR oneChar[4];
	if (dataCount > 1023) return -1;
	for (cnt = 0; cnt<dataCount; cnt++) { 
		//sprintf_s(oneChar, TEXT("%02X "), *(src + cnt));
		HtoA2C(*(src + cnt), &oneChar[0], &oneChar[1]);
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

INT HexAsciiArrToHarr(TCHAR *dest, TCHAR *src, INT dataCount)
{
	INT cnt;
	INT cnt1 = 0;
	INT cnt2 = 0;
	if (dataCount > 1023) return -1;
	for (cnt = 0; cnt<dataCount; cnt++) {		
		if (*(src + cnt) == 0x20) continue;
		if (*(src + cnt) < 0x30 || *(src + cnt) > 0x67) continue;
		if (*(src + cnt) >= 0x3A && *(src + cnt) <= 0x40) continue;
		if (*(src + cnt) >= 0x47 && *(src + cnt) <= 0x60) continue;
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
	*(dest + cnt1) = NULL;
	return cnt1;
}

INT AsciiArrToHexAsciiArr(TCHAR *dest, TCHAR *src, INT dataCount) 
{
	INT cnt;
	INT cnt1 = 0;
	if (dataCount > 1023) return -1;
	for (cnt = 0; cnt < dataCount; cnt++) {		
		HtoA2C(*(src + cnt), &dest[cnt1], &dest[cnt1 + 1]);
		dest[cnt1 + 2] = ' ';
		cnt1 += 3;
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

INT ConvertAsciiArrEscapeSeq(TCHAR *dest, TCHAR *src, INT dataCount)
{
	INT cnt;
	INT cnt1 = 0;
	INT cnt2 = 0;
	if (dataCount > 1023) return -1;
	for (cnt = 0; cnt < dataCount; cnt++) {		
		if (*(src + cnt) == '\\'){			
			if (*(src + cnt + 1) == 'n') *(dest + cnt1) = 0x0A;
			else if(*(src+cnt+1)=='t') *(dest + cnt1) = 0x09;
			else if(*(src+cnt+1)=='v') *(dest + cnt1) = 0x0B;
			else if(*(src+cnt+1)=='b') *(dest + cnt1) = 0x08;
			else if(*(src+cnt+1)=='r') *(dest + cnt1) = 0x0D;
			else if(*(src+cnt+1)=='a') *(dest + cnt1) = 0x07;
			else if(*(src+cnt+1)=='\\') *(dest + cnt1) = 0x5C;
			else if(*(src+cnt+1)=='?') *(dest + cnt1) = 0x3F;
			else if(*(src+cnt+1)=='\'') *(dest + cnt1) = 0x27;
			else if(*(src+cnt+1)=='"') *(dest + cnt1) = 0x22;
			cnt++;
			cnt1++;
		}
		else {
			*(dest + cnt1) = *(src + cnt);
			cnt1++;
		}
	}	
	return cnt1;
}
