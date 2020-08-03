#include "Inverter_E_P5.h"

#define NULL	0

/***************************
*      Public Function     *
****************************/

int GetInvE_P5Request(unsigned char *dest, unsigned int szDest, unsigned char ID)
{
	unsigned char cksum = ID+0x54+0x18; // ID+command+length
	if(szDest<7) return -1;
	dest[0] = 0x0A; // header1
	dest[1] = 0x96; // header2
	dest[2] = ID; // station ID
	dest[3] = 0x54; // command
	dest[4] = 0x18; // Don'tCare
	dest[5] = 0x05; // fixed data
	dest[6] = cksum; // ID+command+Don'tCare
	return 7;
}

int VerifyInvE_P5Response(const unsigned char *src, unsigned char ID)
{
	unsigned char cksum = 0;
	unsigned int cnt;
	if(!(src[0]==0xB1 && src[1]==0xB5)) return E_P5_ERROR_PACKET;
	if(src[2]!=ID) return E_P5_ERROR_ID;
	for(cnt=0;cnt<38;cnt++){
		cksum^=src[cnt];	
	}
	if(cksum!=src[39]) return E_P5_ERROR_CKSUM;
	return 1;
}

int GetInvE_P5Value(unsigned long *dest, const unsigned char *src, char sort)
{ 
	unsigned short valueH;
	unsigned short valueL;
	switch(sort){
	case E_P5_VALUE_ID:
		*dest = src[2];
		break;
	case E_P5_VALUE_PVV1: // V1_one decimal point
		valueL = src[4];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[3];
		*dest = valueL;
		break;
	case E_P5_VALUE_PVI1: // A1_two decimal point
		valueL = src[6];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[5];
		*dest = valueL;
		break;
	case E_P5_VALUE_PVP1: // W1
		valueL = src[8];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[7];
		*dest = valueL;
		break;
	case E_P5_VALUE_PVV2: // V2_one decimal point
		valueL = src[10];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[9];
		*dest = valueL;
		break;
	case E_P5_VALUE_PVI2: // A2_two decimal point
		valueL = src[12];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[11];
		*dest = valueL;
		break;
	case E_P5_VALUE_PVP2: // W2
		valueL = src[14];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[13];
		*dest = valueL;
		break;
	case E_P5_VALUE_OUTPUTV: // V_one decimal point
		valueL = src[16];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[15];
		*dest = valueL;
		break;
	case E_P5_VALUE_OUTPUTI: // A_two decimal point
		valueL = src[18];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[17];
		*dest = valueL;
		break;
	case E_P5_VALUE_OUTPUTP: // W	
		valueL = src[20];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[19];
		*dest = valueL;
		break;
	case E_P5_VALUE_FREQUENCY: // Hz_one decimal point
		valueL = src[22];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[21];
		*dest = valueL;
		break;
	case E_P5_VALUE_TOTALP: // kWh
		valueL = src[24];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[23];
		valueH = src[25];
		*dest = valueH;
		*dest = (*dest<<16) & 0xFFFF0000;
		*dest |= valueL;
		break;
	case E_P5_VALUE_TODAYP: // kWh_two decimal point
		valueL = src[27];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[26];
		*dest = valueL;
		break;	
	case E_P5_VALUE_TIME: // second
		valueL = src[32];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[31];
		valueH = src[33];
		*dest = valueH;
		*dest = (*dest<<16) & 0xFFFF0000;
		*dest |= valueL;
		break;
	case E_P5_VALUE_INVSTATUS:
	// 8bit flags
	// bit7(standby)_1:stop 0:run	bit6(run)_1:run 0:stop	bit4(grid fault)_1:fault 0:normal
	// bit3(trouble)_1:trouble 0:normal	bit2(warning)_1:warning 0:normal
		*dest = src[34];
		break;
	case E_P5_VALUE_GRIDFAULT:
	// 8bit flags - 0 value is normal
	// bit7_1:AC overV	bit6_1:AC underV	bit5_1:AC over freq	bit4_1:AC under freq
	// bit1_1:high AC volt
		*dest = src[35];
		break;
	case E_P5_VALUE_FAULT1:
	// 8bit flags - 0 value is normal
	// bit7_1:Solar overV	bit6_1:Solar underV	bit4_1:Inv overI	bit3_1:DC link overV
	// bit2_1:out DC I	bit1_1:earth fault	bit0_1:Inv over temp
		*dest = src[36];
		break;
	case E_P5_VALUE_FAULT2:
	// 8bit flags - 0 value is normal
	// bit5_1:Solar over power
		*dest = src[37];
		break;
	case E_P5_VALUE_WARRING:
	// 8bit flags - 0 value is normal
	// bit7_1:output power derating	bit5_1:temperature derating	bit4_1:current derating
		*dest = src[38];
		break;
	default:
		return 0;
	}
	return 1;
}
