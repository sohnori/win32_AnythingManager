#pragma once
#ifndef __CRC_H__
#define __CRC_H__
/************************/
/*		CRC define		*/
/************************/

/*
CRC16_ARC         = Params{0x8005, 0x0000, true, true, 0x0000, 0xBB3D, "CRC-16/ARC"}
CRC16_AUG_CCITT   = Params{0x1021, 0x1D0F, false, false, 0x0000, 0xE5CC, "CRC-16/AUG-CCITT"}
CRC16_BUYPASS     = Params{0x8005, 0x0000, false, false, 0x0000, 0xFEE8, "CRC-16/BUYPASS"}
CRC16_CCITT_FALSE = Params{0x1021, 0xFFFF, false, false, 0x0000, 0x29B1, "CRC-16/CCITT-FALSE"}
CRC16_CDMA2000    = Params{0xC867, 0xFFFF, false, false, 0x0000, 0x4C06, "CRC-16/CDMA2000"}
CRC16_DDS_110     = Params{0x8005, 0x800D, false, false, 0x0000, 0x9ECF, "CRC-16/DDS-110"}
CRC16_DECT_R      = Params{0x0589, 0x0000, false, false, 0x0001, 0x007E, "CRC-16/DECT-R"}
CRC16_DECT_X      = Params{0x0589, 0x0000, false, false, 0x0000, 0x007F, "CRC-16/DECT-X"}
CRC16_DNP         = Params{0x3D65, 0x0000, true, true, 0xFFFF, 0xEA82, "CRC-16/DNP"}
CRC16_EN_13757    = Params{0x3D65, 0x0000, false, false, 0xFFFF, 0xC2B7, "CRC-16/EN-13757"}
CRC16_GENIBUS     = Params{0x1021, 0xFFFF, false, false, 0xFFFF, 0xD64E, "CRC-16/GENIBUS"}
CRC16_MAXIM       = Params{0x8005, 0x0000, true, true, 0xFFFF, 0x44C2, "CRC-16/MAXIM"}
CRC16_MCRF4XX     = Params{0x1021, 0xFFFF, true, true, 0x0000, 0x6F91, "CRC-16/MCRF4XX"}
CRC16_RIELLO      = Params{0x1021, 0xB2AA, true, true, 0x0000, 0x63D0, "CRC-16/RIELLO"}
CRC16_T10_DIF     = Params{0x8BB7, 0x0000, false, false, 0x0000, 0xD0DB, "CRC-16/T10-DIF"}
CRC16_TELEDISK    = Params{0xA097, 0x0000, false, false, 0x0000, 0x0FB3, "CRC-16/TELEDISK"}
CRC16_TMS37157    = Params{0x1021, 0x89EC, true, true, 0x0000, 0x26B1, "CRC-16/TMS37157"}
CRC16_USB         = Params{0x8005, 0xFFFF, true, true, 0xFFFF, 0xB4C8, "CRC-16/USB"}
CRC16_CRC_A       = Params{0x1021, 0xC6C6, true, true, 0x0000, 0xBF05, "CRC-16/CRC-A"}
CRC16_KERMIT      = Params{0x1021, 0x0000, true, true, 0x0000, 0x2189, "CRC-16/KERMIT"}
CRC16_MODBUS      = Params{0x8005, 0xFFFF, true, true, 0x0000, 0x4B37, "CRC-16/MODBUS"}
CRC16_X_25        = Params{0x1021, 0xFFFF, true, true, 0xFFFF, 0x906E, "CRC-16/X-25"}
CRC16_XMODEM      = Params{0x1021, 0x0000, false, false, 0x0000, 0x31C3, "CRC-16/XMODEM"}
*/

#define CRC8			0x00
#define CRC8_CDMA2000	0x01
#define CRC8_DARC		0x02
#define CRC8_DVB_S2		0x03
#define CRC8_EBU		0x04
#define CRC8_I_CODE		0x05
#define CRC8_ITU		0x06
#define CRC8_MAXIM		0x07
#define CRC8_ROHC		0x08
#define CRC8_WCDMA		0x09
#define CRC8_AUTOSAR	0x0A
#define CRC8_BLUETOOTH	0x0B
#define CRC8_LTE		0x0C

#define CRC16_CCITT_FALSE	0x01
#define CRC16_ARC			0x02
#define CRC16_AUR_CCITT		0x03
#define CRC16_BUYPASS		0x04
#define CRC16_CDMA2000		0x05
#define CRC16_DDS_110		0x06
#define CRC16_DECT_R		0x07
#define CRC16_DECT_X		0x08
#define CRC16_DNP			0x09
#define CRC16_EN_13757		0x0A
#define CRC16_GENIBUS		0x0B
#define CRC16_MAXIM			0x0C
#define CRC16_MCRF4XX		0x0D
#define CRC16_RIELLO		0x0E
#define CRC16_T10_DIF		0x0F
#define CRC16_TELEDISK		0x10
#define CRC16_TMS37157		0x11
#define CRC16_USB			0x12
#define CRC_A				0x13
#define CRC16_KERMIT		0x14
#define CRC16_MODBUS		0x15
#define CRC16_X_25			0x16
#define CRC16_XMODEM		0x17
#define CRC16_TEST			0x18

#define CRC32			0x00
#define CRC32_BZIP2		0x01
#define CRC32_C			0x02
#define CRC32_D			0x03
#define CRC32_MPEG2		0x04
#define CRC32_POSIX		0x05
#define CRC32_Q			0x06
#define CRC32_JAMCRC	0x07
#define CRC32_XFER		0x08

#define PROTOCOL_DE		0x01

/************************/
/*	 fuction 	*/
/************************/

unsigned char get_CRC8(unsigned char* DB, unsigned short len, char option);
// parameter - data buffer pointer, data length, option byte
// return - calcurated value
// option - 0_CRC8, 1_CRC8_CDMA2000, 2_CRC8_DARC, 3_CRC8_DVB_S2, 4_CRC8_EBU
//		5_CRC8_I_CODE, 6_CRC8_ITU, 7_CRC8_MAXIM, 8_CRC8_ROHC, 9_CRC8_WCDMA

unsigned short get_CRC16(unsigned char* DB, unsigned short len, char option);
// parameter - data buffer pointer, data length, option byte
// return - calcurated value
// option - 

long get_CRC32(unsigned char* DB, unsigned short len, char option);
// parameter - data buffer pointer, data length, option byte
// return - calcurated value
// option - */

unsigned char GetCRCTypeIndex(char *srcStr, int type);

unsigned char GetUserCksumIndex(char *srcStr);

unsigned char GetUserCksum(char *src, unsigned short length, char type);

#endif