#ifndef __CONVERTDATA_H__
#define __CONVERTDATA_H__

/*************************/
/*     Public define 	 */
/*************************/

#define BIN	2
#define DEC	10
#define HEX	16

#define UPPERCASE	85
#define LOWERCASE	76

#define ADD_AHEAD_ZERO	1

/**************************/
/*     Public Function    */
/**************************/

unsigned char ConvertASCIICase(unsigned char value);

unsigned char ConverttoUpperCase(unsigned char value);

unsigned char ConverttoLowerCase(unsigned char value);

char ASCIICHARtoHEX(unsigned char value, int option);

unsigned char ASCII2CHARtoHEX(unsigned char nibbleH, unsigned char nibbleL, int option);

unsigned char ASCIISHORTtoHEX(unsigned short src, int option);

char HEXtoASCIICHAR(unsigned char src, int option);

short HEXtoASCII2Byte(unsigned char src, int option);

unsigned char HtoA2(char value, char *retHexHigh, char *retHexLow);

int HEXtoASCIIHighLow(char *destHigh, char *destLow, unsigned char src, int option);

int ASCIItoLong(long *dest, const unsigned char src[], unsigned char szSrc);

int ASCII5CHARtoUShort
(unsigned short *dest, const unsigned char src[], unsigned char szSrc);

int ASCII3CHARtoUCHAR
(unsigned char *dest, const unsigned char src[], unsigned char szSrc);

int UCHARtoASCIIDEC(unsigned char *dest, unsigned int szDest, unsigned char src, int option);

int USHORTtoASCIIDEC(unsigned char *dest, unsigned int szDest, unsigned short src, int option);

int ULONGtoASCIIDEC(unsigned char *dest, unsigned int szDest, unsigned long src, int option);

int BuffFlushtoNULL(char *src, int szSrc);

int MyStrLen(const char *src);

int MyStrCmp(const char *str1, const char *str2);

int MyStrICmp(const char *str1, const char *str2);

int MyStrNCmp(const char *str1, const char *str2, unsigned int maxLen);

int MyStrNICmp(const char *str1, const char *str2, unsigned int maxLen);

int MyStrCpy(char *dest, int szMaxDest, const char *src);

int MyStrNCpy(char *dest, int szMaxDest, const char *src, int szSrc);

int MyStrCat(char *dest, int szMaxdest, const char *src);

int MyStrNCat(char *dest, int szMaxdest, const char *src, int szSrc);

int MyMemNCpy(char *dest, int szMaxDest, const char *src, int cpyLen);

int IsASCIIHEXValue(char value);

int AtoHstr(char *dest, char *src, int dataCount);

int ASCIIHEXStrtoHEXArray(unsigned char *dest, int szMaxDest, const char *src);

int HEXArraytoASCIIHEXStr(unsigned char *dest, int szMaxDest, const unsigned char *src, unsigned int szSrc);

char GetArrXOR(const char *src, int szSrc);

unsigned char GetArrSum(const unsigned char *src, int szSrc);

#endif