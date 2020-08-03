#ifndef __INVERTER_EKOS_H__
#define __INVERTER_EKOS_H__

// RS 422 or 485 communication baudrate 9600
// 8bits data, 1stop bits, no parity, hex code, max 32 device connect
// max cable length 1200 meter, parallel connect, ID 1~99
// request - ID(1) FuntionCode(1) ParameterAddress(2) RequestParameterAddrCounter(2) CRC16_MODBUS(2)
// response - ID(1) FuntionCode(1) ResponseDataByteCounter(1) Data(n) CRC16_MODBUS(2)

#define EKOS_FC_03		0x03 // set communication
#define EKOS_FC_04		0x04 // read mesurement data
#define EKOS_FC_05		0x05 // control generation start or stop
#define EKOS_FC_06		0x06 // set generation start delay time
#define EKOS_FC_10		0x10 // set time and configuration

#define EKOS_ADDR_GENST		1000 // start generation
#define EKOS_ADDR_GENSP		1001 // stop generation

// define parameter address 
#define EKOS_ADDR_MODELN	0 // 0~7 range, ASCII 16 character
#define EKOS_ADDR_SERIALN	8 // 8~15 range, ASCII 16 character
#define EKOS_ADDR_INVVER	16 // inverter version
#define EKOS_ADDR_INVPOWER	17
#define EKOS_ADDR_TIMEYEAR	18 // now time
#define EKOS_ADDR_TIMEMONTH	19 // now time
#define EKOS_ADDR_TIMEDAY	20 // now time
#define EKOS_ADDR_TIMEHOUR	21 // now time
#define EKOS_ADDR_TIMEMINUTE	22 // now time
#define EKOS_ADDR_TIMESECOND	23 // now time
#define EKOS_ADDR_TIMETOTALGEN	32 // 32~33 total generation time

#define EKOS_ADDR_PVV		34 // 34~35
#define EKOS_ADDR_PVI		38 // 38~39
#define EKOS_ADDR_PVP		40 // 40~41 W
#define EKOS_ADDR_LINEV		42 // 42~43
#define EKOS_ADDR_LINEI		44 // 44~45
#define EKOS_ADDR_LINEP		46 // 46~47
#define EKOS_ADDR_PF		50 // 50~51
#define EKOS_ADDR_FR		52 // 52~53
#define EKOS_ADDR_TODAYP	54 // 54~55 Wh
#define EKOS_ADDR_LINERV	58 // 58~59
#define EKOS_ADDR_LINESV	60 // 60~61
#define EKOS_ADDR_LINETV	62 // 62~63
#define EKOS_ADDR_LINERI	64 // 64~65
#define EKOS_ADDR_LINESI	66 // 66~67
#define EKOS_ADDR_LINETI	68 // 68~69
#define EKOS_ADDR_TOTALP	94 // 94~97

#define EKOS_ADDR_PVSTATUS	98
#define EKOS_ADDR_PV1STATUS	99
#define EKOS_ADDR_PV2STATUS	100
#define EKOS_ADDR_INVSTATUS	101
#define EKOS_ADDR_GRIDSTATUS	102
#define EKOS_ADDR_CONVERTSTATUS	103

// error code
#define EKOS_ERROR_PACKET	(-2)
#define EKOS_ERROR_ID		(-3)
#define EKOS_ERROR_CRC16	(-4)

int GetInvEKOSRequest
(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char FC, unsigned char addr, unsigned short length);

int VerifyInvEKOSResponse(const unsigned char *src,  unsigned char ID, unsigned char FC);

int GetInvEKOSValue(unsigned long *dest, const unsigned char *src, char sort);

#endif