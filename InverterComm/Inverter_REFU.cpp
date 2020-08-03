#include "Inverter_REFU.h"

/***************************
*      Public Function     *
****************************/

int GetInvREFURequest
(unsigned char *dest, unsigned int szDest, unsigned char ID, char AK, unsigned short parameter, short IND)
{
	unsigned int cnt;
	unsigned char bcc = 0;
	unsigned short PKE = 0;
	if(ID>31) return REFU_ERROR_ID;
	if(szDest<24) return -1;
	ID &= 0b00011111; // 7_special, 6_mirror, 5_broadcast telegram 4~0_ID(0~31)
	// special fuction telegram default set
	PKE = AK;
	PKE <<= 8;
	PKE |= parameter;
	dest[0] = 0x02; // STX
	dest[1] = 0x16;// length type2_22byte fixed
	dest[2] = ID;
	dest[3] = (unsigned char)(PKE>>8); // parameter ID high byte
	dest[4] = (unsigned char)PKE;  // parameter ID low byte
	dest[5] = (unsigned char)(IND>>8); // index word high byte
	dest[6] = (unsigned char)IND; // index word low byte
	dest[7] = 0x00; // PZD1(process data)_control word
	dest[8] = 0x00; // PZD2 main set value
	dest[9] = 0x00; // PZD3 addtional set value
	dest[10] = 0x00; // PZD4 addtional set value
	dest[11] = 0x00; // PZD5 addtional set value
	dest[12] = 0x00; // PZD6 addtional set value
	dest[13] = 0x00;
	dest[14] = 0x00;
	dest[15] = 0x00;
	dest[16] = 0x00;
	dest[17] = 0x00;
	dest[18] = 0x00;
	dest[19] = 0x00;
	dest[20] = 0x00;
	dest[21] = 0x00;
	dest[22] = 0x00;
	for(cnt=0;cnt<23;cnt++){
		bcc ^=dest[cnt];	
	}
	dest[23] = bcc; // XOR all
	return 24;
}

int VerifyInvREFUResponse(const unsigned char *src, unsigned char ID, unsigned short parameter)
{
	unsigned int cnt;
	unsigned char bcc = 0;
	unsigned char compareID = src[2];
	unsigned char length = src[1];
	if(!(src[0]==0x02 && src[4]==(unsigned char)parameter)) return REFU_ERROR_PACKET;
	
	compareID &= 0b00011111;
	if(ID!=compareID) return REFU_ERROR_ID;
	
	for(cnt=0;cnt<(length+1);cnt++){
		bcc ^= src[cnt];	
	}
	if(src[length+1]!=bcc) return REFU_ERROR_BCC;
	
	return length+2;
}

int GetInvREFUValue(unsigned long *dest, const unsigned char *src, char sort)
{ // little endian type
	union invData
	{
		float dataf;
		unsigned char dataArr[4];
	}invData;
	unsigned short valueS1;
	invData.dataArr[3] = src[7];
	invData.dataArr[2] = src[8];
	invData.dataArr[1] = src[9];
	invData.dataArr[0] = src[10];
	switch(sort){
	case REFU_VALUE_STATUS:
	// current operating state 
	// 0x00_initialization 0x01_deactivated 0x02_activation 0x03_ready 0x04_operation(run)
	// 0x05_stoppage 0x06_transient failure 0x07_failure
		*dest = (unsigned long)src[10];
		break;
	case REFU_VALUE_STATUSP:
	// return error code
	// REFU_FAULT_ERROR_OK		0x00000000 or
	// REFU_FAULT_GRID_OV		0x00090006 or
	// REFU_FAULT_GRID_UV		0x00090007 or
	// REFU_FAULT_OVER_F		0x00090008 or
	// REFU_FAULT_UNDER_F		0x00090009 or etc
		valueS1 = src[7];
		valueS1 = (valueS1<<8)&0xFF00;
		valueS1 |= src[8];
		*dest = valueS1;
		*dest = ((*dest)<<16)&0xFFFF0000;
		valueS1 = src[9];
		valueS1 = (valueS1<<8)&0xFF00;
		valueS1 |= src[10];
		*dest |= valueS1;
		break;
	case REFU_VALUE_PVV: // V
	case REFU_VALUE_PVP: // W
	case REFU_VALUE_GRIDRV: // V
	case REFU_VALUE_GRIDSV: // V
	case REFU_VALUE_GRIDTV: // V
	case REFU_VALUE_GRIDP: // W
		*dest = (unsigned long)invData.dataf;
		break;
	case REFU_VALUE_PVI: // A_one decimal point
	case REFU_VALUE_GRIDRI: // A_one decimal point
	case REFU_VALUE_GRIDSI: // A_one decimal point
	case REFU_VALUE_GRIDTI:	// A_one decimal point
		*dest = (unsigned long)(invData.dataf*10);
		break;
	case REFU_VALUE_DAILYP:	// kWh		
	case REFU_VALUE_TOTALP: // kWh
		*dest = (unsigned long)(invData.dataf/10);
		break;
	default:
		return 0;
	}
	return 1;
}
