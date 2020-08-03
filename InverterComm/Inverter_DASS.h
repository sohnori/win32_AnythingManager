#ifndef __INVERTER_DASS_H__
#define __INVERTER_DASS_H__

// RS485(ASCII) default baudRate 9600
// 8bits data, 1stop bits, no parity
// packet form
// request - SOP code ID option1 option2 address
// response - SOP(1byte) code(1) address(1) length(2) ID(3) data(~20) CKSUM(2)
// ex) 	^P001ST1
//	^D120001,400,0200,0080,18
//	cksum = 1+2+0+0+0+1+4+0+0+0+2+0+0+0+0+8+0

#define DASS_CMD_MOD		0x01 // default infomation
#define DASS_CMD_ST1		0x02 // PV value
#define DASS_CMD_ST2		0x03 // grid voltage and frequency
#define DASS_CMD_ST3		0x04 // grid current
#define DASS_CMD_ST4		0x05 // inverter power
#define DASS_CMD_ST5		0x06 // sensorbox ,if it is
#define DASS_CMD_ST6		0x07 // inverter status

#define DASS_INFO_ID		0x0F
#define DASS_INFO_PHASE		0x10
#define DASS_INFO_CAPACITY	0x11
#define DASS_INFO_GRIDV		0x12 // grid voltage

#define DASS_VALUE_PVV		0x13 // PV voltage
#define DASS_VALUE_PVI		0x14 // PV current
#define DASS_VALUE_PVP		0x15 // PV power

#define DASS_VALUE_GRIDRSV	0x16 // grid R-S line to line voltage
#define DASS_VALUE_GRIDSTV	0x17 // grid S-T line to line voltage
#define DASS_VALUE_GRIDTRV	0x18 // grid T-R line to line voltage
#define DASS_VALUE_GRIDFR	0x19 // grid frequency

#define DASS_VALUE_GRIDRI	0x1A // grid R phase current
#define DASS_VALUE_GRIDSI	0x1B // grid S phase current
#define DASS_VALUE_GRIDTI	0x1C // grid T phase current

#define DASS_VALUE_GENP		0x1D // grid generated power
#define DASS_VALUE_TOTALP	0x1E // total generated power

#define DASS_VALUE_GRIDSTATUS	0x1F
#define DASS_VALUE_STATUS	0x20
#define DASS_VALUE_FAULT	0x21

#define DASS_VALUE_HORIZON	0x22 // horizon solar radiation quantity
#define DASS_VALUE_TILT		0x23 // tilt solar radiation quantity
#define DASS_VALUE_OUTDOOR	0x24 // ourdoor temperature
#define DASS_VALUE_MODULE	0x25 // PV module temperature

// error code
#define DASS_ERROR_PACKET	(-2)
#define DASS_ERROR_ID		(-3)
#define DASS_ERROR_CKSUM	(-4)

int GetInvDASSRequest(char *dest, unsigned int szDest, char srcID[], unsigned char srcCmd);

int VerifyInvDASSResponse(const unsigned char *src, char srcID[], unsigned char srcCmd);

int GetInvDASSValue(unsigned long *dest, const unsigned char *src, char sort);

int GetInvDASSValueStr(unsigned char *dest, const unsigned char *src, char sort);

#endif