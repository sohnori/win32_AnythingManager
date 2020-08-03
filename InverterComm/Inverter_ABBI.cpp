#include "Inverter_ABBI.h"

#define NULL			0

#define CRC16_CCITT_FALSE	0x01
#define CRC16_ARC		0x02
#define CRC16_AUR_CCITT		0x03
#define CRC16_BUYPASS		0x04
#define CRC16_CDMA2000		0x05
#define CRC16_DDS_110		0x06
#define CRC16_DECT_R		0x07
#define CRC16_DECT_X		0x08
#define CRC16_DNP		0x09
#define CRC16_EN_13757		0x0A
#define CRC16_GENIBUS		0x0B
#define CRC16_MAXIM		0x0C
#define CRC16_MCRF4XX		0x0D
#define CRC16_RIELLO		0x0E
#define CRC16_T10_DIF		0x0F
#define CRC16_TELEDISK		0x10
#define CRC16_TMS37157		0x11
#define CRC16_USB		0x12
#define CRC_A			0x13
#define CRC16_KERMIT		0x14
#define CRC16_MODBUS		0x15
#define CRC16_X_25		0x16
#define CRC16_XMODEM		0x17

/***************************
*      Prevate Function    *
****************************/

static unsigned short GetCRC16(const unsigned char src[], unsigned short srcLength, char option)
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

/***************************
*      Public Function     *
****************************/

int GetInvABBIRequest
(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char cmdCode, unsigned char subCode)
{
	unsigned short crc16_X25;
	if(szDest<10) return -1;
	dest[0] = ID; // inverter ID
	dest[1] = cmdCode; // cmdCode
	dest[2] = subCode; // subCode
	dest[3] = 0x00; // option
	dest[4] = 0x00; // option
	dest[5] = 0x00; // option
	dest[6] = 0x00; // option
	dest[7] = 0x00; // option
	crc16_X25 = GetCRC16(dest, 8, CRC16_X_25);
	dest[8] = (unsigned char)crc16_X25; // crc low
	dest[9] = (unsigned char)(crc16_X25>>8); // crc high
	return 10;
}

int VerifyInvABBIResponse(const unsigned char *src)
{
	unsigned short crc16_X25;
	crc16_X25 = GetCRC16(src, 6, CRC16_X_25);
	if( !((src[6]==(unsigned char)crc16_X25)&&(src[7]==(unsigned char)(crc16_X25>>8)))) return ABBI_ERROR_CRC16_X25;
	
	return 1;
}

int GetInvABBIValue(unsigned long *dest, const unsigned char *src, char sort)
{	
	unsigned short value;
	union Data
	{
		float dataF;
		char dataArr[4];
	} Data;
	Data.dataArr[3] = src[2];
	Data.dataArr[2] = src[3];
	Data.dataArr[1] = src[4];
	Data.dataArr[0] = src[5];
	// if big endian type, use this
	//Data.dataArr[0] = src[2];
	//Data.dataArr[1] = src[3];
	//Data.dataArr[2] = src[4];
	//Data.dataArr[3] = src[5];
	switch(sort){
	case ABBI_VALUE_GRIDV: // V
	case ABBI_VALUE_GRIDP: // W
	case ABBI_VALUE_PVV1: // V
	case ABBI_VALUE_PVV2: // V
	case ABBI_VALUE_PVP1: // W
	case ABBI_VALUE_PVP2: // W
	case ABBI_VALUE_GRIDRV: // V
	case ABBI_VALUE_GRIDSV: // V
	case ABBI_VALUE_GRIDTV: // V
		*dest = (unsigned long)Data.dataF;
		break;
	case ABBI_VALUE_GRIDI: // A_one decimal point	
	case ABBI_VALUE_GRIDFR: // Hz_one decimal point	
	case ABBI_VALUE_PVI1: // A_one decimal point	
	case ABBI_VALUE_PVI2: // A_one decimal point	
	case ABBI_VALUE_GRIDRI: // A_one decimal point
	case ABBI_VALUE_GRIDSI: // A_one decimal point
	case ABBI_VALUE_GRIDTI: // A_one decimal point
		*dest = (unsigned long)(Data.dataF*10);
		break;
	// Wh
	case ABBI_VALUE_ENERGYD:		
	case ABBI_VALUE_ENERGYW:
	case ABBI_VALUE_ENERGYM:
	case ABBI_VALUE_ENERGYY:
	case ABBI_VALUE_ENERGYT:
	case ABBI_VALUE_ENERGYP:
		value = src[2];
		value = (value<<8)&0xFF00;
		value |= (unsigned short)src[3];
		*dest = value;
		*dest = (*dest<<16)&0xFFFF0000;
		value = src[4];
		value = (value<<8)&0xFF00;
		value |= (unsigned short)src[5];
		*dest |= (unsigned long)value;
		break;
	case ABBI_STATE_GLOBAL:
	// state division to value
	// 0_sending parameters 1_wait sun/grid 2_checking grid 3_measuring riso 4_DCDC start 5_inverter start
	// 6_run 7_recovery 8_pause 9_ground fault 10_OTH fault 11_address setting 12_self test 13_self test fail
	// 14_sensor test+meas.riso 15_leak fault 16_wating gor manual reset 17_internal error E026
	// 18_internal error E027 19_internal error E028 20_internal error E029 21_internal error E030
	// 22_sending wind table 23_failed sending table 24_UTH fault 25_remote off 26_interlock fail
	// 27_executing autotest 30_waiting sun 31_temperature fault 32_fan staucked 33_Int.com.fault
	// 34_slave insertion 35_DC switch open 36_TRAS switch open 37_master exclusion 38_auto exclusion
	// 50_wait DC input 98_erasing internal EEprom 99_erasing external EEprom 100_counting EEprom
	// 101_freeze 110_forbidden patner board was found 111_DC string self-test fault 112_service mode
	// 113_"Safety" memory area error 114_too many leak fault events for the day 115_arc afault
	// 120_wrong model code 150_DSP communication error 200_DSP programming
		*dest = (unsigned long)src[1];
		break;
	case ABBI_STATE_INVERTER:
	// state division to value
	// 0_stand by 1_checking grid 2_run 3_bulk OV 4_out OC 5_IGBT saturation 6_bulk UV 7_degauss error
	// 8_no parameters 9_bulk low 10_grid OV 11_communication error 12_degaussing 13_starting
	// 14_bulk cap fail 15_leak fail 16_DCDC fail 17_I leak sensor fail 18_selftest:relay inverter
	// 19_selftest:wait for sensor test 20_selftest:test relay DCDC+sensor 21_selftest:relay inverter fail
	// 22_selftest:timeout fail 23_selftest:relay DCDC fail 24_self test1 25_wating self test start
	// 26_DC injection 27_self test2 28_self test3 29_self test4 30_internal error 31_internal error
	// 40_forbidden state 41_input UC 42_zero power 43_ grid not present 44_wating start 45_MPPT
	// 46_grdi fail 47_input OC 255_inverter DSP not programmed
		*dest = (unsigned long)src[2];
		break;
	case ABBI_STATE_DCDCCH1:
	// state division to value
	// 0_DCDC off 1_ramp start 2_MPPT 3_not used 4_input OC 5_input UV 6_input OV 7_input low
	// 8_no parameters 9_bulk OV 10_communication error 11_ramp fail 12_internal error 13_input mode error
	// 14_ground fault 15_inverter fail 16_DCDC IGBT saturation 17_DCDC Ileak fail 18_DCDC grid fail
	// 19_DCDC_ communicatio error 255_DCDC DSP not programmed
		*dest = (unsigned long)src[3];
		break;
	case ABBI_STATE_DCDCCH2:
	// same DCDCCH1
		*dest = (unsigned long)src[4];
		break;
	case ABBI_STATE_ALARM:
	// state division to value
	// 0_no alarm 1_sun low 2_input OC 3_input UV 4_input OV 5_sun low 6_no parameters 7_bulk OV
	// 8_comm.error 9_output OC 10_IGBT Sat 11_bulk UV 12_internal error 13_grid fail 14_bulk low
	// 15_ramp fail 16_DCDC fail1 17_wrong mode 18_ground fault 19_over temp. 20_bulk Cap fail
	// 21_inverter fail 22_start timeout 23_ground fault 24_AC feed forward 25_I leak sens.fail
	// 26_DCDC fail2 27_self test error1 28_self test error2 29_self test error3 30_self test error4
	// 31_DC inj error 32_grid OV 33_grid UV 34_grid OF 35_grid UF 36_Z grid Hi 37_internal error
	// 38_Riso low 39_Vref error 40_error Meas V 41_error Meas F 42_error Meas Z 43_error Meas Ileak
	// 44_error read V 45_error read I 46_table fail 47_fan fail 48_UTH 49_interlock fail 50_remote off
	// 51_remote off 52_V out Avg erro 53_Clk fail 54_input UC 55_zero power 56_fan stuck 57_DC switch open
	// 58_Tras switch open 59_AC switch open 60_bulk UV 61_autoexclusion 62_grid df/dt 63_Den switch open
	// 64_jbox fail 65_Dc door open 66_AC door open 67_anti islanding 68_fuse DC fail 69_liquid cooler fail
	// 70_SPD AC protection open 71_SPD DC protection open 72_string selftest fail 73_power reduction start
	// 74_power reduction end 75_React. power mode changed 76_data/time changed 77_energy data reset
	// 79_Arc fault 80_bad "safty" memory area 81_module door open 82_"AF self test fail"
		*dest = (unsigned long)src[5];
		break;
	default:
		return 0;
	}
	return 1;
}

int GetInvABBIValueStr(unsigned char *dest, unsigned char szDest, const unsigned char *src, char sort)
{
	unsigned short value;
	union Data
	{
		float dataF;
		char dataArr[4];
	} Data;
	if(szDest<11) return -1;
	Data.dataArr[3] = src[2];
	Data.dataArr[2] = src[3];
	Data.dataArr[1] = src[4];
	Data.dataArr[0] = src[5];
	// if big endian type, use this
	//Data.dataArr[0] = src[2];
	//Data.dataArr[1] = src[3];
	//Data.dataArr[2] = src[4];
	//Data.dataArr[3] = src[5];
	switch(sort){
	case ABBI_VALUE_GRIDV:
	case ABBI_VALUE_GRIDI:
	case ABBI_VALUE_GRIDP:
	case ABBI_VALUE_GRIDFR:
	case ABBI_VALUE_PVV1:
	case ABBI_VALUE_PVI1:
	case ABBI_VALUE_PVV2:
	case ABBI_VALUE_PVI2:
	case ABBI_VALUE_PVP1:
	case ABBI_VALUE_PVP2:
	case ABBI_VALUE_GRIDRV:
	case ABBI_VALUE_GRIDSV:
	case ABBI_VALUE_GRIDTV:
	case ABBI_VALUE_GRIDRI:
	case ABBI_VALUE_GRIDSI:
	case ABBI_VALUE_GRIDTI:
		break;
	case ABBI_VALUE_ENERGYD:
	case ABBI_VALUE_ENERGYW:
	case ABBI_VALUE_ENERGYM:
	case ABBI_VALUE_ENERGYY:
	case ABBI_VALUE_ENERGYT:
	case ABBI_VALUE_ENERGYP:
		break;
	case ABBI_VALUE_SERIALN: // src[0]_most ~ src[5]_least
	case ABBI_VALUE_PN: // src[0]_most ~ src[5]_least
		dest[0] = src[0];
		dest[1] = src[1];
		dest[2] = src[2];
		dest[3] = src[3];
		dest[4] = src[4];
		dest[5] = src[5];
		dest[6] = NULL;
		break;
	case ABBI_VALUE_VERSION:  
	// src[2]_model char src[3]_grid standard char src[4]_transformer char src[5]_type char
	case ABBI_VALUE_PDAY: // src[2]_week2 src[3]_week1 src[4]_year2 src[5]_year1
		dest[0] = src[2];
		dest[1] = src[3];
		dest[2] = src[4];
		dest[3] = src[5];
		dest[4] = NULL;
		break;
	case ABBI_STATE_GLOBAL:
	case ABBI_STATE_INVERTER:
	case ABBI_STATE_DCDCCH1:
	case ABBI_STATE_DCDCCH2:
	case ABBI_STATE_ALARM:
	default:
		return 0;
	}
	return 1;
}