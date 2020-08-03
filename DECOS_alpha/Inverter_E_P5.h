#ifndef __INVERTER_E_P5_H__
#define __INVERTER_E_P5_H__

// RS485(hex) default baudRate 19200
// 8bits data, 1stop bits, no parity, hex code, max 32 device connect
// max cable length 1200 meter, parallel connect, ID 1~99
// packet form - header1 header2 stationID command don't_care tail cksum(2~4bytes)
// ex) request - 0x0a 0x96 0x01 0x54 0x18 0x05 0x6D	cksum = stationID+command+length
// 	response - 0xB1 0xB7 0x01 data~~(3~38) cksum(39th)	cksum(XOR_0~38)	

#define E_P5_VALUE_PVV1		0x01 // PV voltage1
#define E_P5_VALUE_PVI1		0x02 // PV current1
#define E_P5_VALUE_PVP1		0x03 // PV power1
#define E_P5_VALUE_PVV2		0x04 // PV voltage2
#define E_P5_VALUE_PVI2		0x05 // PV current2
#define E_P5_VALUE_PVP2		0x06 // PV power2
#define E_P5_VALUE_OUTPUTV	0x07
#define E_P5_VALUE_OUTPUTI	0x08
#define E_P5_VALUE_OUTPUTP	0x09
#define E_P5_VALUE_FREQUENCY	0x0A
#define E_P5_VALUE_TOTALP	0x0B
#define E_P5_VALUE_TODAYP	0x0C // energy today
#define E_P5_VALUE_TIME		0x0D
#define E_P5_VALUE_INVSTATUS	0x0E
#define E_P5_VALUE_GRIDFAULT	0x0F
#define E_P5_VALUE_FAULT1	0x10
#define E_P5_VALUE_FAULT2	0x11
#define E_P5_VALUE_WARRING	0x12
#define E_P5_VALUE_ID		0x13

// error code
#define E_P5_ERROR_PACKET	(-2)
#define E_P5_ERROR_ID		(-3)
#define E_P5_ERROR_CKSUM	(-4)

int GetInvE_P5Request(unsigned char *dest, unsigned int szDest, unsigned char ID);

int VerifyInvE_P5Response(const unsigned char *src, unsigned char ID);

int GetInvE_P5Value(unsigned long *dest, const unsigned char *src, char sort);

#endif