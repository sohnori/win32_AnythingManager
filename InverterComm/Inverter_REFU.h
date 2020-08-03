#ifndef __INVERTER_REFU_H__
#define __INVERTER_REFU_H__

// RS 485(hex) default baudRate 115200
// 8bits data, 1stop bits, even parity, max 32 device connect
// max cable length 1200 meter, parallel connect, inverter ID 0~31

// packet form - 
// STX(1) dataLength(1_InvID~beforeBCC) telegram(3bit)+ID(5bit) PKE(AK_4bit+reserved_1bit+parameterAddr_11bit) 
// IND(index word) PZDarea BCC_cksum(all XOR)
// ex)PV V_1104
// request - 02 16 01 14 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 51
// response - 02 16 01 54 50 00 00 43 C3 C9 BE 00 07 00 00 01 1C 08 86 01 8B 00 29 D1

// telegram(3bit) - 
// bit7_special telegram - send special message to a slave. telegram for RTP protocol
// bit6_mirror telegram - if this set and send, slave return mirroring data. can be use line test.
// bit5_broadcast - all slave response

// PKE(AK_4bit+reserved_1bit+parameterAddr_11bit) - parameter ID

// AK task ID - 
// 0000 no task
// 0001 request PWE parameter value(16 or 32bit) - request read correspoding parameter value
// 0010 change PWE WORD parameter(16bit) - request to write value
// 0011 change PWE DWORD parameter(32bit) - request to write value
// 0110 request PWE array2 - request read correspoding array. array classify to IND value
// 0111 change PWE array word(16bit) - request to write value to array
// 1000 change PWE array Dword(32bit) - request to write value to array
// 1001 request the number of array elements

// AK reply ID - 
// 0000 no task
// 0001 transfer PWE word - response parameter value(16bit)
// 0010 transfer PWE word - response parameter value(32bit)
// 0100 transfer PWE array word
// 0101 transfer PWE array Dword
// 0110 transfer the number of array elements
// 0111 task cannot be executed
// 1000 no control/change rights for PKW interface

// IND index word - 
// high byte_not used, low byte_0~254 used with AK(0110,0111,1000)
// if low byte set to 0, read first parameter value of array
// if low byte set to 1, read second parameter value of array

// PZD process data - 
// from master to slave : PZD1_control word PZD2_main setting value PZD3~6_to add setting value
// response slave : PZD1_status or main value or real value

// PNU IND_0:phaseL1 1: phaseL2 2:phaseL3
#define REFU_PARAMETER_FWVER		0
#define REFU_PARAMETER_DEVTYPE		20
#define REFU_PARAMETER_SERIALN		43
#define REFU_PARAMETER_INTERTEMP	92
#define REFU_PARAMETER_ERRORCODE	500
#define REFU_PARAMETER_OPSTATE		501
#define REFU_PARAMETER_PHASEUACV	1102 // unfiltered AC voltage
#define REFU_PARAMETER_ACTUALACI	1103
#define REFU_PARAMETER_SOLARV		1104
#define REFU_PARAMETER_SOLARI		1105
#define REFU_PARAMETER_LINEPOWER	1106
#define REFU_PARAMETER_DCPOWER		1107
#define REFU_PARAMETER_PHASEACV		1121
#define REFU_PARAMETER_FREQUENCY	1122
#define REFU_PARAMETER_ACI		1141
#define REFU_PARAMETER_DAILYENERGE	1150 // IND_0: todat 1:yesterday
#define REFU_PARAMETER_TOTALP		1151

// value
#define REFU_VALUE_STATUS		0x01
#define REFU_VALUE_STATUSP		0x02
#define REFU_VALUE_PVV			0x03
#define REFU_VALUE_PVI			0x04
#define REFU_VALUE_PVP			0x05
#define REFU_VALUE_GRIDRV		0x06
#define REFU_VALUE_GRIDSV		0x07
#define REFU_VALUE_GRIDTV		0x08
#define REFU_VALUE_GRIDRI		0x09
#define REFU_VALUE_GRIDSI		0x0A
#define REFU_VALUE_GRIDTI		0x0B
#define REFU_VALUE_GRIDP		0x0C
#define REFU_VALUE_DAILYP		0x0D
#define REFU_VALUE_TOTALP		0x0F

// AK define
#define REFU_AK_REQUEST_PWEVALUE	0b00010000 // corresponding value(16 or 32bit)
#define REFU_AK_CHANGE_PWEWORD		0b00100000 // 16 bit
#define REFU_AK_CHANGE_PWEDWORD		0b00110000 // 32 bit
#define REFU_AK_REQUEST_PWEARRAY2	0b01100000
#define REFU_AK_CHANGE_PWEARRAYWORD	0b01110000
#define REFU_AK_CHANGE_PWEARRAYDWORD	0b10000000
#define REFU_AK_REQUEST_NUMBEROFARRAY	0b10010000 // request the number of array elements


// fault code
#define REFU_FAULT_ERROR_OK		0x00000000
#define REFU_FAULT_GRID_OV		0x00090006
#define REFU_FAULT_GRID_UV		0x00090007
#define REFU_FAULT_OVER_F		0x00090008
#define REFU_FAULT_UNDER_F		0x00090009

// error code
#define REFU_ERROR_PACKET	(-2)
#define REFU_ERROR_CMD		(-3)
#define REFU_ERROR_ID		(-4)
#define REFU_ERROR_BCC		(-5)

int GetInvREFURequest
(unsigned char *dest, unsigned int szDest, unsigned char ID, char AK, unsigned short parameter, short IND);

int VerifyInvREFUResponse(const unsigned char *src, unsigned char ID, unsigned short parameter);

int GetInvREFUValue(unsigned long *dest, const unsigned char *src, char sort);

#endif