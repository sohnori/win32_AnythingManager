#include "Inverter_DASS.h"

#define NULL	0

/***************************
*      Public Function     *
****************************/

int GetInvDASSRequest(char *dest, unsigned int szDest, char srcID[], unsigned char srcCmd)
{
	if(szDest<8) return -1;
	switch (srcCmd){
	case DASS_CMD_MOD:		
		dest[5] = 'M';
		dest[6] = 'O';
		dest[7] = 'D';
		break;
	case DASS_CMD_ST1:		
		dest[5] = 'S';
		dest[6] = 'T';
		dest[7] = '1';
		break;
	case DASS_CMD_ST2:		
		dest[5] = 'S';
		dest[6] = 'T';
		dest[7] = '2';
		break;
	case DASS_CMD_ST3:		
		dest[5] = 'S';
		dest[6] = 'T';
		dest[7] = '3';
		break;
	case DASS_CMD_ST4:
		dest[5] = 'S';
		dest[6] = 'T';
		dest[7] = '4';
		break;
	case DASS_CMD_ST5:		
		dest[5] = 'S';
		dest[6] = 'T';
		dest[7] = '5';
		break;
	case DASS_CMD_ST6:
		dest[5] = 'S';
		dest[6] = 'T';
		dest[7] = '6';
		break;
	default:
		return 0;
	}
	dest[0] = '^';
	dest[1] = 'P';
	dest[2] = srcID[0];
	dest[3] = srcID[1];
	dest[4] = srcID[2];
	return 8;
}

int VerifyInvDASSResponse(const unsigned char *src, char srcID[], unsigned char srcCmd)
{
	unsigned char cksum;
	unsigned char length;
	unsigned char value = 0;
	if(!(src[5]==srcID[0] && src[6]==srcID[1] && src[7]==srcID[2])) return DASS_ERROR_ID;
	length = (src[3] - 0x30)*10;
	length += (src[4] -0x30);
	cksum = (src[length+3] - 0x30)*10;
	cksum += (src[length+4] -0x30);
	switch (srcCmd){
	case DASS_CMD_MOD:
		if(!(src[0]=='^' && src[1]=='D' && src[2]=='0')) return DASS_ERROR_PACKET;
		value = src[2]-0x30;
		value += src[3]-0x30;
		value += src[4]-0x30;
		value += src[5]-0x30;
		value += src[6]-0x30;
		value += src[7]-0x30;
		value += src[9]-0x30;
		value += src[11]-0x30;
		value += src[12]-0x30;
		value += src[13]-0x30;
		value += src[14]-0x30;
		value += src[16]-0x30;
		value += src[17]-0x30;
		value += src[18]-0x30;		
		break;
	case DASS_CMD_ST1:
		if(!(src[0]=='^' && src[1]=='D' && src[2]=='1')) return DASS_ERROR_PACKET;
		value = src[2]-0x30;
		value += src[3]-0x30;
		value += src[4]-0x30;
		value += src[5]-0x30;
		value += src[6]-0x30;
		value += src[7]-0x30;
		value += src[9]-0x30;
		value += src[10]-0x30;
		value += src[11]-0x30;
		value += src[13]-0x30;
		value += src[14]-0x30;
		value += src[15]-0x30;
		value += src[16]-0x30;
		value += src[18]-0x30;
		value += src[19]-0x30;
		value += src[20]-0x30;
		value += src[21]-0x30;		
		break;
	case DASS_CMD_ST2:
		if(!(src[0]=='^' && src[1]=='D' && src[2]=='2')) return DASS_ERROR_PACKET;
		value = src[2]-0x30;
		value += src[3]-0x30;
		value += src[4]-0x30;
		value += src[5]-0x30;
		value += src[6]-0x30;
		value += src[7]-0x30;
		value += src[9]-0x30;
		value += src[10]-0x30;
		value += src[11]-0x30;
		value += src[13]-0x30;
		value += src[14]-0x30;
		value += src[15]-0x30;
		value += src[17]-0x30;
		value += src[18]-0x30;
		value += src[19]-0x30;
		value += src[21]-0x30;
		value += src[22]-0x30;
		value += src[23]-0x30;
		break;
	case DASS_CMD_ST3:
		if(!(src[0]=='^' && src[1]=='D' && src[2]=='3')) return DASS_ERROR_PACKET;
		value = src[2]-0x30;
		value += src[3]-0x30;
		value += src[4]-0x30;
		value += src[5]-0x30;
		value += src[6]-0x30;
		value += src[7]-0x30;
		value += src[9]-0x30;
		value += src[10]-0x30;
		value += src[11]-0x30;
		value += src[12]-0x30;
		value += src[14]-0x30;
		value += src[15]-0x30;
		value += src[16]-0x30;
		value += src[17]-0x30;
		value += src[19]-0x30;
		value += src[20]-0x30;
		value += src[21]-0x30;
		value += src[22]-0x30;
		break;
	case DASS_CMD_ST4:
		if(!(src[0]=='^' && src[1]=='D' && src[2]=='4')) return DASS_ERROR_PACKET;
		value = src[2]-0x30;
		value += src[3]-0x30;
		value += src[4]-0x30;
		value += src[5]-0x30;
		value += src[6]-0x30;
		value += src[7]-0x30;
		value += src[9]-0x30;
		value += src[10]-0x30;
		value += src[11]-0x30;
		value += src[12]-0x30;
		value += src[14]-0x30;
		value += src[15]-0x30;
		value += src[16]-0x30;
		value += src[17]-0x30;
		value += src[18]-0x30;
		value += src[19]-0x30;
		value += src[20]-0x30;		
		break;
	case DASS_CMD_ST5:
		if(!(src[0]=='^' && src[1]=='D' && src[2]=='5')) return DASS_ERROR_PACKET;
		value = src[2]-0x30;
		value += src[3]-0x30;
		value += src[4]-0x30;
		value += src[5]-0x30;
		value += src[6]-0x30;
		value += src[7]-0x30;
		value += src[9]-0x30;
		value += src[10]-0x30;
		value += src[11]-0x30;
		value += src[12]-0x30;
		value += src[14]-0x30;
		value += src[15]-0x30;
		value += src[16]-0x30;
		value += src[17]-0x30;
		value += src[19]-0x30;
		value += src[20]-0x30;
		value += src[21]-0x30;
		value += src[22]-0x30;
		value += src[24]-0x30;
		value += src[25]-0x30;
		value += src[26]-0x30;
		value += src[27]-0x30;
		break;
	case DASS_CMD_ST6: // cksum_calcurate fault code A~Z to 10~35(decimal)
		if(!(src[0]=='^' && src[1]=='D' && src[2]=='6')) return DASS_ERROR_PACKET;
		/*value = src[2]-0x30;
		value += src[3]-0x30;
		value += src[4]-0x30;
		value += src[5]-0x30;
		value += src[6]-0x30;
		value += src[7]-0x30;
		value += src[9]-0x30;
		value += src[11]-0x30;
		value += src[13]-0x30;*/
		break;
	default:
		return 0;
	}
	if (cksum!=value && srcCmd!=DASS_CMD_ST6) return DASS_ERROR_CKSUM;	
	return 1;
}

int GetInvDASSValue(unsigned long *dest, const unsigned char *src, char sort)
{
	switch (sort){
	case DASS_INFO_ID:
		*dest = (unsigned long)(src[5]-0x30)*100;
		*dest += (src[5]-0x30)*10;
		*dest += (src[7]-0x30);
		break;
	case DASS_INFO_PHASE:
		*dest = src[9]-0x30;
		break;
	case DASS_INFO_CAPACITY: // kW_one decimal point
		*dest = (unsigned long)(src[11]-0x30)*1000;
		*dest += (unsigned long)(src[12]-0x30)*100;
		*dest += (src[13]-0x30)*10;
		*dest += (src[14]-0x30);
		break;
	case DASS_INFO_GRIDV: // V
		*dest = (unsigned long)(src[16]-0x30)*100;
		*dest += (src[17]-0x30)*10;
		*dest += (src[18]-0x30);
		break;
	case DASS_VALUE_PVV: // V
		*dest = (unsigned long)(src[9]-0x30)*100;
		*dest += (src[10]-0x30)*10;
		*dest += (src[11]-0x30);
		break;
	case DASS_VALUE_PVI: // A_one decimal point	
		*dest = (unsigned long)(src[13]-0x30)*1000;
		*dest += (unsigned long)(src[14]-0x30)*100;
		*dest += (src[15]-0x30)*10;
		*dest += (src[16]-0x30);
		break;
	case DASS_VALUE_PVP: // kW_one decimal point	
		*dest = (unsigned long)(src[18]-0x30)*1000;
		*dest += (unsigned long)(src[19]-0x30)*100;
		*dest += (src[20]-0x30)*10;
		*dest += (src[21]-0x30);
		break;
	case DASS_VALUE_GRIDRSV: // V
		*dest = (unsigned long)(src[9]-0x30)*100;
		*dest += (src[10]-0x30)*10;
		*dest += (src[11]-0x30);		
		break;
	case DASS_VALUE_GRIDSTV: // V
		*dest = (unsigned long)(src[13]-0x30)*100;
		*dest += (src[14]-0x30)*10;
		*dest += (src[15]-0x30);		
		break;
	case DASS_VALUE_GRIDTRV: // V
		*dest = (unsigned long)(src[17]-0x30)*100;
		*dest += (src[18]-0x30)*10;
		*dest += (src[19]-0x30);		
		break;
	case DASS_VALUE_GRIDFR: // Hz_one decimal point	
		*dest = (unsigned long)(src[21]-0x30)*100;
		*dest += (src[22]-0x30)*10;
		*dest += (src[23]-0x30);		
		break;
	case DASS_VALUE_GRIDRI: // A_one decimal point	
		*dest = (unsigned long)(src[9]-0x30)*1000;
		*dest += (unsigned long)(src[10]-0x30)*100;
		*dest += (src[11]-0x30)*10;
		*dest += (src[12]-0x30);
		break;
	case DASS_VALUE_GRIDSI: // A_one decimal point
		*dest = (unsigned long)(src[14]-0x30)*1000;
		*dest += (unsigned long)(src[15]-0x30)*100;
		*dest += (src[16]-0x30)*10;
		*dest += (src[17]-0x30);
		break;
	case DASS_VALUE_GRIDTI: // A_one decimal point
		*dest = (unsigned long)(src[19]-0x30)*1000;
		*dest += (unsigned long)(src[20]-0x30)*100;
		*dest += (src[21]-0x30)*10;
		*dest += (src[22]-0x30);
		break;
	case DASS_VALUE_GENP: // kW_one decimal point
		*dest = (unsigned long)(src[9]-0x30)*1000;
		*dest += (unsigned long)(src[10]-0x30)*100;
		*dest += (src[11]-0x30)*10;
		*dest += (src[12]-0x30);
		break;
	case DASS_VALUE_TOTALP: // kWh
		*dest = (unsigned long)(src[14]-0x30)*1000000;
		*dest += (unsigned long)(src[15]-0x30)*100000;
		*dest += (unsigned long)(src[16]-0x30)*10000;
		*dest += (unsigned long)(src[17]-0x30)*1000;
		*dest += (unsigned long)(src[18]-0x30)*100;
		*dest += (src[19]-0x30)*10;
		*dest += (src[20]-0x30);
		break;
	case DASS_VALUE_GRIDSTATUS: // 0x31_line power loss or 0x30_normal
		*dest = src[9];
		break;
	case DASS_VALUE_STATUS: // 0x31_stop or 0x30_run
		*dest = src[11];
		break;
	case DASS_VALUE_FAULT: 
	// 0x30_normal 0x31_earth fault 0x32_over heat 0x33_DC over voltage 0x34_ MC error 0x35_DC link error
	// 0x36_output over current 0x37_PV error(reverse) 0x38_line connected(stand alone inverter)
	// 0x39_over current(IGBT) 0x41,0x42,0x43_over current(IGBT) 0x44_reverse phase 0x4B_line OF
	// 0x4C_line UF 0x4D_line OV 0x4E_line UV 
		*dest = src[13];
		break;
	case DASS_VALUE_HORIZON: // W/m^2
		*dest = (unsigned long)(src[9]-0x30)*1000;
		*dest += (unsigned long)(src[10]-0x30)*100;
		*dest += (src[11]-0x30)*10;
		*dest += (src[12]-0x30);
		break;
	case DASS_VALUE_TILT: // W/m^2
		*dest = (unsigned long)(src[14]-0x30)*1000;
		*dest += (unsigned long)(src[15]-0x30)*100;
		*dest += (src[16]-0x30)*10;
		*dest += (src[17]-0x30);
		break;
	case DASS_VALUE_OUTDOOR: // celsius_one decimal point
		*dest = (unsigned long)(src[19]-0x30)*1000;
		*dest += (unsigned long)(src[20]-0x30)*100;
		*dest += (src[21]-0x30)*10;
		*dest += (src[22]-0x30);
		break;
	case DASS_VALUE_MODULE: // celsius_one decimal point
		*dest = (unsigned long)(src[24]-0x30)*1000;
		*dest += (unsigned long)(src[25]-0x30)*100;
		*dest += (src[26]-0x30)*10;
		*dest += (src[27]-0x30);
		break;
	default:
		return 0;
	}
	return 1;
}

int GetInvDASSValueStr(unsigned char *dest, const unsigned char *src, char sort)
{
	switch (sort){
	case DASS_INFO_ID:
		dest[0] = src[5];
		dest[1] = src[6];
		dest[2] = src[7];
		dest[3] = NULL;
		return 3;
	case DASS_INFO_PHASE: // ASCII 1 or 3
		dest[0] = src[9];
		dest[1] = NULL;
		return 1;
	case DASS_INFO_CAPACITY: // kW_one decimal point
		dest[0] = src[11];
		dest[1] = src[12];
		dest[2] = src[13];
		dest[3] = src[14];
		dest[4] = NULL;
		return 4;
	case DASS_INFO_GRIDV: // V
		dest[0] = src[16];
		dest[1] = src[17];
		dest[2] = src[18];
		dest[3] = NULL;
		return 3;
	case DASS_VALUE_PVV: // V
		dest[0] = src[9];
		dest[1] = src[10];
		dest[2] = src[11];
		dest[3] = NULL;
		return 3;
	case DASS_VALUE_PVI: // A_one decimal point	
		dest[0] = src[13];
		dest[1] = src[14];
		dest[2] = src[15];
		dest[3] = src[16];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_PVP: // kW_one decimal point	
		dest[0] = src[18];
		dest[1] = src[19];
		dest[2] = src[20];
		dest[3] = src[21];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_GRIDRSV: // V
		dest[0] = src[9];
		dest[1] = src[10];
		dest[2] = src[11];
		dest[3] = NULL;
		return 3;
	case DASS_VALUE_GRIDSTV: // V
		dest[0] = src[13];
		dest[1] = src[14];
		dest[2] = src[15];
		dest[3] = NULL;
		return 3;
	case DASS_VALUE_GRIDTRV: // V
		dest[0] = src[17];
		dest[1] = src[18];
		dest[2] = src[19];
		dest[3] = NULL;
		return 3;
	case DASS_VALUE_GRIDFR: // Hz_one decimal point	
		dest[0] = src[21];
		dest[1] = src[22];
		dest[2] = src[23];
		dest[3] = NULL;
		return 3;
	case DASS_VALUE_GRIDRI: // A_one decimal point	
		dest[0] = src[9];
		dest[1] = src[10];
		dest[2] = src[11];
		dest[3] = src[12];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_GRIDSI: // A_one decimal point
		dest[0] = src[14];
		dest[1] = src[15];
		dest[2] = src[16];
		dest[3] = src[17];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_GRIDTI: // A_one decimal point
		dest[0] = src[19];
		dest[1] = src[21];
		dest[2] = src[22];
		dest[3] = src[23];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_GENP: // kW_one decimal point
		dest[0] = src[9];
		dest[1] = src[10];
		dest[2] = src[11];
		dest[3] = src[12];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_TOTALP: // kWh
		dest[0] = src[14];
		dest[1] = src[15];
		dest[2] = src[16];
		dest[3] = src[17];
		dest[4] = src[18];
		dest[5] = src[19];
		dest[6] = src[20];		
		dest[7] = NULL;
		return 7;
	case DASS_VALUE_GRIDSTATUS: // 0x31_line power loss or 0x30_normal
		dest[0] = src[9];
		dest[1] = NULL;
		return 1;
	case DASS_VALUE_STATUS: // 0x31_stop or 0x30_run
		dest[0] = src[11];
		dest[1] = NULL;
		return 1;
	case DASS_VALUE_FAULT: 
	// 0x30_normal 0x31_earth fault 0x32_over heat 0x33_DC over voltage 0x34_ MC error 0x35_DC link error
	// 0x36_output over current 0x37_PV error(reverse) 0x38_line connected(stand alone inverter)
	// 0x39_over current(IGBT) 0x41,0x42,0x43_over current(IGBT) 0x44_reverse phase 0x4B_line OF
	// 0x4C_line UF 0x4D_line OV 0x4E_line UV 
		dest[0] = src[13];
		dest[1] = NULL;
		return 1;
	case DASS_VALUE_HORIZON: // W/m^2
		dest[0] = src[9];
		dest[1] = src[10];
		dest[2] = src[11];
		dest[3] = src[12];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_TILT: // W/m^2
		dest[0] = src[14];
		dest[1] = src[15];
		dest[2] = src[16];
		dest[3] = src[17];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_OUTDOOR: // celsius_one decimal point
		dest[0] = src[19];
		dest[1] = src[20];
		dest[2] = src[21];
		dest[3] = src[22];
		dest[4] = NULL;
		return 4;
	case DASS_VALUE_MODULE: // celsius_one decimal point
		dest[0] = src[24];
		dest[1] = src[25];
		dest[2] = src[26];
		dest[3] = src[27];
		dest[4] = NULL;
		return 4;
	default:
		return 0;
	}
}
