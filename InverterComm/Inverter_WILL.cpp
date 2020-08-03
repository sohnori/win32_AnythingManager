#include "Inverter_WILL.h"

typedef unsigned char u8int;

#define BIN	2
#define DEC	10
#define HEX	16

#define UPPERCASE	85
#define LOWERCASE	76

#define ADD_AHEAD_ZERO	1

/***************************
*      Prevate Function     *
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
		else if (nibbleL>=0x20 && nibbleL<0x30) return nibbleL -= 12;
		else if (nibbleL>=0x30 && nibbleL<0x40) return nibbleL -= 18;
		else if (nibbleL>=0x40 && nibbleL<0x50) return nibbleL -= 24;
		else if (nibbleL>=0x50 && nibbleL<0x60) return nibbleL -= 30;
		else if (nibbleL>=0x60 && nibbleL<0x70) return nibbleL -= 36;
		else if (nibbleL>=0x70 && nibbleL<0x80) return nibbleL -= 42;
		else if (nibbleL>=0x80 && nibbleL<0x90) return nibbleL -= 48;
		else if (nibbleL>=0x90) return nibbleL -= 54;	
	}	
	return nibbleL;	
}

/***************************
*      Public Function     *
****************************/

int GetInvWILLRequest
(u8int *dest, unsigned int szDest, u8int ID, u8int *cmd, u8int *addr, u8int length)
{	
	unsigned int cnt;
	unsigned short bcc = 0;
	if(szDest<20) return -1;
	
	dest[0] = 0x05; // ENQ
	dest[1] = (unsigned char)(ID>>8)+0x30; //convert HEX high to ASCII
	dest[2] = (ID&0x0F)+0x30; //convert HEX low to ASCII
	dest[3] = cmd[0]; // 'r' or 'w'
	dest[4] = cmd[1]; // 'S'
	dest[5] = cmd[2]; // 'B'
	dest[6] = 0x30; // variable length ASCII_'0'
	dest[7] = 0x37; // variable length ASCII_'7' point address value, maybe fixed
	dest[8] = addr[0]; // '%'
	dest[9] = addr[1]; // 'M'
	dest[10] = addr[2]; // 'W'
	dest[11] = addr[3]; // address MSB
	dest[12] = addr[4];
	dest[13] = addr[5];
	dest[14] = addr[6]; // address LSB
	dest[15] = (unsigned char)(length>>8)+0x30; //convert HEX high to ASCII
	dest[16] = (length&0x0F)+0x30; //convert HEX low to ASCII
	dest[17] = 0x04; // EOT
	for(cnt=0;cnt<18;cnt++){
		bcc += dest[cnt];	
	}
	bcc = HEXtoASCII2Byte((unsigned char) bcc, UPPERCASE); // convert bcc low byte to ASCII
	dest[18] = (unsigned char) (bcc>>8);
	dest[19] = (unsigned char) bcc;
	return  20;
}

int VerifyInvWILLResponse(const unsigned char *src,  unsigned char ID)
{
	unsigned int cnt;
	unsigned short bcc = 0;
	unsigned char length = ASCII2CHARtoHEX(src[8], src[9], HEX);
	if(!(src[0]==0x06 && src[10+length*2]==0x03)) return WILL_ERROR_PACKET;
	if(!(src[1]==((unsigned char)(ID>>8)+0x30) && src[2]==((unsigned char)ID+0x30))) return WILL_ERROR_ID;
	for(cnt=0;cnt<(11+length*2);cnt++){
		bcc += src[cnt];
	}
	bcc = HEXtoASCII2Byte((unsigned char) bcc, UPPERCASE);
	if(!(src[cnt]==(unsigned char)(bcc>>8) && src[cnt+1]==(unsigned char)bcc)) return WILL_ERROR_BCC;
	return 1;
}

int GetInvWILLValue(unsigned long *dest, const unsigned char *src, char sort)
{ // have to specify the data start point - response packet data start point is from packet[10]
	unsigned short valueH;
	unsigned short valueL;
	unsigned char valueByte;
	switch(sort){
	case WILL_VALUE_PVST:
	// 16 bit flag if set to 1, occur fault.
	// one phase - bit15_PV status bit2_PV UV bit1_PV OV trip bit0_PV OV
	// M series - bit15_PV status bit2_PV UV bit0_PV OV
	// SL or T series - bit15_PV status bit14_emergency stop bit13_insulation fault bit12_CB open
	//	bit11_SPD trouble bit2_PV UV bit1_PV OV trip bit0_PV OV		
	case WILL_VALUE_INVST:
	// 16 bit flag if set to 1, occur fault.
	// one phase type - bit15_inverter status bit11_voltage error bit8_MC fault bit7_ overheat
	//	 bit6_sychronous fault bit5_DC earth fault bit3_over current bit2_over current trip
	// SL or T series - bit15_inverter status bit11_voltage fault bit10_PWM fault bit9_IPM fault
	//	bit8_MC fault bit7_overheat bit6_synchronous fault bit5_FR fault bit4_overload
	//	bit3_OC bit2_OC trip bit1_under voltage bit0_over voltage		
	case WILL_VALUE_GRIDST:
	// 16 bit flag if set to 1, occur fault.
	// one phase type - bit15_grid status bit12_CB open bit5_grounding bit4_power failure
	// 	bit3_under frequency bit2_overfrequency bit1_under voltage bit0_over voltage
	// M series - bit15_grid status bit5_grounding bit4_power failure bit3_UF bit2_OF
	//	bit1_under voltage bit0_over voltage
	// SL or T series - bit15_grid status bit12_CB open bit5_grounding bit4_power failure
	//	bit3_under FR bit2_over FR bit1_UV bit0_OV		
	case WILL_VALUE_CONVST:
	// 16 bit flag if set to 1, occur fault.
	// one phase type - bit15_converter status bit5_fuse open bit4_over current trip
	// 	bit3_under voltage trip bit2_under voltage bit1_over voltage trip bit0_over voltage
		valueByte = ASCII2CHARtoHEX(src[0], src[1], HEX);
		valueL = valueByte;
		valueL = (valueL<<8)&0xFF00;
		valueByte = ASCII2CHARtoHEX(src[2], src[3], HEX);
		valueL |= (unsigned short)valueByte;
		*dest = valueL;
		break;
	case WILL_VALUE_PVV: // V		
	case WILL_VALUE_PVI: // A_one decimal point		
	case WILL_VALUE_PVP:
	// one phase - W
	// three phase - kW_one decimal point		
	case WILL_VALUE_RV: // V		
	case WILL_VALUE_SV: // V		
	case WILL_VALUE_TV: // V		
	case WILL_VALUE_RI: // A_one decimal point		
	case WILL_VALUE_SI: // A_one decimal point		
	case WILL_VALUE_TI: // A_one decimal point		
	case WILL_VALUE_FR: // Hz_one decimal point		
	case WILL_VALUE_PF: // %_one decimal point		
	case WILL_VALUE_GRIDP:
	// one phase - W
	// three phase - kW_one decimal point
		valueByte = ASCII2CHARtoHEX(src[0], src[1], HEX);
		valueL = valueByte;
		valueL = (valueL<<8)&0xFF00;
		valueByte = ASCII2CHARtoHEX(src[2], src[3], HEX);
		valueL |= (unsigned short)valueByte;
		*dest = valueL;
		break;
	case WILL_VALUE_TOTALP:
	// one phase - Wh
	// three phase - Wh		
	case WILL_VALUE_TODAYP:
	// one phase - Wh
	// three phase - Wh
		valueByte = ASCII2CHARtoHEX(src[0], src[1], HEX);
		valueH = valueByte;
		valueH = (valueH<<8)&0xFF00;
		valueByte = ASCII2CHARtoHEX(src[2], src[3], HEX);
		valueH |= (unsigned short)valueByte;
		valueByte = ASCII2CHARtoHEX(src[4], src[5], HEX);
		valueL = valueByte;
		valueL = (valueL<<8)&0xFF00;
		valueByte = ASCII2CHARtoHEX(src[6], src[7], HEX);
		valueL |= (unsigned short)valueByte;
		*dest = valueH;
		*dest = (unsigned long)(valueH<<16)&0xFFFF0000;
		*dest |= (unsigned long)valueL;
		break;
	default:
		return 0;
	}
	return 1;
}
