#include "ConvertData.h"

#define NULL	0

/***************************
*      Private Function    *
****************************/

/***************************
*      Public Function     *
****************************/

unsigned char ConvertASCIICase(unsigned char value)
{
	if ((value>=0x41 && value<=0x5A)) return value += 0x20;
	else if ((value>=0x61 && value<=0x7A)) return value -= 0x20;
	else return value;
}

unsigned char ConverttoUpperCase(unsigned char value)
{
	if ((value>=0x61 && value<=0x7A)) return value -= 0x20;
	else return value;
}

unsigned char ConverttoLowerCase(unsigned char value)
{
	if ((value>=0x41 && value<=0x5A)) return value += 0x20;	
	else return value;
}

char ASCIICHARtoHEX(unsigned char value, int option)
{
	if(value>=0x30 && value<=0x39) return value -= 0x30;
	else if (option==HEX && (value>=0x41 && value<=0x46)) return value -= 0x37;
	else if (option==HEX && (value>=0x61 && value<=0x66)) return value -= 0x57;
	else return -1;
}

unsigned char ASCII2CHARtoHEX(unsigned char nibbleH, unsigned char nibbleL, int option)
{
	if ((nibbleH >= 0x30) && (nibbleH <= 0x39)) nibbleH -= 0x30;
	else if (option==HEX && (nibbleH >= 0x41) && (nibbleH <= 0x46)) nibbleH -=0x37;
	else if (option==HEX && (nibbleH >= 0x61) && (nibbleH <= 0x66)) nibbleH -=0x57;
	else return 0xFF;

	if ((nibbleL >= 0x30) && (nibbleL <= 0x39))	nibbleL -= 0x30;
	else if (option==HEX && (nibbleL >= 0x41) && (nibbleL <= 0x46)) nibbleL -=0x37;
	else if (option==HEX && (nibbleL >= 0x61) && (nibbleL <= 0x66)) nibbleL -=0x57;
	else return 0xFF;

	nibbleL |= (nibbleH<<4) & 0xf0;
	if (option==DEC) { // convert ASCII decimal value to HEX
		if (nibbleL>=0x10 && nibbleL<0x20) return nibbleL -= 6;
		if (nibbleL>=0x20 && nibbleL<0x30) return nibbleL -= 12;
		if (nibbleL>=0x30 && nibbleL<0x40) return nibbleL -= 18;
		if (nibbleL>=0x40 && nibbleL<0x50) return nibbleL -= 24;
		if (nibbleL>=0x50 && nibbleL<0x60) return nibbleL -= 30;
		if (nibbleL>=0x60 && nibbleL<0x70) return nibbleL -= 36;
		if (nibbleL>=0x70 && nibbleL<0x80) return nibbleL -= 42;
		if (nibbleL>=0x80 && nibbleL<0x90) return nibbleL -= 48;
		if (nibbleL>=0x90) return nibbleL -= 54;	
	}	
	return nibbleL;	
}

unsigned char ASCIISHORTtoHEX(unsigned short src, int option)
{
	unsigned char nibbleH = (unsigned char) (src>>8);
	unsigned char nibbleL = (unsigned char) src;
	return ASCII2CHARtoHEX(nibbleH, nibbleL, option);
}

char HEXtoASCIICHAR(unsigned char src, int option)
{
	if (src>=0x00 && src<=0x09) return src += 0x30;
	else if (option==UPPERCASE && (src>=0x0A && src<=0x0F)) return src += 0x37;
	else if (option==LOWERCASE && (src>=0x0A && src<=0x0F)) return src += 0x57;
	else return -1;
}

short HEXtoASCII2Byte(unsigned char src, int option)
{
	unsigned char nibbleH = (src>>4) & 0x0F;
	unsigned char nibbleL = src & 0x0F;
	unsigned short ret;
	ret = HEXtoASCIICHAR(nibbleH, option);
	ret = (ret<<8) & 0xFF00;
	ret |= HEXtoASCIICHAR(nibbleL, option);
	return ret;
}

unsigned char HtoA2(char value, char *retHexHigh, char *retHexLow)
{
	*retHexHigh = (value >> 4) & 0x0F;
	*retHexLow = value & 0x0F;
	if (*retHexHigh > 9) *retHexHigh += 0x37;
	else *retHexHigh += 0x30;
	if (*retHexLow > 9) *retHexLow += 0x37;
	else *retHexLow += 0x30;
	return 1;
}

int HEXtoASCIIHighLow(char *destHigh, char *destLow, unsigned char src, int option)
{
	unsigned char nibbleH = (src>>4) & 0x0F;
	unsigned char nibbleL = src & 0x0F;
	*destHigh = HEXtoASCIICHAR(nibbleH, option);	
	*destLow = HEXtoASCIICHAR(nibbleL, option);
	return 1;
}

int ASCIItoLong(long *dest, const unsigned char src[], unsigned char szSrc)
{
	unsigned char cnt = 0;
	unsigned char cnt1;
	unsigned long deca = 1; // deca-position value
	if (szSrc>10) return -1;
	if (src[0]=='-') cnt = 1;
	*dest = 0;
	for(;cnt<szSrc;cnt++){
		if (src[cnt]<0x30 || src[cnt]>0x39) return 0;
		// count deca-position value
		for(cnt1=cnt;(szSrc-(cnt1+1))!=0;cnt1++){
			deca *= 10;	
		}
		deca *=ASCIICHARtoHEX(src[cnt], DEC);
		*dest += deca;
		deca = 1;
	}
	if (src[0]=='-') *dest *= -1;
	return 1;
}

int ASCII5CHARtoUShort
(unsigned short *dest, const unsigned char src[], unsigned char szSrc) 
{	
	unsigned char cnt = 0;
	unsigned char cnt1;	
	long deca;
	ASCIItoLong(&deca, src, szSrc);
	if (deca>65535 || deca<0) {*dest = 0; return -1;}
	if (szSrc>5) return -1;	
	*dest = 0;
	deca = 1; // deca-position value	
	for(;cnt<szSrc;cnt++){
		if (src[cnt]<0x30 || src[cnt]>0x39) return 0;
		// count deca-position value
		for(cnt1=cnt;(szSrc-(cnt1+1))!=0;cnt1++){
			deca *= 10;	
		}		
		deca *= ASCIICHARtoHEX(src[cnt], DEC);	
		*dest += (unsigned short) deca;		
		deca = 1;
	}
	return 1;
}

int ASCII3CHARtoUCHAR
(unsigned char *dest, const unsigned char src[], unsigned char szSrc) 
{	
	unsigned char cnt = 0;
	unsigned char cnt1;	
	unsigned short deca;
	ASCII5CHARtoUShort(&deca, src, szSrc);
	if (deca>255) {*dest = 0; return -1;}
	if (szSrc>3) return -1;	
	*dest = 0;
	deca = 1; // deca-position value	
	for(;cnt<szSrc;cnt++){
		if (src[cnt]<0x30 || src[cnt]>0x39) return 0;
		// count deca-position value
		for(cnt1=cnt;(szSrc-(cnt1+1))!=0;cnt1++){
			deca *= 10;	
		}		
		deca *= ASCIICHARtoHEX(src[cnt], DEC);	
		*dest += (unsigned char) deca;		
		deca = 1;
	}
	return 1;
}

int UCHARtoASCIIDEC(unsigned char *dest, unsigned int szDest, unsigned char src, int option)
{
	unsigned char quotient;	
	unsigned char cnt = 0;
	if (szDest<3) return -1;
	quotient = src/100;
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0)) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%100;
	quotient = src/10;
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}	
	src = src%10;
	quotient = src;	
	dest[cnt] = quotient+0x30;
	cnt++;
	return cnt;
}

int USHORTtoASCIIDEC(unsigned char *dest, unsigned int szDest, unsigned short src, int option)
{
	unsigned char quotient;	
	unsigned char cnt = 0;
	if (szDest<5) return -1;
	quotient = (unsigned char) (src/10000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0)) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%10000;
	quotient = (unsigned char) (src/1000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%1000;
	quotient = (unsigned char) (src/100);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%100;
	quotient = (unsigned char) (src/10);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}	
	src = src%10;
	quotient = (unsigned char) src;	
	dest[cnt] = quotient+0x30;
	cnt++;
	return cnt;
}

int ULONGtoASCIIDEC(unsigned char *dest, unsigned int szDest, unsigned long src, int option)
{
	unsigned char quotient;	
	unsigned char cnt = 0;
	if (szDest<10) return -1;
	quotient = (unsigned char) (src/1000000000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0)) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%1000000000;
	quotient = (unsigned char) (src/100000000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%100000000;
	quotient = (unsigned char) (src/10000000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%10000000;
	quotient = (unsigned char) (src/1000000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%1000000;
	quotient = (unsigned char) (src/100000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%100000;
	quotient = (unsigned char) (src/10000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%10000;
	quotient = (unsigned char) (src/1000);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%1000;
	quotient = (unsigned char) (src/100);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%100;
	quotient = (unsigned char) (src/10);
	if (quotient!=0 || (option==ADD_AHEAD_ZERO && quotient==0) || cnt!=0) {
		dest[cnt] = quotient+0x30;
		cnt++;
	}
	src = src%10;
	quotient = (unsigned char) src;	
	dest[cnt] = quotient+0x30;
	cnt++;
	return cnt;
}

int BuffFlushtoNULL(char *src, int szSrc)
{
	int cnt;
	for(cnt=0;cnt<szSrc;cnt++){
		src[cnt] = NULL;	
	}
	return cnt;
}

int MyStrLen(const char *src)
{
	int cnt;
	for(cnt=0;src[cnt]!=NULL;cnt++)
		;
	return cnt;
}

int MyStrCmp(const char *str1, const char *str2)
{
	int cnt;
	for(cnt=0;(str1[cnt]!=NULL || str2[cnt]!=NULL);cnt++){
		if(str1[cnt]>str2[cnt]) return str1[cnt]-str2[cnt];
		if(str1[cnt]<str2[cnt]) return str1[cnt]-str2[cnt];
	}
	return str1[cnt]-str2[cnt];
}

int MyStrICmp(const char *str1, const char *str2)
{
	int cnt;
	unsigned char buff1;
	unsigned char buff2;
	for(cnt=0;(str1[cnt]!=NULL || str2[cnt]!=NULL);cnt++){
		buff1 = ConverttoUpperCase(str1[cnt]);
		buff2 = ConverttoUpperCase(str2[cnt]);
		if(buff1>buff2) return buff1-buff2;
		if(buff1<buff2) return buff1-buff2;
	}
	buff1 = ConverttoUpperCase(str1[cnt]);
	buff2 = ConverttoUpperCase(str2[cnt]);
	return buff1-buff2;
}

int MyStrNCmp(const char *str1, const char *str2, unsigned int maxLen)
{
	int cnt;
	for(cnt=0;cnt<maxLen;cnt++){		
		if(str1[cnt]>str2[cnt]) return str1[cnt]-str2[cnt];
		if(str1[cnt]<str2[cnt]) return str1[cnt]-str2[cnt];
	}
	return str1[cnt-1]-str2[cnt-1];
}

int MyStrNICmp(const char *str1, const char *str2, unsigned int maxLen)
{
	int cnt;
	unsigned char buff1;
	unsigned char buff2;
	for(cnt=0;cnt<maxLen;cnt++){
		buff1 = ConverttoUpperCase(str1[cnt]);
		buff2 = ConverttoUpperCase(str2[cnt]);
		if(buff1>buff2) return buff1-buff2;
		if(buff1<buff2) return buff1-buff2;
	}
	buff1 = ConverttoUpperCase(str1[cnt-1]);
	buff2 = ConverttoUpperCase(str2[cnt-1]);
	return buff1-buff2;
}

int MyStrCpy(char *dest, int szMaxDest, const char *src)
{
	int cnt;
	for(cnt=0;src[cnt]!=NULL;cnt++){
		if (cnt>=szMaxDest) {
			dest[cnt] = NULL;
			return -1;
		}
		dest[cnt] = src[cnt];		
	}
	dest[cnt] = NULL;
	return cnt;
}

int MyStrNCpy(char *dest, int szMaxDest, const char *src, int szSrc)
{
	int cnt;
	for(cnt=0;cnt<szSrc;cnt++){
		if (cnt>=szMaxDest) {
			dest[cnt] = NULL;
			return -1;
		}
		dest[cnt] = src[cnt];		
	}
	dest[cnt] = NULL;
	return cnt;
}

int MyStrCat(char *dest, int szMaxdest, const char *src)
{
	int cnt;
	int destCnt = MyStrLen(dest);
	int srcCnt = MyStrLen(src);
	for(cnt=0;src[cnt]!=NULL;cnt++){
		if(destCnt>=(szMaxdest-1)) {
			dest[destCnt] = NULL;
			return -1;
		}
		dest[destCnt] = src[cnt];
		destCnt++;
	}
	dest[destCnt] = NULL;
	return destCnt;
}

int MyStrNCat(char *dest, int szMaxdest, const char *src, int szSrc)
{
	int cnt;
	int destCnt = MyStrLen(dest);
	int srcCnt = MyStrLen(src);
	for(cnt=0;cnt<szSrc;cnt++){
		if(destCnt>=(szMaxdest-1)) {
			dest[destCnt] = NULL;
			return -1;
		}
		dest[destCnt] = src[cnt];
		destCnt++;
	}
	dest[destCnt] = NULL;
	return cnt;
}

int MyMemNCpy(char *dest, int szMaxDest, const char *src, int cpyLen)
{
	int cnt;
	for(cnt=0;cnt<cpyLen;cnt++){
		if (cnt>=szMaxDest) return -1;
		dest[cnt] = src[cnt];		
	}	
	return cnt;
}

int IsASCIIHEXValue(char value)
{
	if((value>=0x30 && value<=0x39) || 
	(value>=0x41 && value<=0x46) ||
	(value>=0x61 && value<=0x66)) return 1;
	
	return 0;
}

int AtoHstr(char *dest, char *src, int dataCount)
{
	int cnt;
	int cnt1 = 0;
	char oneChar[4];
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

int ASCIIHEXStrtoHEXArray(unsigned char *dest, int szMaxDest, const char *src)
{
	int cnt;
	int cnt1 = 0;
	for(cnt=0;src[cnt]!=NULL;cnt+=2){
		if (cnt1>=szMaxDest) return -1;
		while(IsASCIIHEXValue(src[cnt])==0){
			if(src[cnt]==NULL) return -1;
			cnt++;
		}
		dest[cnt1] = ASCII2CHARtoHEX(src[cnt], src[cnt+1], HEX);
		cnt1++;
	}
	return cnt1;
}

int HEXArraytoASCIIHEXStr(unsigned char *dest, int szMaxDest, const unsigned char *src, unsigned int szSrc)
{
	int cnt;
	int cnt1 = 0;
	for(cnt=0;cnt<szSrc;cnt++){
		if (cnt1>=szMaxDest) return -1;
		HEXtoASCIIHighLow(&dest[cnt1], &dest[cnt1+1], src[cnt], UPPERCASE);		
		cnt1+=2;
	}
	return cnt1;
}

char GetArrXOR(const char *src, int szSrc)
{
	int cnt;
	char retValue = 0;
	for(cnt=0;cnt<szSrc;cnt++){
		retValue ^= src[cnt];
	}
	return retValue;
}

unsigned char GetArrSum(const unsigned char *src, int szSrc)
{
	int cnt;
	unsigned char retValue = 0;
	for(cnt=0;cnt<szSrc;cnt++){
		retValue += src[cnt];
	}
	return retValue;	
}
