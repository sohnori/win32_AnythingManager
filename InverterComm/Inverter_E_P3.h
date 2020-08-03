#ifndef __INVERTER_E_P3_H__
#define __INVERTER_E_P3_H__

// RS485(hex) default baudRate 9600
// 8bits data, 1stop bits, no parity
// packet form - header1 header2 stationID command length tail cksum(2~4bytes)
// ex) request - 0x0a 0x96 0x01 0x54 0x18 0x05 0x6D	cksum = stationID+command+length
// 	response - 0xB1 0xB7 0x01 data~~ cksum(31th)	cksum(XOR_0~30)	

#define E_P3_VALUE_PVV1		0x01 // PV voltage1
#define E_P3_VALUE_PVI		0x02 // PV current
#define E_P3_VALUE_PVV2		0x03 // PV voltage1
#define E_P3_VALUE_OUTPUTV	0x04
#define E_P3_VALUE_OUTPUTI	0x05
#define E_P3_VALUE_TEMPERATURE	0x06
#define E_P3_VALUE_TODAYP	0x07 // energy today
#define E_P3_VALUE_TOTALP	0x08
#define E_P3_VALUE_FAULT	0x09
#define E_P3_VALUE_STATUS	0x0A
#define E_P3_VALUE_FREQUENCY	0x0B
#define E_P3_VALUE_OPERTIME	0x0C
#define E_P3_VALUE_POWERFACTOR	0x0D
#define E_P3_VALUE_DSPVERSION	0x0E
#define E_P3_VALUE_ID		0x0F
#define E_P3_VALUE_INPUTP	0x10
#define E_P3_VALUE_OUTPUTP	0x11

// error code
#define E_P3_ERROR_PACKET	(-2)
#define E_P3_ERROR_ID		(-3)
#define E_P3_ERROR_CKSUM	(-4)

int GetInvE_P3Request(unsigned char *dest, unsigned int szDest, unsigned char ID);

int VerifyInvE_P3Response(const unsigned char *src, unsigned char ID);

int GetInvE_P3Value(unsigned long *dest, const unsigned char *src, char sort);

#endif