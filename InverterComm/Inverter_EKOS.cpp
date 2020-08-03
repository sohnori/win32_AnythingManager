#include "Inverter_EKOS.h"

typedef unsigned char u8int;

/***************************
*      Prevate Function     *
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

int GetInvEKOSRequest
(u8int *dest, unsigned int szDest, u8int ID, u8int FC, u8int addr, unsigned short length)
{
	unsigned short crc16;
	if(szDest<8) return -1;
	dest[0] = ID;
	dest[1] = FC;
	dest[2] = (unsigned char) (addr>>8);
	dest[3] = (unsigned char) addr;
	dest[4] = (unsigned char) (length>>8);
	dest[5] = (unsigned char) length;
	crc16 = get_CRC16_MODBUS(dest, 6);
	dest[6] = (unsigned char) crc16;
	dest[7] = (unsigned char) (crc16>>8);
	return 8;
}

int VerifyInvEKOSResponse(const unsigned char *src,  unsigned char ID, unsigned char FC)
{
	unsigned short crc16;
	if(src[0]!=ID)	return EKOS_ERROR_PACKET;
	if(src[1]!=FC)	return EKOS_ERROR_ID;
	crc16 = get_CRC16_MODBUS(src, src[2]+3);
	if(!(src[(src[2]+3)]==(unsigned char)crc16 && src[(src[2]+4)]==(unsigned char)(crc16>>8))) return EKOS_ERROR_CRC16;
	return 1;
}
	
int GetInvEKOSValue(unsigned long *dest, const unsigned char *src, char sort)
{ // can be use little endian type
// have to specify the data start point - response packet data start point is from packet[3]
	unsigned long totalH;
	unsigned long totalL;
	unsigned short value;
	union Data
	{
		float dataF;
		char dataArr[4];
	} Data;
	Data.dataArr[3] = src[0];
	Data.dataArr[2] = src[1];
	Data.dataArr[1] = src[2];
	Data.dataArr[0] = src[3];
	// if big endian type
	//Data.dataArr[0] = src[0];
	//Data.dataArr[1] = src[1];
	//Data.dataArr[2] = src[2];
	//Data.dataArr[3] = src[3];
	switch(sort){
	case EKOS_ADDR_PVV: // V
	case EKOS_ADDR_PVP: // W		
	case EKOS_ADDR_LINEV:// V
	case EKOS_ADDR_LINEP: // W		
	case EKOS_ADDR_TODAYP: // Wh		
	case EKOS_ADDR_LINERV: // V		
	case EKOS_ADDR_LINESV: // V		
	case EKOS_ADDR_LINETV: // V
		*dest = (unsigned long)Data.dataF;
		break;
	case EKOS_ADDR_PF: // %
		if(Data.dataF<0) Data.dataF *= -1;
		*dest = (unsigned long)Data.dataF;
		break;
	case EKOS_ADDR_PVI: // A_one decimal point
	case EKOS_ADDR_LINEI: // A_one decimal point
	case EKOS_ADDR_FR: // Hz_one decimal point		
	case EKOS_ADDR_LINERI: // A_one decimal point
	case EKOS_ADDR_LINESI: // A_one decimal point
	case EKOS_ADDR_LINETI: // A_one decimal point
		*dest = (unsigned long)(Data.dataF*10);
		break;
	case EKOS_ADDR_TOTALP: // kWh
		value = src[0];
		value = (value<<8)&0xFF00;
		value |= src[1];
		totalH = value;
		totalH = (totalH<<16)&0xFFFF0000;
		value = src[2];
		value = (value<<8)&0xFF00;
		value |= src[3];
		totalH |= value;
		value = src[4];
		value = (value<<8)&0xFF00;
		value |= src[5];
		totalL = value;
		totalL = (totalL<<16)&0xFFFF0000;
		value = src[6];
		value = (value<<8)&0xFF00;
		value |= src[7];
		totalL |= value;
		// convert totalH(MWh), totalL(Wh) to kWh
		*dest = (totalH*1000)+(totalL/1000);
		break;
	case EKOS_ADDR_PVSTATUS:
	// 16 bit flag if set to 1, occur fault.
	// bit15_PV fault bit14_emergency s/w on bit13_PV earth fault bit12_MCB open bit11_SPD fault
	// bit2_PV UV bit0_PV OV
	case EKOS_ADDR_INVSTATUS:
	// 16 bit flag if set to 1, occur fault.
	// bit15_Inverter fault bit14_1:run 0:stop bit12_MC off bit11_DCV unbalance bit10_INV PWM fault
	// bit9_key swiching status1:run 0:stop bit8_MC fault bit7_overheat bit4_overload
	// bit3_over current S/W bit2_over current H/W
	case EKOS_ADDR_GRIDSTATUS:	
	// 16 bit flag if set to 1, occur fault.
	// bit15_grid fault bit12_MCCB open bit11_SPD fault bit10,9_external force stop bit8_grid OVGR
	// bit5_grid earth fault bit4_grid power failure bit3_grid UF bit2_grid OF
	// bit1_grid UV bit0_grid OV
	case EKOS_ADDR_CONVERTSTATUS:
	// 16 bit flag if set to 1, occur fault.
	// bit15_converter fault bit9_overheat bit5_PWM fault bit4_over voltage
	// bit1_converter sensor offset error bit0_OC H/W
		*dest = src[0];
		break;
	default:
		return 0;
	}
	return 1;
}

