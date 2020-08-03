#ifndef __EQUIPMENT_REMS_H__
#define __EQUIPMENT_REMS_H__

// RS 485(hexASCII) default baudRate 9600
// 8bits data, 1stop bits, no parity, max 32 device connect
// max cable length 1200 meter, parallel connect, ID 1~99

// request packet - SOP(1) address(1) command(1) crcH(1) crcL(1)
// response packet - SOP(1) address(1) command(1) dataLengthHigh(1) dataLengthLow(1) crcH(1) crcL(1)

#define REMS_MAXSIZE_RX_PACKET		63
#define REMS_MAXSIZE_TX_PACKET		5
#define REMS_MAXSIZE_COMBINEDDATA	73

#define REMS_CMD_PV_ONE			0x01
#define REMS_CMD_PV_THREE		0x07
#define REMS_CMD_PVH_NATURE		0x01
#define REMS_CMD_PVH_SUPPLY		0x01
#define REMS_CMD_PVH_LOAD		0x03
#define REMS_CMD_GEOTHERMY_HPUMP	0x01 // heat pump
#define REMS_CMD_GEOTHERMY_H_C		0x03 // heat and cooling
#define REMS_CMD_GEOTHERMY_HWS		0x05 // hot water supply
#define REMS_CMD_WINDPOWER		0x01
#define REMS_CMD_FUELCELL		0x01
#define REMS_CMD_ESS			0x01

#define REMS_DATA_PV_ONE		0x01
#define REMS_DATA_PV_THREE		0x03
#define REMS_DATA_PVH			0x10
#define REMS_DATA_PVH_NATURE		0x11
#define REMS_DATA_PVH_SUPPLY		0x12
#define REMS_DATA_PVH_LOAD		0x13
#define REMS_DATA_GEOTHERMY		0x20
#define REMS_DATA_GEOTHERMY_HPUMP	0x21 // heat pump
#define REMS_DATA_GEOTHERMY_H_C		0x22 // heat and cooling
#define REMS_DATA_GEOTHERMY_HWS		0x23 // hot water supply
#define REMS_DATA_WINDPOWER		0x30
#define REMS_DATA_FUELCELL		0x40
#define REMS_DATA_ESS			0x50 // energy storage system

// error code for 485
#define REMS_ERROR_PACKET	(-2)
#define REMS_ERROR_ID		(-3)
#define REMS_ERROR_CRC		(-4)

// lora send packet form
// command(1)_0x14(fixed) energySort(1) type(1) multyCounter(1) errorCode(1) data(n)
// for to generate lora send packet

#define REMS_MAXSIZE_LORAPACKET		78

#define REMS_ENERGYTYPE_LORA_PV		0x01
#define REMS_ENERGYTYPE_LORA_PVH	0x02
#define REMS_ENERGYTYPE_LORA_GEOTHERMY	0x03
#define REMS_ENERGYTYPE_LORA_WINDPOWER	0x04
#define REMS_ENERGYTYPE_LORA_FUELCELL	0x06
#define REMS_ENERGYTYPE_LORA_ESS	0x07

#define REMS_ENERGYTYPE_LORA_PV_ONE		0x01
#define REMS_ENERGYTYPE_LORA_PV_THREE		0x02
#define REMS_ENERGYTYPE_LORA_PVH_FORCE		0x01
#define REMS_ENERGYTYPE_LORA_PVH_NATURE		0x02
#define REMS_ENERGYTYPE_LORA_GEOTHERMY_HP	0x01
#define REMS_ENERGYTYPE_LORA_GEOTHERMY_LOAD	0x02
#define REMS_ENERGYTYPE_LORA_ETC		0x01

#define REMS_ENERGYTYPE_485_PV1P		0x01
#define REMS_ENERGYTYPE_485_PV3P		0x02
#define REMS_ENERGYTYPE_485_PVHF		0x03
#define REMS_ENERGYTYPE_485_PVHN		0x04
#define REMS_ENERGYTYPE_485_GEOT		0x05
#define REMS_ENERGYTYPE_485_GEOH		0x06
#define REMS_ENERGYTYPE_485_GEOL		0x07
#define REMS_ENERGYTYPE_485_WIND		0x08
#define REMS_ENERGYTYPE_485_FUEL		0x09
#define REMS_ENERGYTYPE_485_ESSS		0x0A

//error code for lora
#define REMS_ERROR_SERIALFAIL		0x38
#define REMS_ERROR_NORESPONSE		0x39

struct tagREMSLoraH{ // REMS lora packet header
	unsigned char type;
	unsigned char subType;
	unsigned char multyCounter;
	unsigned char errorCode;
};

struct tagREMSPV{ // PV(one or three) data
	unsigned short pvv; // V
	unsigned short pvi; // A
	unsigned short pvp; // W
	unsigned short outrv; // V
	unsigned short outsv; // V
	unsigned short outtv; // V
	unsigned short outri; // A
	unsigned short outsi; // A
	unsigned short outti; // A
	unsigned short outp; // W
	unsigned short pf; // %_one decimal point
	unsigned short fr; // Hz_one decimal point
	unsigned long totalph;
	unsigned long totalpl; // Wh
	unsigned short status;
	// 16 bit flags - if set to 1, occur fault
	// bit12_earth fault, bit11_power failure bit10_grid UF bit9_grid OV bit8_grid OC bit7_grid UV
	// bit6_grid OV bit5_inverter overheat bit4_inverter IGBT error bit3_PV OC bit2_PV UV
	// bit1_PV OV bit0_inverter run:0 or stop:1
}; // 26 or 36 bytes

struct tagREMSPVHSupply{// PV heat supply data
	// temrperature - most 4 bit is negative identifier. 0b0000_positive 0b1111_negative
	unsigned short hcintemp; // celsius_heat concentrator input temperature, one decimal point
	unsigned short hcouttemp; // celsius_heat concentrator output temperature, one decimal point
	unsigned short hsuppertemp; // celsius_heat storage upper temperature, one decimal point
	unsigned short hslowertemp; // celsius_heat storage lower temperature, one decimal point
	unsigned long discharge; // LPM_discharge value per minute, one decimal point
	unsigned long totalGenH; 
	unsigned long totalGenL; // kcal_supply total value, two decimal point
	unsigned short status;
	// 16 bit flags - if set to 1, occur fault
	// bit2_overheat(220 celsius) error bit1_underheat error(-20 celsius)
	// bit0_heat medium circulation error
}; // 22 bytes

struct tagREMSPVHLoad{// PV heat load data
	// temrperature - most 4 bit is negative identifier. 0b0000_positive 0b1111_negative
	unsigned short wstemp; // water supply temperature, one decimal point
	unsigned short hwstemp; // hot water supply temperature, one decimal point
	unsigned long discharge; // LPM_discharge value per minute, one decimal point
	unsigned long totalSpendH; 
	unsigned long totalSpendL; // kcal_total spend calorie, two decimal point
}; // 16 bytes

struct tagREMSPVH{// PV heat supply and load data
	struct tagREMSPVHSupply PVHSupply;
	struct tagREMSPVHLoad PVHLoad;
}; // 38 bytes

struct tagREMSPVHNature{// PV heat nature type load data
	// temrperature - most 4 bit is negative identifier. 0b0000_positive 0b1111_negative
	unsigned short wstemp; // celsius_water supply temperature, one decimal point
	unsigned short hwstemp; // celsius_hot water supply temperature, one decimal point
	unsigned long discharge; // LPM_discharge value per minute, one decimal point
	unsigned long totalSpendH; 
	unsigned long totalSpendL; // kcal_total spend calorie, two decimal point
	unsigned short status;
	// 16 bit flags - if set to 1, occur fault
	// bit0_run:0 stop:1
}; // 16 bytes

struct tagREMSGeothermyHP{ // geothermy heat pump data
	// temrperature - most 4 bit is negative identifier. 0b0000_positive 0b1111_negative
	unsigned short voltage; // V
	unsigned short current; // A
	unsigned short power;// W
	unsigned long heatGen;// W_heat generation value
	unsigned long totalGenH; 
	unsigned long totalGenL; // kWh_total generation value, one decimal point
	unsigned long totalSpendPH;
	unsigned long totalSpendPL; //kWh_total spend power, one decimal point
	unsigned char state; // 0:stop 1:cooling 2:heating
	unsigned short wintemp; // celsius_geothermy water input temperature, one decimal point
	unsigned short wouttemp; // celsius_geothermy water output temperature, one decimal point
	unsigned short hcintemp; // celsius_heating and cooling input temperature, one decimal point
	unsigned short hcouttemp; // celsius_heating and coolong output temperature, one decimal point
	unsigned long hcwdischargeH;
	unsigned long hcwdischargeL; // LPM_heat and cool water discharge, one decimal point
	unsigned short status;
	// 16 bit flags - if set to 1, occur fault
	// bit0_heat pump run:0 stop:1 
}; // 41 byte

struct tagREMSGeothermyHC{ // geothermy heating and cooling data
	// temrperature - most 4 bit is negative identifier. 0b0000_positive 0b1111_negative
	unsigned short intemp; // celsius_heating and cooling input temperature, one decimal point
	unsigned short outtemp; // celsius_heating and cooling output temperature, one decimal point
	unsigned long Spenddischarge; // LPM_heating and cooling spend discharge, one decimal point
	unsigned long SpendcalorieH;
	unsigned long SpendcalorieL; // kWh_total spend calorie, one decimal point
}; //16 bytes

struct tagREMSGeothermyHWS{ // geothermy hot water supply
	// temrperature - most 4 bit is negative identifier. 0b0000_positive 0b1111_negative
	unsigned short wstemp; // celsius_water supply temperature, one decimal point 
	unsigned short hwstemp; // celsius_hot water supply temperature, one decimal point
	unsigned long spenddischarge; // LPM_spend discharge, one decimal point
	unsigned long spendcalorieH;
	unsigned long spendcalorieL; // kWh_total spend calorie, one decimal point
}; // 16 byte

struct tagREMSGeothermy{ // geothermy hot water supply
	struct tagREMSGeothermyHP GeothermyHP;
	struct tagREMSGeothermyHC GeothermyHC;
	struct tagREMSGeothermyHWS GeothermyHWS;
}; // 73 bytes

struct tagREMSWindpower{ // wind power generation
	unsigned short beforeV; // V
	unsigned short beforeC; // A
	unsigned short beforeP; // W
	unsigned short afterV; // V
	unsigned short afterC; // A
	unsigned short afterP; // W
	unsigned short frequency; // Hz, one decimal point
	unsigned long totalPH; 
	unsigned long totalPL; // Wh total generation
	unsigned short status;
	// 16 bit flags - if set to 1, occur fault
	// bit0_run:0 stop:1 
}; // 24 bytes

struct tagREMSFuelcell{ // fuel cell
	// temrperature - most 4 bit is negative identifier. 0b0000_positive 0b1111_negative
	unsigned short beforeV; // V
	unsigned short beforeC; // A
	unsigned short beforeP; // W
	unsigned short afterV; // V
	unsigned short afterC; // A
	unsigned short afterP; // W
	unsigned short hgen; // W_heat generation
	unsigned long hgentotalH; 
	unsigned long hgentotalL; // kWh_heat generation total, one decimal point
	unsigned long hspendtotalH;
	unsigned long pdhspendtotalL; // kWh_previous day heat spend total, one decimal point
	unsigned long pdhspendtotalH;
	unsigned long hspendtotalL; // kWh_previous day heat spend total, one decimal point
	unsigned short wintemp; // celsius_water input temperature, one decimal point
	unsigned short wouttemp; // celsius_water output temperature, one decimal point
	unsigned short PF; // power factor, one decimal point
	unsigned short FR; // frequency, one decimal point
	unsigned long totalgenH;
	unsigned long totalgenL; // Wh_total generation
	unsigned short status;
	// 16 bit flags - if set to 1, occur fault
	// bit0_run:0 stop:1 
}; // 56 bytes

struct tagREMSESS{ // energe storage system
	unsigned short beforeV; // V
	unsigned short beforeC; // A
	unsigned short beforeP; // W
	unsigned short afterV; // V
	unsigned short afterC; // A
	unsigned short afterP; // W
	unsigned char bstate; // battery state - 0:stop 1:charging
	unsigned char bSOC; // battery states on charge
	unsigned char bSOH; // // battery states on health
	unsigned short btemplower; // celsius_battery lower temperature, one decimal point 
	unsigned short btemphigher; // celsius_battery higher temperature, one decimal point 
	unsigned short btempaver; // celsius_battery average temperature, one decimal point
	unsigned short batteryV; // V_battery voltage
	unsigned short batteryC; // A_battery current
	unsigned short status;
	// 16 bit flags - if set to 1, occur fault
	// bit0_run:0 stop:1 
}; // 27 bytes

int GetREMSEquipTypeDefined(const unsigned char *src);

int GetREMSEquipRequestP(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char equipType, unsigned char typeSub);

int VerifyREMSEquipResponseP(const unsigned char *src,  unsigned char ID, unsigned char equipType, unsigned char typeSub);

int GetREMSEquipRequest(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char cmd);

int VerifyREMSEquipResponse(const unsigned char *src,  unsigned char ID, unsigned char cmd);

int GetREMSEquipData(unsigned char *dest, unsigned int szDest, const unsigned char *src);

int GetREMSEquipCombinedData
(unsigned char *dest, unsigned int szDest, const unsigned char *src1, const unsigned char *src2, unsigned char sort);

int GenREMSLoraPacket(unsigned char *dest, unsigned int szDest, const unsigned char *src, unsigned int szSrc, struct tagREMSLoraH header);

unsigned char GetREMSLoraType(unsigned char equipType);

unsigned char GetREMSLoraSubType(unsigned char equipType);

#endif