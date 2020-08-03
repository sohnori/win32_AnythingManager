#include "Equipment_REMS.h"

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

static unsigned short get_CRC16_MODBUS(const unsigned char* data, unsigned short len)
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

int GetREMSEquipTypeDefined(const unsigned char *src)
{
	if(src[0]=='P' && src[1]=='V' && src[2]=='1' && src[3]=='P') return REMS_ENERGYTYPE_485_PV1P;
	if(src[0]=='P' && src[1]=='V' && src[2]=='3' && src[3]=='P') return REMS_ENERGYTYPE_485_PV3P;
	if(src[0]=='P' && src[1]=='V' && src[2]=='H' && src[3]=='F') return REMS_ENERGYTYPE_485_PVHF;
	if(src[0]=='P' && src[1]=='V' && src[2]=='H' && src[3]=='N') return REMS_ENERGYTYPE_485_PVHN;
	if(src[0]=='G' && src[1]=='E' && src[2]=='O' && src[3]=='T') return REMS_ENERGYTYPE_485_GEOT;
	if(src[0]=='W' && src[1]=='I' && src[2]=='N' && src[3]=='D') return REMS_ENERGYTYPE_485_WIND;
	if(src[0]=='F' && src[1]=='U' && src[2]=='E' && src[3]=='L') return REMS_ENERGYTYPE_485_FUEL;
	if(src[0]=='E' && src[1]=='S' && src[2]=='S' && src[3]=='S') return REMS_ENERGYTYPE_485_ESSS;
	return 0;
}

int GetREMSEquipRequestP(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char equipType, unsigned char typeSub)
{	
	unsigned short crc16_Modbus;
	if(szDest<REMS_MAXSIZE_TX_PACKET) return -1;
	dest[0] = 0x7E;
	dest[1] = ID;
	switch(equipType){	
	case REMS_ENERGYTYPE_485_PVHF:
		if(typeSub==REMS_CMD_PVH_LOAD) dest[2] = 0x03;
		else if(typeSub==REMS_CMD_PVH_SUPPLY) dest[2] = 0x01;
		else return 0;
		break;	
	case REMS_ENERGYTYPE_485_GEOT:
		if(typeSub==REMS_CMD_GEOTHERMY_H_C) dest[2] = 0x03;
		else if(typeSub==REMS_CMD_GEOTHERMY_HWS) dest[2] = 0x05;
		else if(typeSub==REMS_CMD_GEOTHERMY_HPUMP) dest[2] = 0x01;
		else return 0;
		break;
	case REMS_ENERGYTYPE_485_PV1P:
	case REMS_ENERGYTYPE_485_PVHN:
	case REMS_ENERGYTYPE_485_WIND:
	case REMS_ENERGYTYPE_485_FUEL:
	case REMS_ENERGYTYPE_485_ESSS:
		dest[2] = 0x01;
		break;
	case REMS_ENERGYTYPE_485_PV3P:
		dest[2] = 0x07;
		break;
	default:
		return 0;
	}
	crc16_Modbus = get_CRC16_MODBUS(dest, 3);
	dest[3] = (unsigned char)crc16_Modbus;
	dest[4] = (unsigned char)(crc16_Modbus>>8);
	return 5;
}

int VerifyREMSEquipResponseP(const unsigned char *src,  unsigned char ID, unsigned char equipType, unsigned char typeSub)
{
	unsigned short length;
	unsigned short crc16_Modbus;
	switch(equipType){	
	case REMS_ENERGYTYPE_485_PVHF:
		if(typeSub==REMS_CMD_PVH_LOAD) {
			if(!(src[0]==0x7E && src[1]==ID && src[2]==0x04)) return REMS_ERROR_PACKET;
		}
		else if(typeSub==REMS_CMD_PVH_SUPPLY){
			if(!(src[0]==0x7E && src[1]==ID && src[2]==0x02)) return REMS_ERROR_PACKET;
		}
		else return 0;
		break;	
	case REMS_ENERGYTYPE_485_GEOT:
		if(typeSub==REMS_CMD_GEOTHERMY_H_C){
			if(!(src[0]==0x7E && src[1]==ID && src[2]==0x04)) return REMS_ERROR_PACKET;
		}
		else if(typeSub==REMS_CMD_GEOTHERMY_HWS) {
			if(!(src[0]==0x7E && src[1]==ID && src[2]==0x06)) return REMS_ERROR_PACKET;
		}
		else if(typeSub==REMS_CMD_GEOTHERMY_HPUMP){
			if(!(src[0]==0x7E && src[1]==ID && src[2]==0x02)) return REMS_ERROR_PACKET;
		}
		else return 0;
		break;
	case REMS_ENERGYTYPE_485_PV1P:
	case REMS_ENERGYTYPE_485_PVHN:
	case REMS_ENERGYTYPE_485_WIND:
	case REMS_ENERGYTYPE_485_FUEL:
	case REMS_ENERGYTYPE_485_ESSS:
		if(!(src[0]==0x7E && src[1]==ID && src[2]==0x02)) return REMS_ERROR_PACKET;
		break;
	case REMS_ENERGYTYPE_485_PV3P:
		if(!(src[0]==0x7E && src[1]==ID && src[2]==0x08)) return REMS_ERROR_PACKET;
		break;
	default:
		return 0;
	}	
	length = (unsigned char) src[3];
	length <<= 8;
	length |= (unsigned char) src[4]; // load packet data length
	length += 5; // include packet header(start of packet, ID, cmd, data length)
	crc16_Modbus = get_CRC16_MODBUS(src, length);
	if(!(src[length]==(unsigned char)crc16_Modbus && src[length+1]==(unsigned char)(crc16_Modbus>>8))) return REMS_ERROR_CRC;
	
	return length+2; // return packet size
}

int GetREMSEquipRequest(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char cmd)
{
	unsigned short crc16_Modbus;
	if(szDest<REMS_MAXSIZE_TX_PACKET) return -1;
	dest[0] = 0x7E;
	dest[1] = ID;
	dest[2] = cmd;
	crc16_Modbus = get_CRC16_MODBUS(dest, 3);
	dest[3] = (unsigned char)crc16_Modbus;
	dest[4] = (unsigned char)(crc16_Modbus>>8);
	return 5;
}

int VerifyREMSEquipResponse(const unsigned char *src,  unsigned char ID, unsigned char cmd)
{
	unsigned short length;
	unsigned short crc16_Modbus;	
	if(!(src[0]==0x7E && src[1]==ID && src[2]==(cmd+1))) return REMS_ERROR_PACKET;
	length = (unsigned char) src[3];
	length <<= 8;
	length |= (unsigned char) src[4]; // load packet data length
	length += 5; // include packet header(start of packet, ID, cmd, data length)
	crc16_Modbus = get_CRC16_MODBUS(src, length);
	if(!(src[length]==(unsigned char)crc16_Modbus && src[length+1]==(unsigned char)(crc16_Modbus>>8))) return REMS_ERROR_CRC;
	
	return length+2; // return packet size
}

int GetREMSEquipData(unsigned char *dest, unsigned int szDest, const unsigned char *src)
{
	unsigned int cnt;
	unsigned short length;
	if(szDest<REMS_MAXSIZE_RX_PACKET) return -1;
	length = src[3];
	length = (length<<8)&0xFF00;
	length |= (unsigned short)src[4];
	for(cnt=0;cnt<length;cnt++){
		dest[cnt] = src[6+cnt];	
	}
	return cnt;
}

int GetREMSEquipCombinedData
(unsigned char *dest, unsigned int szDest, const unsigned char *src1, const unsigned char *src2, unsigned char sort)
{
	unsigned int cnt1;
	unsigned int cnt2;
	if(szDest<REMS_MAXSIZE_COMBINEDDATA) return -1;
	switch(sort){
	case REMS_DATA_PVH:
		for(cnt1=0;cnt1<25;cnt1++){
			dest[cnt1] = src1[cnt1];
		}
		for(cnt2=5;cnt2<(16+5);cnt2++){
			dest[cnt1+cnt2-5] = src2[cnt2];	
		}
		dest[cnt1+cnt2-5] = src1[cnt1];
		dest[cnt1+cnt2-5+1] = src1[cnt1+1];
		dest[cnt1+cnt2-5+2] = 0x00; // crc high null value
		dest[cnt1+cnt2-5+3] = 0x00; // crc low null value
		return cnt1+cnt2-5+4;
	case REMS_DATA_GEOTHERMY:
		for(cnt1=0;cnt1<21;cnt1++){
			dest[cnt1] = src1[cnt1];
		}
		for(cnt2=5;cnt2<(16+5);cnt2++){
			dest[cnt1+cnt2-5] = src2[cnt2];	
		}		
		dest[cnt1+cnt2-5] = 0x00; // crc high null value
		dest[cnt1+cnt2-5+1] = 0x00; // crc low null value
		return cnt1+cnt2-5+2;
	default:
		return 0;
	}
}

int GenREMSLoraPacket(unsigned char *dest, unsigned int szDest, const unsigned char *src, unsigned int szSrc, struct tagREMSLoraH header)
{
	unsigned int cnt;
	if(szDest<REMS_MAXSIZE_LORAPACKET) return -1;
	dest[0] = 0x14; // command fixed
	dest[1] = header.type; // energy type code
	dest[2] = header.subType; // energy type sort code
	dest[3] = header.multyCounter;
	dest[4] = header.errorCode;
	if((int)(szSrc-2)<=5) return 5;
	for(cnt=5;cnt<(szSrc-2);cnt++){ // from data start to before CRC
		if(cnt>REMS_MAXSIZE_LORAPACKET-1) return -1;
		dest[cnt] = src[cnt];	
	}
	return cnt;
}

unsigned char GetREMSLoraType(unsigned char equipType)
{
	switch(equipType){
	case REMS_ENERGYTYPE_485_PV1P:
	case REMS_ENERGYTYPE_485_PV3P:
		return REMS_ENERGYTYPE_LORA_PV;
	case REMS_ENERGYTYPE_485_PVHF:
	case REMS_ENERGYTYPE_485_PVHN:
		return REMS_ENERGYTYPE_LORA_PVH;
	case REMS_ENERGYTYPE_485_GEOT:
		return REMS_ENERGYTYPE_LORA_GEOTHERMY;
	case REMS_ENERGYTYPE_485_WIND:
		return REMS_ENERGYTYPE_LORA_WINDPOWER;
	case REMS_ENERGYTYPE_485_FUEL:
		return REMS_ENERGYTYPE_LORA_FUELCELL;
	case REMS_ENERGYTYPE_485_ESSS:
		return REMS_ENERGYTYPE_LORA_ESS;
	default:
		return 0;
	}	
}

unsigned char GetREMSLoraSubType(unsigned char equipType)
{
	switch(equipType){
	case REMS_ENERGYTYPE_485_PV1P:
		return REMS_ENERGYTYPE_LORA_PV_ONE;
	case REMS_ENERGYTYPE_485_PV3P:
		return REMS_ENERGYTYPE_LORA_PV_THREE;
	case REMS_ENERGYTYPE_485_PVHF:
		return REMS_ENERGYTYPE_LORA_PVH_FORCE;
	case REMS_ENERGYTYPE_485_PVHN:
		return REMS_ENERGYTYPE_LORA_PVH_NATURE;
	case REMS_ENERGYTYPE_485_GEOH:
		return REMS_ENERGYTYPE_LORA_GEOTHERMY_HP;
	case REMS_ENERGYTYPE_485_GEOL:
		return REMS_ENERGYTYPE_LORA_GEOTHERMY_LOAD;
	case REMS_ENERGYTYPE_485_WIND:		
	case REMS_ENERGYTYPE_485_FUEL:		
	case REMS_ENERGYTYPE_485_ESSS:
		return REMS_ENERGYTYPE_LORA_ETC;
	default:
		return 0;
	}
}
