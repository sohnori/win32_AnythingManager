#ifndef __INVERTER_SUNG_H__
#define __INVERTER_SUNG_H__

// RS 422 or 485 communication baudrate 9600
// 8bits data, 1stop bits, no parity, hex code, max 32 device connect
// max cable length 1200 meter, parallel connect, ID 1~247
// ethernet IP:192.168.1.100 sub-net:255.255.0.0 port:502

/*data type
U16: 16-bit unsigned integer, big-endian
S16: 16-bit signed integer, big-endian
U32: 32-bit unsigned integer; little-endian for double-word data. Big-endian for byte data
S32: 32-bit signed integer; little-endian for double-word data. Big-endian for byte data
Example:
transmission order of U16 data 0x0102 is 01, 02
transmission order of U32 data 0x01020304 is 03, 04, 01, 02
The transmission order of multibyte data UTF-8: the high-byte data is in the front and the low-byte data is at
back.
Example: transmission order of UTF-8 data ABCD is A, B, C, D. 
*/

/*
Address type
Address of 3x type is read-only register, supporting the cmdcode inquiry of 0x04(function code).
Address of 4x type is holding register, supporting the cmdcode inquiry of 0x03(function code), and cmdcodeswrite-in of 0x10
and 0x06. Cmdcodes 0x10 and 0x06 support the broadcast address.
Support Modbus error code 02 (address error), 04 (setting failure).
Visit all registers by subtracting 1 from the register address. Example: if the address is 5000 ?5001, visit it using
address 4999 ?5000. Entering ¡°01 04 1387 00 02 + CRC¡± to check the data of address 5000 ?5001.
*/

// Function code
#define SUNG_FUNCTION_CODE_READONLY		0x04
#define SUNG_FUNCTION_CODE_HOLDING		0x03

// Running information variable address definition (read-only register, Address type: 3X)
#define SUNG_ADDR_SERIALN		4990 // 4990~4999
#define SUNG_ADDR_DEVTYPE		5000
#define SUNG_ADDR_NOMINALP		5001
#define SUNG_ADDR_OUTPUTTYPE	5002
#define SUNG_ADDR_DAILYP		5003
#define SUNG_ADDR_TOTALP		5004 // 5004~5005
#define SUNG_ADDR_RUNNINGTIME	5006 // 5006~5007
#define SUNG_ADDR_INTERTEMP		5008
#define SUNG_ADDR_DCV1			5011
#define SUNG_ADDR_DCI1			5012
#define SUNG_ADDR_DCV2			5013
#define SUNG_ADDR_DCI2			5014
#define SUNG_ADDR_DCV3			5015
#define SUNG_ADDR_DCI3			5016
#define SUNG_ADDR_DCV4			5115
#define SUNG_ADDR_DCI4			5116
#define SUNG_ADDR_DCV5			5117
#define SUNG_ADDR_DCI5			5118
#define SUNG_ADDR_DCP			5017 // 5017~5018
#define SUNG_ADDR_ABV			5019
#define SUNG_ADDR_BCV			5020
#define SUNG_ADDR_CAV			5021
#define SUNG_ADDR_AI			5022
#define SUNG_ADDR_BI			5023
#define SUNG_ADDR_CI			5024
#define SUNG_ADDR_GRIDP			5031 // 5031~5032
#define SUNG_ADDR_PF			5035 // power factor
#define SUNG_ADDR_FR			5036 // frequency
#define SUNG_ADDR_WORKSTATE		5038 // work state
#define SUNG_ADDR_WORKSTATE2		5081 // 5081~5082

// value
#define SUNG_VALUE_SERIALN		0x01 // 
#define SUNG_VALUE_DEVTYPE		0x02
#define SUNG_VALUE_NOMINALP		0x03
#define SUNG_VALUE_OUTPUTTYPE		0x04
#define SUNG_VALUE_DAILYP		0x05
#define SUNG_VALUE_TOTALP		0x06 // 
#define SUNG_VALUE_RUNNINGTIME		0x07 // 
#define SUNG_VALUE_INTERTEMP		0x08

#define SUNG_VALUE_DCV1			0x0A
#define SUNG_VALUE_DCI1			0x0B
#define SUNG_VALUE_DCV2			0x0C
#define SUNG_VALUE_DCI2			0x0D
#define SUNG_VALUE_DCV3			0x0E
#define SUNG_VALUE_DCI3			0x0F
#define SUNG_VALUE_DCV4			0x10
#define SUNG_VALUE_DCI4			0x11
#define SUNG_VALUE_DCV5			0x12
#define SUNG_VALUE_DCI5			0x13
#define SUNG_VALUE_DCP			0x14 // 
#define SUNG_VALUE_ABV			0x15
#define SUNG_VALUE_BCV			0x16
#define SUNG_VALUE_CAV			0x17
#define SUNG_VALUE_AI			0x18
#define SUNG_VALUE_BI			0x19
#define SUNG_VALUE_CI			0x1A
#define SUNG_VALUE_GRIDP		0x1B // 
#define SUNG_VALUE_PF			0x1C // power factor
#define SUNG_VALUE_FR			0x1D // frequency
#define SUNG_VALUE_WORKSTATE		0x1E // work state
#define SUNG_VALUE_WORKSTATE2		0x1F // 

// error code
#define SUNG_ERROR_PACKET	(-2)
#define SUNG_ERROR_ID		(-3)
#define SUNG_ERROR_CRC16	(-4)

int GetInvSUNGRequest
(unsigned char *dest, unsigned int szDest, unsigned char invID, unsigned char functionCode, unsigned short addr, unsigned short option);

int VerifyInvSUNGResponse(const unsigned char *src, unsigned char invID, unsigned char functionCode);

int GetInvSUNGValue(unsigned long *dest, const unsigned char *src, char sort);

int GetInvSUNGValueStr(unsigned char *dest, unsigned int szDest, const unsigned char *src, char sort);

#endif