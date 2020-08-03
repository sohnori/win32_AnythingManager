#include "MyCRC.h"

/**************************/
/*     Public Function    */
/**************************/

unsigned char GetCRC8(unsigned char src[], unsigned short srcLength, char option)
{
	unsigned char polynomial;
	unsigned char crc8;
	unsigned char cnt_byte, cnt_bit;		
	
	// define polynimial
	if(option==CRC8) 		polynomial = 0x07; // reflected_in_out_false
	else if(option==CRC8_CDMA2000)	polynomial = 0x9B; // reflected_in_out_false
	else if(option==CRC8_DARC)	polynomial = 0x9C; // reflected_in_out_true
	else if(option==CRC8_DVB_S2)	polynomial = 0xD5; // reflected_in_out_false
	else if(option==CRC8_EBU)	polynomial = 0xB8; // reflected_in_out_true
	else if(option==CRC8_I_CODE)	polynomial = 0x1D; // reflected_in_out_false
	else if(option==CRC8_ITU)	polynomial = 0x07; // reflected_in_out_false
	else if(option==CRC8_MAXIM)	polynomial = 0x8C; // reflected_in_out_true
	else if(option==CRC8_ROHC)	polynomial = 0xE0; // reflected_in_out_true
	else if(option==CRC8_WCDMA)	polynomial = 0xD9; // reflected_in_out_true
	else if(option==CRC8_AUTOSAR)	polynomial = 0x2F; // reflected_in_out_false
	else if(option==CRC8_BLUETOOTH)	polynomial = 0xE5; // reflected_in_out_true
	else if(option==CRC8_LTE)	polynomial = 0x9B; // reflected_in_out_false
	else return 0x00;
	
	// Init value set
	if(option==CRC8) 		crc8 = 0x00;
	else if(option==CRC8_CDMA2000)	crc8 = 0xFF;
	else if(option==CRC8_DARC)	crc8 = 0x00;
	else if(option==CRC8_DVB_S2)	crc8 = 0x00;
	else if(option==CRC8_EBU)	crc8 = 0xFF;
	else if(option==CRC8_I_CODE)	crc8 = 0xFD;
	else if(option==CRC8_ITU)	crc8 = 0x00;
	else if(option==CRC8_MAXIM)	crc8 = 0x00;
	else if(option==CRC8_ROHC)	crc8 = 0xFF;
	else if(option==CRC8_WCDMA)	crc8 = 0x00;
	else if(option==CRC8_AUTOSAR)	crc8 = 0xFF;
	else if(option==CRC8_BLUETOOTH)	crc8 = 0x00;
	else if(option==CRC8_LTE)	crc8 = 0x00;	
	
	if(option==CRC8 || option==CRC8_CDMA2000 ||  option==CRC8_DVB_S2 ||  
	option==CRC8_I_CODE || option==CRC8_ITU || option==CRC8_AUTOSAR || option==CRC8_LTE) 
	{ // reflected_in_out_false
		for(cnt_byte=0;cnt_byte<srcLength;cnt_byte++)
		{
			crc8 ^= src[cnt_byte];
			
			for(cnt_bit=0;cnt_bit<8;cnt_bit++)
			{
				if(crc8 & 0b10000000)
				{
					crc8 <<=1;
					crc8 ^= polynomial;
				}
				else
				{
					crc8 <<= 1;	
				}
			}
		}
	}
	if(option==CRC8_DARC || option==CRC8_EBU ||  option==CRC8_MAXIM ||  
	option==CRC8_ROHC || option==CRC8_WCDMA || option==CRC8_BLUETOOTH ) 	
	{ // reflected_in_out_true
		for(cnt_byte=0;cnt_byte<srcLength;cnt_byte++)
		{
			crc8 ^= src[cnt_byte];
			
			for(cnt_bit=0;cnt_bit<8;cnt_bit++)
			{
				if(crc8 & 0b00000001)
				{
					crc8 >>=1;
					crc8 ^= polynomial;
				}
				else
				{
					crc8 >>= 1;	
				}
			}
		}
	}
	// XOR out
	if(option==CRC8_ITU)		crc8 ^= 0x55;
	if(option==CRC8_AUTOSAR)	crc8 ^= 0xFF;
	return crc8;
}

unsigned short GetCRC16(unsigned char src[], unsigned short srcLength, char option)
{
	unsigned short polynomial;
	unsigned short CRC16;
	unsigned short cnt_byte;
	unsigned char cnt_bit;		
	
	// define polynimial
	if(option==CRC16_CCITT_FALSE) 	polynomial = 0x1021; // reflected_in_out_false
	else if(option==CRC16_ARC)	polynomial = 0xA001; // reflected_in_out_true
	else if(option==CRC16_AUR_CCITT)polynomial = 0x1021; // reflected_in_out_false
	else if(option==CRC16_BUYPASS)	polynomial = 0x8005; // reflected_in_out_false
	else if(option==CRC16_CDMA2000)	polynomial = 0xC867; // reflected_in_out_false
	else if(option==CRC16_DDS_110)	polynomial = 0x8005; // reflected_in_out_false
	else if(option==CRC16_DECT_R)	polynomial = 0x0589; // reflected_in_out_false
	else if(option==CRC16_DECT_X)	polynomial = 0x0589; // reflected_in_out_false
	else if(option==CRC16_DNP)	polynomial = 0xA6BC; // reflected_in_out_true
	else if(option==CRC16_EN_13757)	polynomial = 0x3D65; // reflected_in_out_false
	else if(option==CRC16_GENIBUS)	polynomial = 0x1021; // reflected_in_out_false
	else if(option==CRC16_MAXIM)	polynomial = 0xA001; // reflected_in_out_true
	else if(option==CRC16_MCRF4XX)	polynomial = 0x8408; // reflected_in_out_true
	else if(option==CRC16_RIELLO)	polynomial = 0x8408; // reflected_in_out_true
	else if(option==CRC16_T10_DIF)	polynomial = 0x8BB7; // reflected_in_out_false
	else if(option==CRC16_TELEDISK)	polynomial = 0xA097; // reflected_in_out_false
	else if(option==CRC16_TMS37157)	polynomial = 0x8408; // reflected_in_out_true
	else if(option==CRC16_USB)	polynomial = 0xA001; // reflected_in_out_true
	else if(option==CRC_A)		polynomial = 0x8408; // reflected_in_out_true
	else if(option==CRC16_KERMIT)	polynomial = 0x8408; // reflected_in_out_true
	else if(option==CRC16_MODBUS)	polynomial = 0xA001; // reflected_in_out_true
	else if(option==CRC16_X_25)	polynomial = 0x8408; // reflected_in_out_true
	else if(option==CRC16_XMODEM)	polynomial = 0x1021; // reflected_in_out_false
	else return 0x00;
	
	// Init value set
	if(option==CRC16_CCITT_FALSE) 	CRC16 = 0xFFFF; 
	else if(option==CRC16_ARC)	CRC16 = 0x0000;
	else if(option==CRC16_AUR_CCITT)CRC16 = 0x1D0F;
	else if(option==CRC16_BUYPASS)	CRC16 = 0x0000;
	else if(option==CRC16_CDMA2000)	CRC16 = 0xFFFF;
	else if(option==CRC16_DDS_110)	CRC16 = 0x800D;
	else if(option==CRC16_DECT_R)	CRC16 = 0x0000;
	else if(option==CRC16_DECT_X)	CRC16 = 0x0000;
	else if(option==CRC16_DNP)	CRC16 = 0x0000;
	else if(option==CRC16_EN_13757)	CRC16 = 0x0000;
	else if(option==CRC16_GENIBUS)	CRC16 = 0xFFFF;
	else if(option==CRC16_MAXIM)	CRC16 = 0x0000;
	else if(option==CRC16_MCRF4XX)	CRC16 = 0xFFFF;
	else if(option==CRC16_RIELLO)	CRC16 = 0x554D;
	else if(option==CRC16_T10_DIF)	CRC16 = 0x0000;
	else if(option==CRC16_TELEDISK)	CRC16 = 0x0000;
	else if(option==CRC16_TMS37157)	CRC16 = 0x3791;
	else if(option==CRC16_USB)	CRC16 = 0xFFFF;
	else if(option==CRC_A)		CRC16 = 0x6363;
	else if(option==CRC16_KERMIT)	CRC16 = 0x0000;
	else if(option==CRC16_MODBUS)	CRC16 = 0xFFFF;
	else if(option==CRC16_X_25)	CRC16 = 0xFFFF;
	else if(option==CRC16_XMODEM)	CRC16 = 0x0000;
	
	if(option==CRC16_CCITT_FALSE || option==CRC16_AUR_CCITT ||  option==CRC16_BUYPASS || option==CRC16_CDMA2000
	|| option==CRC16_DDS_110 || option==CRC16_DECT_R || option==CRC16_DECT_X || option==CRC16_EN_13757
	|| option==CRC16_GENIBUS || option==CRC16_T10_DIF || option==CRC16_TELEDISK || option==CRC16_XMODEM)
	{ // reflected_in_out_false
		for(cnt_byte=0;cnt_byte<srcLength;cnt_byte++)
		{
			CRC16 ^= (unsigned short) src[cnt_byte]<<8;
			
			for(cnt_bit=0;cnt_bit<8;cnt_bit++)
			{
				if(CRC16 & 0x8000)
				{
					CRC16 <<=1;
					CRC16 ^= polynomial;
				}
				else
				{
					CRC16 <<= 1;	
				}
			}
		}
	}
	
	if(option==CRC16_ARC || option==CRC16_DNP ||  option==CRC16_MAXIM
	|| option==CRC16_MCRF4XX || option==CRC16_RIELLO || option==CRC16_TMS37157
	|| option==CRC16_USB || option==CRC_A || option==CRC16_KERMIT || option==CRC16_MODBUS
	|| option==CRC16_X_25)
	{ // reflected_in_out_true
		for(cnt_byte=0;cnt_byte<srcLength;cnt_byte++)
		{
			CRC16 ^= src[cnt_byte];			
			
			for(cnt_bit=0;cnt_bit<8;cnt_bit++)
			{
				if(CRC16 & 0x0001)
				{
					CRC16 >>=1;
					CRC16 ^= polynomial;					
				}
				else
				{
					CRC16 >>= 1;	
				}
			}
		}
	}
	
	// XOR out
	if(option==CRC16_DNP || option==CRC16_EN_13757 || option==CRC16_GENIBUS || option==CRC16_MAXIM
	|| option==CRC16_USB || option==CRC16_X_25) CRC16 ^= 0xFFFF;	
	if(option==CRC16_DECT_R) CRC16 ^= 0x0001;
		
	return CRC16;
}

unsigned long GetCRC32(unsigned char src[], unsigned short srcLength, char option)
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
	
	// define polynimial
	if(option==CRC32) 		poly.polynomial = 0xEDB88320; // reflected_in_out_true
	else if(option==CRC32_BZIP2)	poly.polynomial = 0x04C11DB7; // reflected_in_out_false
	else if(option==CRC32_C)	poly.polynomial = 0x82F63B78; // reflected_in_out_true
	else if(option==CRC32_D)	poly.polynomial = 0xD419CC15; // reflected_in_out_true
	else if(option==CRC32_MPEG2)	poly.polynomial = 0x04C11DB7; // reflected_in_out_false
	else if(option==CRC32_POSIX)	poly.polynomial = 0x04C11DB7; // reflected_in_out_false
	else if(option==CRC32_Q)	poly.polynomial = 0x814141AB; // reflected_in_out_false
	else if(option==CRC32_JAMCRC)	poly.polynomial = 0xEDB88320; // reflected_in_out_true
	else if(option==CRC32_XFER)	poly.polynomial = 0x000000AF; // reflected_in_out_false	
	else return 0x00;
	
	// Init value set
	if(option==CRC32) 		crc32.crc32_value = 0xFFFFFFFF; 
	else if(option==CRC32_BZIP2)	crc32.crc32_value = 0xFFFFFFFF;
	else if(option==CRC32_C)	crc32.crc32_value = 0xFFFFFFFF;
	else if(option==CRC32_D)	crc32.crc32_value = 0xFFFFFFFF;
	else if(option==CRC32_MPEG2)	crc32.crc32_value = 0xFFFFFFFF;
	else if(option==CRC32_POSIX)	crc32.crc32_value = 0x00000000;
	else if(option==CRC32_Q)	crc32.crc32_value = 0x00000000;
	else if(option==CRC32_JAMCRC)	crc32.crc32_value = 0xFFFFFFFF;
	else if(option==CRC32_XFER)	crc32.crc32_value = 0x00000000;	
	
	if(option==CRC32_BZIP2 || option==CRC32_MPEG2 ||  option==CRC32_POSIX || 
	option==CRC32_Q || option==CRC32_XFER)
	{ // reflected_in_out_false
		for(cnt_byte=0;cnt_byte<srcLength;cnt_byte++)
		{
			crc32.byte[3] ^= src[cnt_byte];
			
			for(cnt_bit=0;cnt_bit<8;cnt_bit++)
			{
				if(crc32.byte[3] & 0x80)
				{
					crc32.byte[3] <<=1;
					if(crc32.byte[2] & 0x80) crc32.byte[3] |= 0x01;
					crc32.byte[2] <<=1;
					if(crc32.byte[1] & 0x80) crc32.byte[2] |= 0x01;
					crc32.byte[1] <<=1;
					if(crc32.byte[0] & 0x80) crc32.byte[1] |= 0x01;
					crc32.byte[0] <<=1;
					crc32.crc32_value ^= poly.polynomial;
				}
				else
				{
					crc32.byte[3] <<=1;
					if(crc32.byte[2] & 0x80) crc32.byte[3] |= 0x01;
					crc32.byte[2] <<=1;
					if(crc32.byte[1] & 0x80) crc32.byte[2] |= 0x01;
					crc32.byte[1] <<=1;
					if(crc32.byte[0] & 0x80) crc32.byte[1] |= 0x01;
					crc32.byte[0] <<=1;	
				}
			}
		}
	}
	
	if(option==CRC32 || option==CRC32_C ||  option==CRC32_D || option==CRC32_JAMCRC)
	{ // reflected_in_out_true
		for(cnt_byte=0;cnt_byte<srcLength;cnt_byte++)
		{
			crc32.byte[0] ^= src[cnt_byte];			
			
			for(cnt_bit=0;cnt_bit<8;cnt_bit++)
			{
				if(crc32.byte[0] & 0x01)
				{
					crc32.byte[0] >>=1;
					if(crc32.byte[1] & 0x01) crc32.byte[0] |= 0x80;
					crc32.byte[1] >>=1;
					if(crc32.byte[2] & 0x01) crc32.byte[1] |= 0x80;
					crc32.byte[2] >>=1;
					if(crc32.byte[3] & 0x01) crc32.byte[2] |= 0x80;
					crc32.byte[3] >>=1;
					crc32.crc32_value ^= poly.polynomial;					
				}
				else
				{
					crc32.byte[0] >>=1;
					if(crc32.byte[1] & 0x01) crc32.byte[0] |= 0x80;
					crc32.byte[1] >>=1;
					if(crc32.byte[2] & 0x01) crc32.byte[1] |= 0x80;
					crc32.byte[2] >>=1;
					if(crc32.byte[3] & 0x01) crc32.byte[2] |= 0x80;
					crc32.byte[3] >>=1;	
				}
			}
		}
	}
	
	// XOR out
	if(option==CRC32 || option==CRC32_BZIP2 || option==CRC32_C || option==CRC32_D
	|| option==CRC32_POSIX) crc32.crc32_value ^= 0xFFFFFFFF;	
		
	return crc32.crc32_value;
}
