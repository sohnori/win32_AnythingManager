#include "CRC.h"

unsigned char get_CRC8(unsigned char* DB, unsigned short len, char option)
{
	unsigned char polynomial;
	unsigned char crc8;
	unsigned char cnt_byte, cnt_bit;

	if (option == CRC8) 				polynomial = 0x07; // reflected_in_out_false
	else if (option == CRC8_CDMA2000)	polynomial = 0x9B; // reflected_in_out_false
	else if (option == CRC8_DARC)		polynomial = 0x9C; // reflected_in_out_true
	else if (option == CRC8_DVB_S2)		polynomial = 0xD5; // reflected_in_out_false
	else if (option == CRC8_EBU)		polynomial = 0xB8; // reflected_in_out_true
	else if (option == CRC8_I_CODE)		polynomial = 0x1D; // reflected_in_out_false
	else if (option == CRC8_ITU)		polynomial = 0x07; // reflected_in_out_false
	else if (option == CRC8_MAXIM)		polynomial = 0x8C; // reflected_in_out_true
	else if (option == CRC8_ROHC)		polynomial = 0xE0; // reflected_in_out_true
	else if (option == CRC8_WCDMA)		polynomial = 0xD9; // reflected_in_out_true
	else if (option == CRC8_AUTOSAR)	polynomial = 0x2F; // reflected_in_out_false
	else if (option == CRC8_BLUETOOTH)	polynomial = 0xE5; // reflected_in_out_true
	else if (option == CRC8_LTE)		polynomial = 0x9B; // reflected_in_out_false
	else return 0x00;

	if (option == CRC8) 				crc8 = 0x00;
	else if (option == CRC8_CDMA2000)	crc8 = 0xFF;
	else if (option == CRC8_DARC)		crc8 = 0x00;
	else if (option == CRC8_DVB_S2)		crc8 = 0x00;
	else if (option == CRC8_EBU)		crc8 = 0xFF;
	else if (option == CRC8_I_CODE)		crc8 = 0xFD;
	else if (option == CRC8_ITU)		crc8 = 0x00;
	else if (option == CRC8_MAXIM)		crc8 = 0x00;
	else if (option == CRC8_ROHC)		crc8 = 0xFF;
	else if (option == CRC8_WCDMA)		crc8 = 0x00;
	else if (option == CRC8_AUTOSAR)	crc8 = 0xFF;
	else if (option == CRC8_BLUETOOTH)	crc8 = 0x00;
	else if (option == CRC8_LTE)		crc8 = 0x00;
	// Init value set

	if (option == CRC8 || option == CRC8_CDMA2000 || option == CRC8_DVB_S2 ||
		option == CRC8_I_CODE || option == CRC8_ITU || option == CRC8_AUTOSAR || option == CRC8_LTE)
	{
		for (cnt_byte = 0; cnt_byte<len; cnt_byte++)
		{
			crc8 ^= DB[cnt_byte];

			for (cnt_bit = 0; cnt_bit<8; cnt_bit++)
			{
				if (crc8 & 0b10000000)
				{
					crc8 <<= 1;
					crc8 ^= polynomial;
				}
				else
				{
					crc8 <<= 1;
				}
			}
		}
	}
	if (option == CRC8_DARC || option == CRC8_EBU || option == CRC8_MAXIM ||
		option == CRC8_ROHC || option == CRC8_WCDMA || option == CRC8_BLUETOOTH)
	{
		for (cnt_byte = 0; cnt_byte<len; cnt_byte++)
		{
			crc8 ^= DB[cnt_byte];

			for (cnt_bit = 0; cnt_bit<8; cnt_bit++)
			{
				if (crc8 & 0b00000001)
				{
					crc8 >>= 1;
					crc8 ^= polynomial;
				}
				else
				{
					crc8 >>= 1;
				}
			}
		}
	}
	if (option == CRC8_ITU)		crc8 ^= 0x55;
	if (option == CRC8_AUTOSAR)	crc8 ^= 0xFF;
	return crc8;
}

unsigned short get_CRC16(unsigned char* DB, unsigned short len, char option)
{
	unsigned short polynomial;
	unsigned short CRC16;
	unsigned short cnt_byte;
	unsigned char cnt_bit;

	if (option == CRC16_CCITT_FALSE) 	polynomial = 0x1021; // reflected_in_out_false
	else if (option == CRC16_ARC)		polynomial = 0xA001; // reflected_in_out_true
	else if (option == CRC16_AUR_CCITT)	polynomial = 0x1021; // reflected_in_out_false
	else if (option == CRC16_BUYPASS)	polynomial = 0x8005; // reflected_in_out_false
	else if (option == CRC16_CDMA2000)	polynomial = 0xC867; // reflected_in_out_false
	else if (option == CRC16_DDS_110)	polynomial = 0x8005; // reflected_in_out_false
	else if (option == CRC16_DECT_R)	polynomial = 0x0589; // reflected_in_out_false
	else if (option == CRC16_DECT_X)	polynomial = 0x0589; // reflected_in_out_false
	else if (option == CRC16_DNP)		polynomial = 0xA6BC; // reflected_in_out_true
	else if (option == CRC16_EN_13757)	polynomial = 0x3D65; // reflected_in_out_false
	else if (option == CRC16_GENIBUS)	polynomial = 0x1021; // reflected_in_out_false
	else if (option == CRC16_MAXIM)		polynomial = 0xA001; // reflected_in_out_true
	else if (option == CRC16_MCRF4XX)	polynomial = 0x8408; // reflected_in_out_true
	else if (option == CRC16_RIELLO)	polynomial = 0x8408; // reflected_in_out_true
	else if (option == CRC16_T10_DIF)	polynomial = 0x8BB7; // reflected_in_out_false
	else if (option == CRC16_TELEDISK)	polynomial = 0xA097; // reflected_in_out_false
	else if (option == CRC16_TMS37157)	polynomial = 0x8408; // reflected_in_out_true
	else if (option == CRC16_USB)		polynomial = 0xA001; // reflected_in_out_true
	else if (option == CRC_A)			polynomial = 0x8408; // reflected_in_out_true
	else if (option == CRC16_KERMIT)	polynomial = 0x8408; // reflected_in_out_true
	else if (option == CRC16_MODBUS)	polynomial = 0xA001; // reflected_in_out_true
	else if (option == CRC16_X_25)		polynomial = 0x8408; // reflected_in_out_true
	else if (option == CRC16_XMODEM)	polynomial = 0x1021; // reflected_in_out_false
	else if (option == CRC16_TEST)		polynomial = 0xA01C; // reflected_in_out_false
	else return 0x00;

	if (option == CRC16_CCITT_FALSE) 	CRC16 = 0xFFFF;
	else if (option == CRC16_ARC)		CRC16 = 0x0000;
	else if (option == CRC16_AUR_CCITT)	CRC16 = 0x1D0F;
	else if (option == CRC16_BUYPASS)	CRC16 = 0x0000;
	else if (option == CRC16_CDMA2000)	CRC16 = 0xFFFF;
	else if (option == CRC16_DDS_110)	CRC16 = 0x800D;
	else if (option == CRC16_DECT_R)	CRC16 = 0x0000;
	else if (option == CRC16_DECT_X)	CRC16 = 0x0000;
	else if (option == CRC16_DNP)		CRC16 = 0x0000;
	else if (option == CRC16_EN_13757)	CRC16 = 0x0000;
	else if (option == CRC16_GENIBUS)	CRC16 = 0xFFFF;
	else if (option == CRC16_MAXIM)		CRC16 = 0x0000;
	else if (option == CRC16_MCRF4XX)	CRC16 = 0xFFFF;
	else if (option == CRC16_RIELLO)	CRC16 = 0x554D;
	else if (option == CRC16_T10_DIF)	CRC16 = 0x0000;
	else if (option == CRC16_TELEDISK)	CRC16 = 0x0000;
	else if (option == CRC16_TMS37157)	CRC16 = 0x3791;
	else if (option == CRC16_USB)		CRC16 = 0xFFFF;
	else if (option == CRC_A)			CRC16 = 0x6363;
	else if (option == CRC16_KERMIT)	CRC16 = 0x0000;
	else if (option == CRC16_MODBUS)	CRC16 = 0xFFFF;
	else if (option == CRC16_X_25)		CRC16 = 0xFFFF;
	else if (option == CRC16_XMODEM)	CRC16 = 0x0000;
	else if (option == CRC16_TEST)		CRC16 = 0x0000;
	// Init value set

	if (option == CRC16_CCITT_FALSE || option == CRC16_AUR_CCITT || option == CRC16_BUYPASS || option == CRC16_CDMA2000
		|| option == CRC16_DDS_110 || option == CRC16_DECT_R || option == CRC16_DECT_X || option == CRC16_EN_13757
		|| option == CRC16_GENIBUS || option == CRC16_T10_DIF || option == CRC16_TELEDISK || option == CRC16_XMODEM || option == CRC16_TEST)
	{
		for (cnt_byte = 0; cnt_byte<len; cnt_byte++)
		{
			CRC16 ^= (unsigned short)DB[cnt_byte] << 8;

			for (cnt_bit = 0; cnt_bit<8; cnt_bit++)
			{
				if (CRC16 & 0x8000)
				{
					CRC16 <<= 1;
					CRC16 ^= polynomial;
				}
				else
				{
					CRC16 <<= 1;
				}
			}
		}
	}
	if (option == CRC16_ARC || option == CRC16_DNP || option == CRC16_MAXIM
		|| option == CRC16_MCRF4XX || option == CRC16_RIELLO || option == CRC16_TMS37157
		|| option == CRC16_USB || option == CRC_A || option == CRC16_KERMIT || option == CRC16_MODBUS
		|| option == CRC16_X_25)
	{
		for (cnt_byte = 0; cnt_byte<len; cnt_byte++)
		{
			CRC16 ^= DB[cnt_byte];

			for (cnt_bit = 0; cnt_bit<8; cnt_bit++)
			{
				if (CRC16 & 0x0001)
				{
					CRC16 >>= 1;
					CRC16 ^= polynomial;
				}
				else
				{
					CRC16 >>= 1;
				}
			}
		}
	}
	if (option == CRC16_DNP || option == CRC16_EN_13757 || option == CRC16_GENIBUS || option == CRC16_MAXIM
		|| option == CRC16_USB || option == CRC16_X_25) CRC16 ^= 0xFFFF;

	if (option == CRC16_DECT_R) CRC16 ^= 0x0001;

	return CRC16;
}

long get_CRC32(unsigned char* DB, unsigned short len, char option)
{
	union poly
	{
		unsigned long polynomial;
		unsigned char byte[4];
	}poly;

	union crc32
	{
		unsigned long crc32_value;
		unsigned char byte[4];
	}crc32;

	unsigned short cnt_byte;
	unsigned char cnt_bit;

	if (option == CRC32) 			poly.polynomial = 0xEDB88320; // reflected_in_out_true
	else if (option == CRC32_BZIP2)	poly.polynomial = 0x04C11DB7; // reflected_in_out_false
	else if (option == CRC32_C)		poly.polynomial = 0x82F63B78; // reflected_in_out_true
	else if (option == CRC32_D)		poly.polynomial = 0xD419CC15; // reflected_in_out_true
	else if (option == CRC32_MPEG2)	poly.polynomial = 0x04C11DB7; // reflected_in_out_false
	else if (option == CRC32_POSIX)	poly.polynomial = 0x04C11DB7; // reflected_in_out_false
	else if (option == CRC32_Q)		poly.polynomial = 0x814141AB; // reflected_in_out_false
	else if (option == CRC32_JAMCRC)poly.polynomial = 0xEDB88320; // reflected_in_out_true
	else if (option == CRC32_XFER)	poly.polynomial = 0x000000AF; // reflected_in_out_false

	else return 0x00;

	if (option == CRC32) 			crc32.crc32_value = 0xFFFFFFFF;
	else if (option == CRC32_BZIP2)	crc32.crc32_value = 0xFFFFFFFF;
	else if (option == CRC32_C)		crc32.crc32_value = 0xFFFFFFFF;
	else if (option == CRC32_D)		crc32.crc32_value = 0xFFFFFFFF;
	else if (option == CRC32_MPEG2)	crc32.crc32_value = 0xFFFFFFFF;
	else if (option == CRC32_POSIX)	crc32.crc32_value = 0x00000000;
	else if (option == CRC32_Q)		crc32.crc32_value = 0x00000000;
	else if (option == CRC32_JAMCRC)crc32.crc32_value = 0xFFFFFFFF;
	else if (option == CRC32_XFER)	crc32.crc32_value = 0x00000000;
	// Init value set

	if (option == CRC32_BZIP2 || option == CRC32_MPEG2 || option == CRC32_POSIX ||
		option == CRC32_Q || option == CRC32_XFER)
	{
		for (cnt_byte = 0; cnt_byte<len; cnt_byte++)
		{
			crc32.byte[3] ^= DB[cnt_byte];

			for (cnt_bit = 0; cnt_bit<8; cnt_bit++)
			{
				if (crc32.byte[3] & 0x80)
				{
					crc32.byte[3] <<= 1;
					if (crc32.byte[2] & 0x80) crc32.byte[3] |= 0x01;
					crc32.byte[2] <<= 1;
					if (crc32.byte[1] & 0x80) crc32.byte[2] |= 0x01;
					crc32.byte[1] <<= 1;
					if (crc32.byte[0] & 0x80) crc32.byte[1] |= 0x01;
					crc32.byte[0] <<= 1;
					crc32.crc32_value ^= poly.polynomial;
				}
				else
				{
					crc32.byte[3] <<= 1;
					if (crc32.byte[2] & 0x80) crc32.byte[3] |= 0x01;
					crc32.byte[2] <<= 1;
					if (crc32.byte[1] & 0x80) crc32.byte[2] |= 0x01;
					crc32.byte[1] <<= 1;
					if (crc32.byte[0] & 0x80) crc32.byte[1] |= 0x01;
					crc32.byte[0] <<= 1;
				}
			}
		}
	}
	if (option == CRC32 || option == CRC32_C || option == CRC32_D || option == CRC32_JAMCRC)
	{
		for (cnt_byte = 0; cnt_byte<len; cnt_byte++)
		{
			crc32.byte[0] ^= DB[cnt_byte];

			for (cnt_bit = 0; cnt_bit<8; cnt_bit++)
			{
				if (crc32.byte[0] & 0x01)
				{
					crc32.byte[0] >>= 1;
					if (crc32.byte[1] & 0x01) crc32.byte[0] |= 0x80;
					crc32.byte[1] >>= 1;
					if (crc32.byte[2] & 0x01) crc32.byte[1] |= 0x80;
					crc32.byte[2] >>= 1;
					if (crc32.byte[3] & 0x01) crc32.byte[2] |= 0x80;
					crc32.byte[3] >>= 1;
					crc32.crc32_value ^= poly.polynomial;
				}
				else
				{
					crc32.byte[0] >>= 1;
					if (crc32.byte[1] & 0x01) crc32.byte[0] |= 0x80;
					crc32.byte[1] >>= 1;
					if (crc32.byte[2] & 0x01) crc32.byte[1] |= 0x80;
					crc32.byte[2] >>= 1;
					if (crc32.byte[3] & 0x01) crc32.byte[2] |= 0x80;
					crc32.byte[3] >>= 1;
				}
			}
		}
	}
	if (option == CRC32 || option == CRC32_BZIP2 || option == CRC32_C || option == CRC32_D
		|| option == CRC32_POSIX) crc32.crc32_value ^= 0xFFFFFFFF;

	return crc32.crc32_value;
}

unsigned char GetCRCTypeIndex(char *srcStr, int type)
{
	unsigned char retValue;
	switch (type) {
		case 8:
			if (srcStr[0]=='C' && srcStr[1] == 'R' && srcStr[2] == 'C' && srcStr[3] == '8'){
				if (srcStr[4] == 0) retValue = 0x00;
				else if (srcStr[5] == 'C' && srcStr[6] == 'D' && srcStr[7] == 'M' && srcStr[8] == 'A') retValue = 0x01;
				else if (srcStr[5] == 'D' && srcStr[6] == 'A' && srcStr[7] == 'R' && srcStr[8] == 'C') retValue = 0x02;
				else if (srcStr[5] == 'D' && srcStr[6] == 'V' && srcStr[7] == 'B' && srcStr[10] == '2') retValue = 0x03;
				else if (srcStr[5] == 'E' && srcStr[6] == 'B' && srcStr[7] == 'U') retValue = 0x04;
				else if (srcStr[5] == 'I' && srcStr[7] == 'C' && srcStr[8] == 'O' && srcStr[9] == 'D') retValue = 0x05;
				else if (srcStr[5] == 'I' && srcStr[6] == 'T' && srcStr[7] == 'U') retValue = 0x06;
				else if (srcStr[5] == 'M' && srcStr[6] == 'A' && srcStr[7] == 'X') retValue = 0x07;
				else if (srcStr[5] == 'R' && srcStr[6] == 'O' && srcStr[7] == 'H') retValue = 0x08;
				else if (srcStr[5] == 'W' && srcStr[6] == 'C' && srcStr[7] == 'D') retValue = 0x09;
				else if (srcStr[5] == 'A' && srcStr[6] == 'U' && srcStr[7] == 'T') retValue = 0x0A;
				else if (srcStr[5] == 'B' && srcStr[6] == 'L' && srcStr[7] == 'U') retValue = 0x0B;
				else if (srcStr[5] == 'L' && srcStr[6] == 'T' && srcStr[7] == 'E') retValue = 0x0C;
				else retValue = 0x80;
			}
			else retValue = 0x80;
			break;
		case 16:
			if (srcStr[0] == 'C' && srcStr[1] == 'R' && srcStr[2] == 'C' && srcStr[3] == '1' && srcStr[4] == '6') {				
				if (srcStr[6] == 'C' && srcStr[7] == 'C' && srcStr[8] == 'I') retValue = 0x01;
				else if (srcStr[6] == 'A' && srcStr[7] == 'R' && srcStr[8] == 'C') retValue = 0x02;
				else if (srcStr[6] == 'A' && srcStr[7] == 'U' && srcStr[8] == 'R') retValue = 0x03;
				else if (srcStr[6] == 'B' && srcStr[7] == 'U' && srcStr[8] == 'Y') retValue = 0x04;
				else if (srcStr[6] == 'C' && srcStr[7] == 'D' && srcStr[8] == 'M') retValue = 0x05;
				else if (srcStr[6] == 'D' && srcStr[7] == 'D' && srcStr[11] == 'S') retValue = 0x06;
				else if (srcStr[6] == 'D' && srcStr[7] == 'E' && srcStr[11] == 'R') retValue = 0x07;
				else if (srcStr[6] == 'D' && srcStr[7] == 'E' && srcStr[11] == 'X') retValue = 0x08;
				else if (srcStr[6] == 'D' && srcStr[7] == 'N' && srcStr[8] == 'P') retValue = 0x09;
				else if (srcStr[6] == 'E' && srcStr[7] == 'N' && srcStr[8] == '_') retValue = 0x0A;
				else if (srcStr[6] == 'G' && srcStr[7] == 'E' && srcStr[8] == 'N') retValue = 0x0B;
				else if (srcStr[6] == 'M' && srcStr[7] == 'A' && srcStr[8] == 'X') retValue = 0x0C;
				else if (srcStr[6] == 'M' && srcStr[7] == 'C' && srcStr[8] == 'R') retValue = 0x0D;
				else if (srcStr[6] == 'R' && srcStr[7] == 'I' && srcStr[8] == 'E') retValue = 0x0E;
				else if (srcStr[6] == 'T' && srcStr[7] == '1' && srcStr[8] == '0') retValue = 0x0F;
				else if (srcStr[6] == 'T' && srcStr[7] == 'E' && srcStr[8] == 'L') retValue = 0x10;
				else if (srcStr[6] == 'T' && srcStr[7] == 'M' && srcStr[8] == 'S') retValue = 0x11;
				else if (srcStr[6] == 'U' && srcStr[7] == 'S' && srcStr[8] == 'B') retValue = 0x12;
				else if (srcStr[6] == 'K' && srcStr[7] == 'E' && srcStr[8] == 'R') retValue = 0x14;
				else if (srcStr[6] == 'M' && srcStr[7] == 'O' && srcStr[8] == 'D') retValue = 0x15;
				else if (srcStr[6] == 'X' && srcStr[8] == '2' && srcStr[9] == '5') retValue = 0x16;
				else if (srcStr[6] == 'X' && srcStr[7] == 'M' && srcStr[8] == 'O') retValue = 0x17;
				else if (srcStr[6] == 'T' && srcStr[7] == 'E' && srcStr[8] == 'S') retValue = 0x18;
				else retValue = 0x80;
			}
			else if (srcStr[0] == 'C' && srcStr[1] == 'R' && srcStr[2] == 'C' && srcStr[3] == '_' && srcStr[4] == 'A') retValue = 0x13;
			else retValue = 0x80;
			break;
		case 32:
			if (srcStr[0] == 'C' && srcStr[1] == 'R' && srcStr[2] == 'C' && srcStr[3] == '3' && srcStr[4] == '2') {
				if (srcStr[5] == 0) retValue = 0x00;
				else if (srcStr[6] == 'B' && srcStr[7] == 'Z' && srcStr[8] == 'I') retValue = 0x01;
				else if (srcStr[5] == '_' && srcStr[6] == 'C') retValue = 0x02;
				else if (srcStr[5] == '_' && srcStr[6] == 'D') retValue = 0x03;
				else if (srcStr[6] == 'M' && srcStr[7] == 'P' && srcStr[8] == 'E') retValue = 0x04;
				else if (srcStr[6] == 'P' && srcStr[7] == 'O' && srcStr[8] == 'S') retValue = 0x05;
				else if (srcStr[5] == '_' && srcStr[6] == 'Q') retValue = 0x06;
				else if (srcStr[6] == 'J' && srcStr[7] == 'A' && srcStr[8] == 'M') retValue = 0x07;
				else if (srcStr[6] == 'X' && srcStr[7] == 'F' && srcStr[8] == 'E') retValue = 0x08;							
				else retValue = 0x80;
			}			
			else retValue = 0x80;
			break;
		default:
			retValue = 0x80;
			break;
	}
	return retValue;
}