#include "Inverter_HEXP.h"

#define BIN	2
#define DEC	10
#define HEX	16

#define UPPERCASE	85
#define LOWERCASE	76

/***************************
*      Prevate Function    *
****************************/

static char HEXtoASCIICHAR(unsigned char src, int option)
{
	if (src>=0x00 && src<=0x09) return src += 0x30;
	else if (option==UPPERCASE && (src>=0x0A && src<=0x0F)) return src += 0x37;
	else if (option==LOWERCASE && (src>=0x0A && src<=0x0F)) return src += 0x57;
	else return -1;
}

static short HEXtoASCII2Byte(unsigned char src, int option)
{
	unsigned char nibbleH = (src>>4) & 0x0F;
	unsigned char nibbleL = src & 0x0F;
	unsigned short ret;
	ret = HEXtoASCIICHAR(nibbleH, option);
	ret = (ret<<8) & 0xFF00;
	ret |= HEXtoASCIICHAR(nibbleL, option);
	return ret;
}

static unsigned char ASCII2CHARtoHEX(unsigned char nibbleH, unsigned char nibbleL, int option)
{
	if ((nibbleH >= 0x30) && (nibbleH <= 0x39)) nibbleH -= 0x30;
	else if (option==HEX && (nibbleH >= 0x41) && (nibbleH <= 0x46)) nibbleH -=0x37;
	else if (option==HEX && (nibbleH >= 0x61) && (nibbleH <= 0x66)) nibbleH -=0x57;
	else return 0xFF;

	if ((nibbleL >= 0x30) && (nibbleL <= 0x39))	nibbleL -= 0x30;
	else if (option==HEX && (nibbleL >= 0x41) && (nibbleL <= 0x46)) nibbleL -=0x37;
	else if (option==HEX && (nibbleL >= 0x61) && (nibbleL <= 0x66)) nibbleL -=0x57;
	else return 0xFF;

	nibbleL |= (nibbleH<<4) & 0xf0;
	if (option==DEC) { // convert ASCII decimal value to HEX
		if (nibbleL>=0x10 && nibbleL<0x20) return nibbleL -= 6;
		if (nibbleL>=0x20 && nibbleL<0x30) return nibbleL -= 12;
		if (nibbleL>=0x30 && nibbleL<0x40) return nibbleL -= 18;
		if (nibbleL>=0x40 && nibbleL<0x50) return nibbleL -= 24;
		if (nibbleL>=0x50 && nibbleL<0x60) return nibbleL -= 30;
		if (nibbleL>=0x60 && nibbleL<0x70) return nibbleL -= 36;
		if (nibbleL>=0x70 && nibbleL<0x80) return nibbleL -= 42;
		if (nibbleL>=0x80 && nibbleL<0x90) return nibbleL -= 48;
		if (nibbleL>=0x90) return nibbleL -= 54;	
	}	
	return nibbleL;	
}

static int VerifyInvHEXPCksum(const unsigned char *srcCksum, unsigned short valueCksum)
{
	unsigned char compareCksum[4];
	unsigned short cksum = 0;
	cksum = HEXtoASCII2Byte((unsigned char)(valueCksum>>8), LOWERCASE);
	compareCksum[0] = (unsigned char)(cksum>>8); // check sum MSB
	compareCksum[1] = (unsigned char)cksum;
	cksum = HEXtoASCII2Byte((unsigned char)valueCksum, LOWERCASE);
	compareCksum[2] = (unsigned char)(cksum>>8);
	compareCksum[3] = (unsigned char)cksum; // check sum LSB
	
	if(!(srcCksum[0]==compareCksum[0] && srcCksum[1]==compareCksum[1] && \
	srcCksum[2]==compareCksum[2] && srcCksum[3]==compareCksum[3]))	return HEXP_ERROR_CKSUM;
	
	return 1;
}

/***************************
*      Public Function     *
****************************/

int GetInvHEXPRequest(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char parameter)
{
	unsigned short cksum = 0;
	unsigned int cnt;
	unsigned short value = HEXtoASCII2Byte(ID, 0);
	if(szDest<15) return -1;
	dest[0] = 0x05; //ENQ
	dest[1] = (unsigned char)(value>>8); // ID high
	dest[2] = (unsigned char) value; // ID low
	dest[3] = 'R'; // command code
	switch(parameter){
	case HEXP_PARA_FAULT: // ASCII 0004
		dest[4] = 0x30; // parameter MSB
		dest[5] = 0x30;
		dest[6] = 0x30;
		dest[7] = 0x34; // parameter LSB
		dest[8] = 0x30; // data count high
		dest[9] = 0x34; // data count low
		break;
	case HEXP_PARA_PV: // ASCII 0020
		dest[4] = 0x30;
		dest[5] = 0x30;
		dest[6] = 0x32;
		dest[7] = 0x30;
		dest[8] = 0x30; // data count high
		dest[9] = 0x32; // data count low
		break;
	case HEXP_PARA_GRID: // ASCII 0050
		dest[4] = 0x30;
		dest[5] = 0x30;
		dest[6] = 0x35;
		dest[7] = 0x30;
		dest[8] = 0x30; // data count high
		dest[9] = 0x37; // data count low
		break;
	case HEXP_PARA_POWER: // ASCII 0060
		dest[4] = 0x30;
		dest[5] = 0x30;
		dest[6] = 0x36;
		dest[7] = 0x30;
		dest[8] = 0x30; // data count high
		dest[9] = 0x38; // data count low
		break;
	case HEXP_PARA_SYSTEM: // ASCII 01e0
		dest[4] = 0x30;
		dest[5] = 0x31;
		dest[6] = 0x65;
		dest[7] = 0x30;
		dest[8] = 0x30; // data count high
		dest[9] = 0x33; // data count low
		break;
	case HEXP_PARA_ENVIRON: // ASCII 0070
		dest[4] = 0x30;
		dest[5] = 0x30;
		dest[6] = 0x37;
		dest[7] = 0x30;
		dest[8] = 0x30; // data count high
		dest[9] = 0x34; // data count low
		break;
	default:
		return -1;
		
	}
	value=0;
	for(cnt=1;cnt<10;cnt++) {
		value +=dest[cnt];
	}
	cksum = HEXtoASCII2Byte((unsigned char)(value>>8), LOWERCASE);
	dest[10] = (unsigned char)(cksum>>8); // check sum MSB
	dest[11] = (unsigned char)cksum;
	cksum = HEXtoASCII2Byte((unsigned char)value, LOWERCASE);
	dest[12] = (unsigned char)(cksum>>8);
	dest[13] = (unsigned char)cksum; // check sum LSB
	dest[14] = 0x04; // EOT(end of transmition)
	return 15; // return generated packet counter
}

int VerifyInvHEXPResponse(const unsigned char *src, unsigned char ID, unsigned char parameter)
{
	unsigned int cnt;	
	unsigned short value = HEXtoASCII2Byte(ID, 0);	
	// check ACK, ID, CMD
	if(!(src[0]==0x06 && src[1]==(unsigned char)(value>>8) && \
		src[2]==(unsigned char)value && src[3]==0x52)) return HEXP_ERROR_PACKET;
		
	switch(parameter){ //check parameter
	case HEXP_PARA_FAULT: // ASCII 0004
		if(!(src[4]==0x30 && src[5]==0x30 && src[6]==0x30 && src[7]==0x34)){
			return 	HEXP_ERROR_PARAMETER;
		}
		value = 0;
		for (cnt=1;cnt<24;cnt++) // ID ~ end of data
		{
			value += (unsigned char) src[cnt];
		}
		if(VerifyInvHEXPCksum(&src[24],value)!=1) return HEXP_ERROR_CKSUM;
		break;
	case HEXP_PARA_PV: // ASCII 0020
		if(!(src[4]==0x30 && src[5]==0x30 && src[6]==0x32 && src[7]==0x30)){
			return 	HEXP_ERROR_PARAMETER;
		}
		value = 0;
		for (cnt=1;cnt<16;cnt++) // ID ~ end of data
		{
			value += (unsigned char) src[cnt];
		}
		if(VerifyInvHEXPCksum(&src[16],value)!=1) return HEXP_ERROR_CKSUM;
		break;
	case HEXP_PARA_GRID: // ASCII 0050
		if(!(src[4]==0x30 && src[5]==0x30 && src[6]==0x35 && src[7]==0x30)){
			return 	HEXP_ERROR_PARAMETER;
		}
		value = 0;
		for (cnt=1;cnt<36;cnt++) // ID ~ end of data
		{
			value += (unsigned char) src[cnt];
		}
		if(VerifyInvHEXPCksum(&src[36],value)!=1) return HEXP_ERROR_CKSUM;
		break;
	case HEXP_PARA_POWER: // ASCII 0060
		if(!(src[4]==0x30 && src[5]==0x30 && src[6]==0x36 && src[7]==0x30)){
			return 	HEXP_ERROR_PARAMETER;
		}
		value = 0;
		for (cnt=1;cnt<40;cnt++) // ID ~ end of data
		{
			value += (unsigned char) src[cnt];
		}
		if(VerifyInvHEXPCksum(&src[40],value)!=1) return HEXP_ERROR_CKSUM;
		break;
	case HEXP_PARA_SYSTEM: // ASCII 01e0
		if(!(src[4]==0x30 && src[5]==0x31 && src[6]==0x65 && src[7]==0x30)){
			return 	HEXP_ERROR_PARAMETER;
		}
		value = 0;
		for (cnt=1;cnt<20;cnt++) // ID ~ end of data
		{
			value += (unsigned char) src[cnt];
		}
		if(VerifyInvHEXPCksum(&src[20],value)!=1) return HEXP_ERROR_CKSUM;
		break;
	case HEXP_PARA_ENVIRON: // ASCII 0070
		if(!(src[4]==0x30 && src[5]==0x30 && src[6]==0x37 && src[7]==0x34)){
			return 	HEXP_ERROR_PARAMETER;
		}
		value = 0;
		for (cnt=1;cnt<24;cnt++) // ID ~ end of data
		{
			value += (unsigned char) src[cnt];
		}
		if(VerifyInvHEXPCksum(&src[24],value)!=1) return HEXP_ERROR_CKSUM;
		break;
	default:
		return -1;
	}
	return 1;
}

int GetInvHEXPValue(unsigned long *dest, const unsigned char *src, char sort, char invType)
{
	unsigned short valueH;
	unsigned short valueL;
	unsigned char hex;
	switch(sort){
	case HEXP_VALUE_SOLARFAULT:
	// 16 bit flags(one phase type) if set to 1, occur fault.
	// bit3_solar UV limit fault bit2_solar UV fault bit1_solar OV limit fault bit0_solar OV fault
	// 16 bit flags(three phase type) if set to 0, occur fault.
	// bit11_utility line failure bit4_solar UV limit faule bit3_solar OV limit fault
		hex = ASCII2CHARtoHEX(src[8], src[9], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[10], src[11], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_INVFAULT:
	// 16 bit flags(one phase type) if set to 1, occur fault.
	// bit14_inverter status(0:stop 1:run) bit8_inverter MC fault bit7_inverter over temperature fault
	// bit5_inverter fuse open bit3_inverter line async fault bit1_inverter OC overtime fault
	// bit0_inverter over current fault
	// 16 bit flags(three phase type) if set to 0, occur fault.
	// bit11_inverter over current fault	
		hex = ASCII2CHARtoHEX(src[12], src[13], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[14], src[15], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_INVSTATE:
	// 16 bit flags(one phase type) if set to 1, occur fault.
	// bit5_line UF fault bit4_line OF fault bit3_line failure fault bit1_line UV fault
	// bit0_line OV fault
	// 16 bit flags(three phase type) if set to 0, occur fault.
	// bit7_inverter over temprature bit2_inverter fuse state bit0_inverter contactor state(MC)
		hex = ASCII2CHARtoHEX(src[16], src[17], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[18], src[19], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_INVOPERFAULT:
	// 16 bit flags(one phase type) if set to 1, occur fault.	
	// 16 bit flags(three phase type) if set to 0, occur fault.
	// bit10_line OF fault bit9_inverter OC overtime fault bit8_line UF fault bit7_line OV fault
	// bit6_line UV fault bit5_line phase sequency fault bit4_invetrer line async fault
	// bit3_inverter frequency fault bit2_inverter output voltage fault bit0_inverter MC fault
		hex = ASCII2CHARtoHEX(src[20], src[21], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[22], src[23], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_SOLARV: // V
		hex = ASCII2CHARtoHEX(src[8], src[9], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[10], src[11], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_SOLARI: // A_one decimal point
		hex = ASCII2CHARtoHEX(src[12], src[13], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[14], src[15], HEX);
		*dest |= hex;
		if(invType==HEXP_TYPE_THREE){ // convert value to one decimal point
			*dest *= 10;
		}
		break;
	case HEXP_VALUE_LINERSV: // V
		hex = ASCII2CHARtoHEX(src[8], src[9], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[10], src[11], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_LINESTV: // V
		hex = ASCII2CHARtoHEX(src[12], src[13], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[14], src[15], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_LINETRV: // V
		hex = ASCII2CHARtoHEX(src[16], src[17], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[18], src[19], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_LINERI: // A_one decimal point
		hex = ASCII2CHARtoHEX(src[20], src[21], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[22], src[23], HEX);
		*dest |= hex;
		if(invType==HEXP_TYPE_THREE){ // convert value to one decimal point
			*dest *= 10;
		}
		break;
	case HEXP_VALUE_LINESI: // A_one decimal point
		hex = ASCII2CHARtoHEX(src[24], src[25], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[26], src[27], HEX);
		*dest |= hex;
		if(invType==HEXP_TYPE_THREE){ // convert value to one decimal point
			*dest *= 10;
		}
		break;
	case HEXP_VALUE_LINETI: // A_one decimal point
		hex = ASCII2CHARtoHEX(src[28], src[29], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[30], src[31], HEX);
		*dest |= hex;
		if(invType==HEXP_TYPE_THREE){ // convert value to one decimal point
			*dest *= 10;
		}
		break;
	case HEXP_VALUE_LINEFR: // Hz_one decimal point
		hex = ASCII2CHARtoHEX(src[32], src[33], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[34], src[35], HEX);
		*dest |= hex;
		break;
	case HEXP_VALUE_SOLARP: // kW_one decimal point
		hex = ASCII2CHARtoHEX(src[8], src[9], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[10], src[11], HEX);
		*dest |= hex;
		if(invType==HEXP_TYPE_ONE) *dest /= 100; //convert value to one decimal point
		break;
	case HEXP_VALUE_TOTALP: // kWh
		hex = ASCII2CHARtoHEX(src[12], src[13], HEX);
		valueH = hex;
		valueH = (valueH<<8)&0xFF00;
		hex = ASCII2CHARtoHEX(src[14], src[15], HEX);
		valueH |= hex;
		hex = ASCII2CHARtoHEX(src[16], src[17], HEX);
		valueL = hex;
		valueL = (valueL<<8)&0xFF00;
		hex = ASCII2CHARtoHEX(src[18], src[19], HEX);
		valueL |= hex;
		if(invType==HEXP_TYPE_ONE){
			*dest = (unsigned long)valueH*10000+valueL;
			*dest /= 1000;
		}
		else if(invType==HEXP_TYPE_THREE || invType==HEXP_TYPE_H30XXS_ML){
			*dest = (unsigned long)valueL*65536+valueH;
		}
		else ;
		break;
	case HEXP_VALUE_LINEP: // kW_one decimal point
		hex = ASCII2CHARtoHEX(src[20], src[21], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[22], src[23], HEX);
		*dest |= hex;
		if(invType==HEXP_TYPE_ONE) *dest /= 100; //convert value to one decimal point
		break;
	case HEXP_VALUE_PEAK: // kW_one decimal point
		hex = ASCII2CHARtoHEX(src[24], src[25], HEX);
		*dest = hex;
		*dest = (*dest<<8)&0xFFFFFF00;
		hex = ASCII2CHARtoHEX(src[26], src[27], HEX);
		*dest |= hex;
		if(invType==HEXP_TYPE_ONE) *dest /= 100; //convert value to one decimal point
		break;
	case HEXP_VALUE_TODAYP: // kWh
		if(invType==HEXP_TYPE_ONE){
			hex = ASCII2CHARtoHEX(src[32], src[33], HEX);
			*dest = hex;
			*dest = (*dest<<8)&0xFFFFFF00;
			hex = ASCII2CHARtoHEX(src[34], src[35], HEX);
			*dest |= hex;
		}
		else if(invType==HEXP_TYPE_THREE || invType==HEXP_TYPE_H30XXS_ML){
			hex = ASCII2CHARtoHEX(src[28], src[29], HEX);
			*dest = hex;
			*dest = (*dest<<8)&0xFFFFFF00;
			hex = ASCII2CHARtoHEX(src[30], src[31], HEX);
			*dest |= hex;
		}
		else ;
		break;
	case HEXP_VALUE_PF: // %_one decimal point
		if(invType==HEXP_TYPE_ONE){
			hex = ASCII2CHARtoHEX(src[28], src[29], HEX);
			*dest = hex;
			*dest = (*dest<<8)&0xFFFFFF00;
			hex = ASCII2CHARtoHEX(src[30], src[31], HEX);
			*dest |= hex;
		}
		else if(invType==HEXP_TYPE_THREE || invType==HEXP_TYPE_H30XXS_ML){
			hex = ASCII2CHARtoHEX(src[36], src[37], HEX);
			*dest = hex;
			*dest = (*dest<<8)&0xFFFFFF00;
			hex = ASCII2CHARtoHEX(src[38], src[39], HEX);
			*dest |= hex;
		}
		else ;
		break;
	case HEXP_INFO_PHASE: // return ASCII value 1 or 3
		*dest = src[8];
		break;
	// after implement to string function
	/*case HEXP_INFO_POWER: // return ASCII three character
		ptr = (unsigned char*)dest;
		if(invType==HEXP_TYPE_ONE){
			ptr[0] = 0x30;
			ptr[1] = 0x30;
			ptr[2] = 0x30;
			ptr[3] = src[10];
		}
		else if(invType==HEXP_TYPE_THREE || invType==HEXP_TYPE_H30XXS_ML){
			ptr[0] = 0x30;
			ptr[1] = src[9];
			ptr[2] = src[10];
			ptr[3] = src[11];
		}
		else ;
		break;
	case HEXP_INFO_PTIME: // product time - return ASCII year(2byte), month(2byte)
		ptr = (unsigned char*)dest;
		ptr[0] = src[12];
		ptr[1] = src[13];
		ptr[2] = src[14];
		ptr[3] = src[15];
		break;
	case HEXP_INFO_SERIALN: // return ASCII four character
		ptr = (unsigned char*)dest;
		ptr[0] = src[16];
		ptr[1] = src[17];
		ptr[2] = src[18];
		ptr[3] = src[19];
		break;	
	case HEXP_VALUE_TILT:
	// have to convert received 400~2000 value to 0~2000W/m^2
		if(invType==HEXP_TYPE_THREE || invType==HEXP_TYPE_H30XXS_ML){
			hex = ASCII2CHARtoHEX(src[8], src[9], HEX);
			*dest = hex;
			*dest = (*dest<<8)&0xFFFFFF00;
			hex = ASCII2CHARtoHEX(src[10], src[11], HEX);
			*dest |= hex;
		}
		break;
	case HEXP_VALUE_HORIZON:
	// have to convert received 400~2000 value to 0~2000W/m^2
		if(invType==HEXP_TYPE_THREE || invType==HEXP_TYPE_H30XXS_ML){
			hex = ASCII2CHARtoHEX(src[12], src[13], HEX);
			*dest = hex;
			*dest = (*dest<<8)&0xFFFFFF00;
			hex = ASCII2CHARtoHEX(src[14], src[15], HEX);
			*dest |= hex;
		}
		break;
	case HEXP_VALUE_OUTDOORT:
	// have to convert received 400~2000 value to -20~80 celsius
		if(invType==HEXP_TYPE_THREE || invType==HEXP_TYPE_H30XXS_ML){
			hex = ASCII2CHARtoHEX(src[16], src[17], HEX);
			*dest = hex;
			*dest = (*dest<<8)&0xFFFFFF00;
			hex = ASCII2CHARtoHEX(src[18], src[19], HEX);
			*dest |= hex;
		}
		break;
	case HEXP_VALUE_MODULET:
	// have to convert received 400~2000 value to -20~80 celsius
		if(invType==HEXP_TYPE_THREE || invType==HEXP_TYPE_H30XXS_ML){
			hex = ASCII2CHARtoHEX(src[20], src[21], HEX);
			*dest = hex;
			*dest = (*dest<<8)&0xFFFFFF00;
			hex = ASCII2CHARtoHEX(src[22], src[23], HEX);
			*dest |= hex;
		}
		break;*/
	default:
		return 0;
	}
	return 1;
}
