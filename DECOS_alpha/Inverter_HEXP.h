#ifndef __INVERTER_HEXP_H__
#define __INVERTER_HEXP_H__

// RS 485(hexASCII) default baudRate 9600
// 8bits data, 1stop bits, no parity, max 32 device connect
// max cable length 1200 meter, parallel connect, ID 0~31(0x00~0x1f)
// request - ENQ(1) ID(2) CMD(1) parameter-addr(4) data-count(2) cksum(4) EOT(1)
// response - ACK(1) ID(2) CMD(1) parameter-addr(4) data-count(4*n) cksum(4) EOT(1)

#define HEXP_TYPE_ONE		0x01
#define HEXP_TYPE_THREE		0x03
#define HEXP_TYPE_H30XXS_ML	0x48

#define HEXP_VALUE_SOLARFAULT	0x10
#define HEXP_VALUE_INVFAULT	0x11
#define HEXP_VALUE_INVSTATE	0x12
#define HEXP_VALUE_INVOPERFAULT	0x13
#define HEXP_VALUE_SOLARV	0x14
#define HEXP_VALUE_SOLARI	0x15
#define HEXP_VALUE_LINERSV	0x16
#define HEXP_VALUE_LINESTV	0x17
#define HEXP_VALUE_LINETRV	0x18
#define HEXP_VALUE_LINERI	0x19
#define HEXP_VALUE_LINESI	0x1A
#define HEXP_VALUE_LINETI	0x1B
#define HEXP_VALUE_LINEFR	0x1C
#define HEXP_VALUE_SOLARP	0x1D
#define HEXP_VALUE_TOTALP	0x1E
#define HEXP_VALUE_LINEP	0x1F
#define HEXP_VALUE_PEAK		0x20
#define HEXP_VALUE_TODAYP	0x21
#define HEXP_VALUE_PF		0x22 // power factor
#define HEXP_VALUE_TILT		0x23 
#define HEXP_VALUE_HORIZON	0x24 
#define HEXP_VALUE_OUTDOORT	0x25 // outdoor temperature
#define HEXP_VALUE_MODULET	0x26 // module temperature

#define HEXP_INFO_PHASE		0x27
#define HEXP_INFO_POWER		0x28
#define HEXP_INFO_PTIME		0x29 // product time
#define HEXP_INFO_SERIALN	0x2A // product time

#define HEXP_PARA_FAULT		0x04 // parameter - 0x0004
#define HEXP_PARA_PV		0x20 // 0x0020
#define HEXP_PARA_GRID		0x50 // 0x0050
#define HEXP_PARA_POWER		0x60 // 0x0060
#define HEXP_PARA_SYSTEM	0xe0 // 0x01e0
#define HEXP_PARA_ENVIRON	0x70 // 0x0070

// error code
#define HEXP_ERROR_PACKET	(-2)
#define HEXP_ERROR_ID		(-3)
#define HEXP_ERROR_CKSUM	(-4)
#define HEXP_ERROR_PARAMETER	(-5)

int GetInvHEXPRequest(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char parameter);

int VerifyInvHEXPResponse(const unsigned char *src, unsigned char ID, unsigned char parameter);

int GetInvHEXPValue(unsigned long *dest, const unsigned char *src, char sort, char invType);

#endif