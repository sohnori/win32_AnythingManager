#ifndef __INVERTER_WILL_H__
#define __INVERTER_WILL_H__

// RS 485(hexASCII) default baudRate 9600
// 8bits data, 1stop bits, no parity, max 32 device connect
// max cable length 1200 meter, parallel connect, ID 1~99
// request - ENQ(1) ID(2) CMD(1) cmdType(2) variableLength(2) variableName(7) dataCount(2) EOT(1) BCC(1)
// response - ACK(1) ID(2) CMD(1) cmdType(2) blockCount(2) dataCount(2) data(n) ETX(1) BCC(1)
// cmd - ASCII 'r'_read, 'w'_write
// BCC - ASCII sum ENQ or ACK ~ EOT or ETX and get low byte

// inverter type
#define WILL_TYPE_ONE		0x01
#define WILL_TYPE_M		0x02
#define WILL_TYPE_SL		0x03
#define WILL_TYPE_T		0x04

#define WILL_VALUE_PVST		0x10
#define WILL_VALUE_INVST	0x11
#define WILL_VALUE_GRIDST	0x12
#define WILL_VALUE_CONVST	0x13
#define WILL_VALUE_PVV		0x14
#define WILL_VALUE_PVI		0x15
#define WILL_VALUE_PVP		0x16
#define WILL_VALUE_RV		0x17
#define WILL_VALUE_SV		0x18
#define WILL_VALUE_TV		0x19
#define WILL_VALUE_RI		0x1A
#define WILL_VALUE_SI		0x1B
#define WILL_VALUE_TI		0x1C
#define WILL_VALUE_FR		0x1D
#define WILL_VALUE_PF		0x1E
#define WILL_VALUE_GRIDP	0x1F
#define WILL_VALUE_TOTALP	0x20
#define WILL_VALUE_TODAYP	0x21

#define WILL_CMD_READ		"rSB"
#define WILL_CMD_WRITE		"wSB"

#define WILL_ADDR_PVST		"%MW0000" // PV status
#define WILL_ADDR_INVST		"%MW0001" // inverter status
#define WILL_ADDR_GRIDST	"%MW0002" // grid status
#define WILL_ADDR_CONVST	"%MW0003" // converter status
#define WILL_ADDR_PVV		"%MW0032"
#define WILL_ADDR_PVI		"%MW0033"
#define WILL_ADDR_PVP		"%MW0034"
#define WILL_ADDR_GRIDRV	"%MW0064"
#define WILL_ADDR_GRIDSV	"%MW0065"
#define WILL_ADDR_GRIDTV	"%MW0066"
#define WILL_ADDR_GRIDRI	"%MW0067"
#define WILL_ADDR_GRIDSI	"%MW0068"
#define WILL_ADDR_GRIDTI	"%MW0069"
#define WILL_ADDR_GRIDFR	"%MW0070"
#define WILL_ADDR_GRIDPF	"%MW0071"
#define WILL_ADDR_GRIDPW	"%MW0072"
#define WILL_ADDR_TOTALP	"%MW0073"
#define WILL_ADDR_TODAYP	"%MW0077"

// error code
#define WILL_ERROR_PACKET	(-2)
#define WILL_ERROR_CMD		(-3)
#define WILL_ERROR_ID		(-4)
#define WILL_ERROR_BCC		(-5)

int GetInvWILLRequest
(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char *cmd, unsigned char *addr, unsigned char length);

int VerifyInvWILLResponse(const unsigned char *src,  unsigned char ID);

int GetInvWILLValue(unsigned long *dest, const unsigned char *src, char sort);

#endif