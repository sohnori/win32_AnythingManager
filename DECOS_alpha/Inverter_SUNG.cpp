#include "Inverter_SUNG.h"

#define NULL	0

/***************************
*      Prevate Function    *
****************************/

static const short wCRCTable[] = {
	0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
	0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
	0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
	0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
	0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
	0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
	0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
	0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
	0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
	0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
	0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
	0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
	0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
	0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
	0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
	0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
	0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
	0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
	0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
	0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
	0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
	0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
	0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
	0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
	0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
	0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
	0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
	0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
	0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
	0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
	0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
	0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 
};

static unsigned short get_CRC16_MODBUS(const unsigned char* data, unsigned char len)
{
	unsigned char nTemp;
	unsigned char cnt = 0;
	unsigned short wCRCWord = 0xFFFF;

	while (len--) {
		nTemp = data[cnt] ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord ^= wCRCTable[nTemp];
		cnt++;
	}	
	return wCRCWord;		
} // source -  http://hks003.tistory.com/124

/***************************
*      Public Function     *
****************************/

int GetInvSUNGRequest
(unsigned char *dest, unsigned int szDest, unsigned char invID, unsigned char functionCode, unsigned short addr, unsigned short option)
{ // option - read data address counter	
	unsigned short crc16Modbus;
	if(szDest<8) return -1;
	addr -= 1;
	dest[0] = invID;
	dest[1] = functionCode;
	dest[2] = (unsigned char) (addr>>8);
	dest[3] = (unsigned char) addr;
	dest[4] = (unsigned char) (option>>8);
	dest[5] = (unsigned char) option;
	crc16Modbus = get_CRC16_MODBUS(dest, 6);
	dest[6] = (unsigned char)crc16Modbus;
	dest[7] = (unsigned char)(crc16Modbus>>8);
	return 8;
}

int VerifyInvSUNGResponse(const unsigned char *src, unsigned char invID, unsigned char functionCode)
{
	unsigned char length;
	unsigned short crc16Modbus;
	if (src[0]!=invID) return SUNG_ERROR_ID;
	if (src[1]!=functionCode) return SUNG_ERROR_PACKET;
	length = (unsigned char) src[2]; // load packet data length
	length += 3; // include packet header(inveter ID, function code, data length)
	crc16Modbus = get_CRC16_MODBUS(src, length);
	
	if(!(src[length]==(unsigned char)crc16Modbus && src[length+1]==(unsigned char)(crc16Modbus>>8)))
		return SUNG_ERROR_CRC16;	
	return length+2;
}

int GetInvSUNGValue(unsigned long *dest, const unsigned char *src, char sort)
{ // have to specify the data start point - response packet data start point is from packet[3]
	union invData{
		unsigned long valueL;
		unsigned short valueS;
		unsigned char valueArr[4];
	}invData;
	switch(sort){	
	case SUNG_VALUE_DEVTYPE:
	// U16
	// SG60KTL_0x010F SG60KU_0x0136 SG33KTL-M_0x0074 SG36KTL-M_0x0135 SG40KTL-M_0x0135 SG50KTL-M_0x011B
	// SG60KTL-M_0x0131 SG60KU-M_0x0132 SG49K5J_0x0137 SG10KTL-M_0x013E SG12KTL-M_0x013C SG80KTL_0x0138
	// SG80KTL-M_0x0139 SG80HV_0x013A SG125HV_0x013B SG33K3J_0x013D SG50KTL_0x0116		
	case SUNG_VALUE_NOMINALP:// U16 kW_one decimal point
	case SUNG_VALUE_OUTPUTTYPE: // U16 value - 0_two phase 1_3P4L 2_3P3L
	case SUNG_VALUE_DAILYP: // U16 kWh_one decimal point
	case SUNG_VALUE_INTERTEMP: // S16 celsius
	case SUNG_VALUE_DCV1: // V_one decimal point
	case SUNG_VALUE_DCI1: // A_one decimal point
	case SUNG_VALUE_DCV2:
	case SUNG_VALUE_DCI2:
	case SUNG_VALUE_DCV3:
	case SUNG_VALUE_DCI3:
	case SUNG_VALUE_DCV4:
	case SUNG_VALUE_DCI4:
	case SUNG_VALUE_DCV5:
	case SUNG_VALUE_DCI5:
	case SUNG_VALUE_ABV: // V_one decimal point
	case SUNG_VALUE_BCV:
	case SUNG_VALUE_CAV:
	case SUNG_VALUE_AI: // A_one decimal point
	case SUNG_VALUE_BI:
	case SUNG_VALUE_CI:
	case SUNG_VALUE_PF: // three decimal point >0_means leading <0_means lagging
	case SUNG_VALUE_FR: // Hz_onedecimal point
	case SUNG_VALUE_WORKSTATE:
	// device state
	// 0x0000_run 0x8000_stop 0x1300_key stop 0x1500_emerency srop 0x1400_standby 0x1200_initial standby
	// 0x1600_starting 0x9100_alarm run 0x8100_derating run 0x8200_dispatch run 0x5500_fault 0x2500_communication fault
		invData.valueS = src[0];
		invData.valueS = (invData.valueS<<8)&0xFF00;
		invData.valueS|= src[1];
		*dest = (unsigned long)invData.valueS;
		break;		
	case SUNG_VALUE_TOTALP: // U32 kWh
	case SUNG_VALUE_RUNNINGTIME: // U32 h
	case SUNG_VALUE_DCP: // W
	case SUNG_VALUE_GRIDP: // W
		invData.valueArr[3] = src[2];
		invData.valueArr[2] = src[3];
		invData.valueArr[1] = src[0];
		invData.valueArr[0] = src[1];
		*dest = invData.valueL;
		break;
	case SUNG_VALUE_WORKSTATE2:
	// 32 bit flag
	// 0_run 1_stop 3_key stop 5_emergency stop 4_standby 2_initial standby 6_starting
	// 10_alarm run 11_derating run 12_dispatch 9_fault 13_communicate fault 17_ total run bit
	// 18_total fault bit
		invData.valueArr[3] = src[0];
		invData.valueArr[2] = src[1];
		invData.valueArr[1] = src[2];
		invData.valueArr[0] = src[3];
		*dest = invData.valueL;
		break;
	default:
		return 0;		
	}
	return 1;
}

int GetInvSUNGValueStr(unsigned char *dest,unsigned int szDest,  const unsigned char *src, char sort)
{ // have to specify the data start point - response packet data start point is from packet[3]
	int cnt;
	if(szDest<15) return -1;
	switch(sort){
	case SUNG_VALUE_SERIALN:
	// addr 4990~4999 - U16*10(UTF-8)
		for(cnt=0;cnt<10;cnt++) dest[cnt] = src[3+cnt];
		break;
	case SUNG_VALUE_DEVTYPE:
	case SUNG_VALUE_NOMINALP:
	case SUNG_VALUE_OUTPUTTYPE:
	case SUNG_VALUE_DAILYP:
	case SUNG_VALUE_TOTALP:
	case SUNG_VALUE_RUNNINGTIME:
	case SUNG_VALUE_INTERTEMP:	
	case SUNG_VALUE_DCV1:
	case SUNG_VALUE_DCI1:
	case SUNG_VALUE_DCV2:
	case SUNG_VALUE_DCI2:
	case SUNG_VALUE_DCV3:
	case SUNG_VALUE_DCI3:
	case SUNG_VALUE_DCV4:
	case SUNG_VALUE_DCI4:
	case SUNG_VALUE_DCV5:
	case SUNG_VALUE_DCI5:
	case SUNG_VALUE_DCP:
	case SUNG_VALUE_ABV:
	case SUNG_VALUE_BCV:
	case SUNG_VALUE_CAV:
	case SUNG_VALUE_AI:
	case SUNG_VALUE_BI:
	case SUNG_VALUE_CI:
	case SUNG_VALUE_GRIDP:
	case SUNG_VALUE_PF:
	case SUNG_VALUE_FR:
	case SUNG_VALUE_WORKSTATE:
	case SUNG_VALUE_WORKSTATE2:
		break;
	default:
		return 0;		
	}
	return 1;
}