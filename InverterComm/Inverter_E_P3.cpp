#include "Inverter_E_P3.h"

#define NULL	0

/***************************
*      Public Function     *
****************************/

int GetInvE_P3Request(unsigned char *dest, unsigned int szDest, unsigned char ID)
{
	unsigned char cksum = ID+0x54+0x18; // ID+command+length
	if(szDest<7) return -1;
	dest[0] = 0x0A; // header1
	dest[1] = 0x96; // header2
	dest[2] = ID; // station ID
	dest[3] = 0x54; // command
	dest[4] = 0x18; // length
	dest[5] = 0x05; // tail
	dest[6] = cksum; // ID+command+length
	return 7;
}

int VerifyInvE_P3Response(const unsigned char *src, unsigned char ID)
{
	unsigned char cksum = 0;
	unsigned int cnt;
	if(!(src[0]==0xB1 && src[1]==0xB7)) return E_P3_ERROR_PACKET;
	if(src[2]!=ID) return E_P3_ERROR_ID;
	for(cnt=0;cnt<31;cnt++){
		cksum^=src[cnt];	
	}
	if(cksum!=src[31]) return E_P3_ERROR_CKSUM;
	return 1;
}

int GetInvE_P3Value(unsigned long *dest, const unsigned char *src, char sort)
{ // output power = grid V * grid I
  // PV power = (PV V1 * PV I) + (PV V2 * PV I)
	unsigned short valueH;
	unsigned short valueL;
	switch(sort){
	case E_P3_VALUE_ID:
		*dest = src[2];
		break;
	case E_P3_VALUE_PVV1: // V_one decimal point
		valueL = src[4];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[3];
		*dest = valueL;
		break;
	case E_P3_VALUE_PVI: // A_one decimal point
		valueL = src[6];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[5];
		*dest = valueL;
		break;
	case E_P3_VALUE_PVV2: // V_one decimal point
		valueL = src[8];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[7];
		*dest = valueL;
		break;
	case E_P3_VALUE_INPUTP: // kW_one decimal point
		valueH = src[4];
		valueH = (valueH<<8) & 0xFF00;
		valueH |= src[3];		
		valueL = src[6];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[5];
		// input1
		*dest = ((unsigned long)valueH*valueL)/100; // convert to kW value
		valueH = src[8];
		valueH = (valueH<<8) & 0xFF00;
		valueH |= src[7];
		//input2
		*dest += ((unsigned long)valueH*valueL)/100; // convert to kW value
		break;
	case E_P3_VALUE_OUTPUTV: // V_one decimal point
		valueL = src[10];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[9];
		*dest = valueL;
		break;
	case E_P3_VALUE_OUTPUTI: // A_one decimal point
		valueL = src[12];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[11];
		*dest = valueL;
		break;
	case E_P3_VALUE_OUTPUTP: // kW_one decimal point
		valueH = src[10];
		valueH = (valueH<<8) & 0xFF00;
		valueH |= src[9];		
		valueL = src[12];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[11];
		*dest = ((unsigned long)valueH*valueL)/100;
		break;
	case E_P3_VALUE_TEMPERATURE: // celsius_ one decimal point
		valueL = src[14];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[13];
		*dest = valueL;
		break;
	case E_P3_VALUE_TODAYP: // kWh_two decimal point
		valueL = src[16];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[15];
		*dest = valueL;
		break;
	case E_P3_VALUE_TOTALP: // kWh
		valueL = src[18];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[17];
		valueH = src[19];
		*dest = valueH;
		*dest = (*dest<<16) & 0xFFFF0000;
		*dest |= valueL;
		break;
	case E_P3_VALUE_FAULT:
	// 0x00000001_PV over current 0x00000002_PV over voltage
	// 0x00000004_PV under voltage 0x00000008_DC link over voltage
	// 0x00000010_DC link under voltage 0x00000020_inverter over current
	// 0x00000040_grid over voltage 0x00000080_grid under voltage
	// 0x00000100_internal over heat 0x00000200_over frequency
	// 0x00000400_under frequency 0x00000800_PV over power
	// 0x00001000_DC ingredient error 0x00002000 DC earth fault
	// 0x00004000_stand alone 0x00200000_inverter over current H/W
		valueL = src[21];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[20];
		valueH = src[23];
		valueH = (valueH<<8) & 0xFF00;
		valueH |= src[22];
		*dest = valueH;
		*dest = (*dest<<16) & 0xFFFF0000;
		*dest |= valueL;
		break;
	case E_P3_VALUE_STATUS: // 1_run, 0_stop
		*dest = src[24];
		break;
	case E_P3_VALUE_FREQUENCY: // Hz_one decimal point
		valueL = src[26];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[25];
		*dest = valueL;
		break;
	case E_P3_VALUE_OPERTIME: // minute
		valueL = src[28];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[27];
		*dest = valueL;
		break;
	case E_P3_VALUE_POWERFACTOR: // percent
		*dest = src[29];
		break;
	case E_P3_VALUE_DSPVERSION: // Ver_two decimal point
		*dest = src[30];
		break;		
	default:
		return 0;
	}
	return 1;
}
