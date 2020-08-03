#include "Inverter_HANS.h"

#define NULL	0

/***************************
*      Public Function     *
****************************/

int GetInvHANSRequest(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char phase)
{
	unsigned short cksum = ID+0x00+0x05; // DestID+SelfID+Data1(ENQ_fixed)
	if(szDest<12) return -1;
	dest[0] = 0x02; // STX
	dest[1] = 0x0C; // length - STX ~ ETX
	dest[2] = 0x00; // frame number
	dest[3] = 0x00; // option high
	dest[4] = 0x00; // option low
	dest[5] = ID; // destination ID
	dest[6] = 0x00; // selfID
	dest[7] = 0x05; // ENQ(enquiry)
	if(phase==HANS_PHASE_ONE) { 
		dest[8] = 0x11;
		cksum += 0x11; 
	} // single phase
	else if(phase==HANS_PHASE_THREE) {
		dest[8] = 0x33;
		cksum +=0x33;
	} // three phase
	else return -1;
	dest[9] = (unsigned char) (cksum>>8);
	dest[10] = (unsigned char) cksum;
	dest[11] = 0x03; // ETX
	return 12;
}

int VerifyInvHANSResponse(const unsigned char *src, unsigned char ID)
{
	unsigned short cksum = 0;
	unsigned int cnt;
	// verify STX, length, ACK code
	if (!((src[0]==0x02) && (src[1]==0x2E) && (src[7]==0x06))) return HANS_ERROR_PACKET;
	if (src[6]!=ID) return HANS_ERROR_ID;
	for(cnt=5;cnt<43;cnt++) {
		cksum += src[cnt];	
	} // calculate cksum
	if (!((unsigned char)(cksum>>8)==src[43] && (unsigned char) cksum==src[44])) return HANS_ERROR_CKSUM;
	
	return 1;
}

int GetInvHANSValue(unsigned long *dest, const unsigned char *src, char sort)
{
	unsigned short valueH;
	unsigned short valueL;
	switch(sort){
	case HANS_VALUE_ID:
		*dest = src[6];
		break;
	case HANS_VALUE_PHASE:
		if(src[8]==0x11) *dest = 0x01;
		else if(src[8]==0x33) *dest = 0x03;
		else *dest = 0;
		break;
	case HANS_VALUE_TODAYP: // Wh
		valueH = src[11];
		valueH = (valueH<<8) & 0xFF00;
		valueH |= src[12];
		valueL = src[13];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[14];
		*dest = valueH;
		*dest = (*dest<<16) & 0xFFFF0000;
		*dest |= valueL;
		break;
	case HANS_VALUE_TOTALP: // Wh
		valueH = src[15];
		valueH = (valueH<<8) & 0xFF00;
		valueH |= src[16];
		valueL = src[17];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[18];
		*dest = valueH;
		*dest = (*dest<<16) & 0xFFFF0000;
		*dest |= valueL;
		break;
	case HANS_VALUE_PVV1: // V
		valueL = src[19];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[20];
		*dest = valueL;
		break;	
	case HANS_VALUE_PVI1: // A_one decimal point
		valueL = src[21];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[22];
		*dest = valueL;
		break;
	case HANS_VALUE_PVV2: // V
		valueL = src[23];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[24];
		*dest = valueL;
		break;
	case HANS_VALUE_PVI2: // A_one decimal point
		valueL = src[25];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[26];
		*dest = valueL;
		break;
	case HANS_VALUE_PVP: // W
		valueH = src[19];
		valueH = (valueH<<8) & 0xFF00;
		valueH |= src[20];		
		valueL = src[21];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[22];
		// input1
		*dest = ((unsigned long)valueH*valueL)/10; // convert to W value
		valueH = src[23];
		valueH = (valueH<<8) & 0xFF00;
		valueH |= src[24];
		valueL = src[25];
		valueL = (valueL<<8) & 0xFF00;
		valueL |= src[26];
		//input2
		*dest += ((unsigned long)valueH*valueL)/10; // convert to W value
		break;
	case HANS_VALUE_OUTPUTRV: // V
		valueL = src[27];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[28];
		*dest = valueL;
		break;
	case HANS_VALUE_OUTPUTRI: // A_one decimal point
		valueL = src[29];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[30];
		*dest = valueL;
		break;
	case HANS_VALUE_OUTPUTSV: // V
		valueL = src[31];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[32];
		*dest = valueL;
		break;
	case HANS_VALUE_OUTPUTSI: // A_one decimal point
		valueL = src[33];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[34];
		*dest = valueL;
		break;
	case HANS_VALUE_OUTPUTTV: // V
		valueL = src[35];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[36];
		*dest = valueL;
		break;
	case HANS_VALUE_OUTPUTTI: // A_one decimal point
		valueL = src[37];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[38];
		*dest = valueL;
		break;
	case HANS_VALUE_OUTPUTP: // W
		valueH = src[27];
		valueH = (valueH<<8) & 0xff00;
		valueH |= src[28];
		valueL = src[29];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[30];
		*dest = ((unsigned long)valueH*valueL)/10; // convert to W
		break;
	case HANS_VALUE_STATUS: // 0x01_run, 0x00_stop
		valueL = src[9];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[10];
		if(valueL!=0) *dest = 0x01;
		else *dest = 0x00;
		break;
	case HANS_VALUE_FAULTHW:
	// 16 bit flags - if occur, set to 1
	// bit15_output DC current bit13_over temperature bit12_leakage current bit11_under frequency
	// bit10_over frequency bit9_boost IGBT fault bit8_inverter IGBT fault bit7_output under voltage
	// bit6_output over voltage bit5_output over current bit4_DC link under voltage
	// bit3_DC link over voltage bit2_input under voltage bit1_input over voltage bit0_input over current
		valueL = src[39];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[40];
		*dest = valueL;
		break;
	case HANS_VALUE_FAULTSW:
	// 16 bit flags - if occur, set to 1
	// bit15_output DC current bit13_over temperature bit12_leakage current bit11_under frequency
	// bit10_over frequency bit9_boost IGBT fault bit8_inverter IGBT fault bit7_output under voltage
	// bit6_output over voltage bit5_output over current bit4_DC link under voltage
	// bit3_DC link over voltage bit2_input under voltage bit1_input over voltage bit0_input over current
		valueL = src[41];
		valueL = (valueL<<8) & 0xff00;
		valueL |= src[42];
		*dest = valueL;
		break;
	default:
		return 0;
	}
	return 1;
}

