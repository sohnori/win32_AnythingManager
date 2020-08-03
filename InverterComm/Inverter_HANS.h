#ifndef __INVERTER_HANS_H__
#define __INVERTER_HANS_H__

// RS 485(hex) default baudRate 19200
// 8bits data, 1stop bits, no parity, hex code, max 32 device connect
// max cable length 1200 meter, parallel connect, ID 1~99
// packet - STX(1) length(1) FrameNumber(1) Option(2) DestID(1) SelfID(1) Data(n) Cksum(2) ETX(1)
// STX_0x02(fixed) length_STX ~ ETX, FrameNumber_0(general), Option_0(reservation), Cksum_sum(DestID~endofdata)
// ETX_0x03(fixed)

#define HANS_PHASE_ONE		0x01
#define HANS_PHASE_THREE	0x02

#define HANS_VALUE_OUTPUTP	0x03 // 
#define HANS_VALUE_TODAYP	0x04 // 
#define HANS_VALUE_TOTALP	0x05 // 
#define HANS_VALUE_PVV1		0x06 // 
#define HANS_VALUE_PVI1		0x07 // 
#define HANS_VALUE_PVV2		0x08 // 
#define HANS_VALUE_PVI2		0x09 // 
#define HANS_VALUE_OUTPUTRV	0x0A // 
#define HANS_VALUE_OUTPUTRI	0x0B // 
#define HANS_VALUE_OUTPUTSV	0x0C // 
#define HANS_VALUE_OUTPUTSI	0x0D // 
#define HANS_VALUE_OUTPUTTV	0x0E // 
#define HANS_VALUE_OUTPUTTI	0x0F // 
#define HANS_VALUE_FAULTHW	0x10 // 
#define HANS_VALUE_FAULTSW	0x11 //
#define HANS_VALUE_PHASE	0x12 //
#define HANS_VALUE_ID		0x13 //
#define HANS_VALUE_PVP		0x14 //
#define HANS_VALUE_STATUS	0x15 //

// error code
#define HANS_ERROR_PACKET	(-2)
#define HANS_ERROR_ID		(-3)
#define HANS_ERROR_CKSUM	(-4)

int GetInvHANSRequest(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char phase);

int VerifyInvHANSResponse(const unsigned char *src, unsigned char ID);

int GetInvHANSValue(unsigned long *dest, const unsigned char *src, char sort);

#endif