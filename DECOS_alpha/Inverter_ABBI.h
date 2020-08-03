#ifndef __INVERTER_ABBI_H__
#define __INVERTER_ABBI_H__

// RS 485(hex) default baudRate 19200
// 8bits data, 1stop bits, no parity, max 32 device connect
// max cable length 1200 meter, parallel connect
// Aurora inverter ID 0~63, Aurora Wind Box 100~107, broadcast Commands 250~255

/*
The typical TX message has the following format:
Byte 0   Byte 1  Byte 2 Byte 3 Byte 4 Byte 5 Byte 6 Byte 7 Byte 8 	Byte 9
Address  Command Data 0 Data 1 Data 2 Data 3 Data 4 Data 5 CRC Low Byte CRC High Byte

Byte 0               Byte 1       Byte 2 Byte 3 Byte 4 Byte 5  Byte 6       Byte 7
Transmission State   Unit State   Data 0 Data 1 Data 2 Data 3  CRC Low Byte CRC High Byte

Transmission state  This field describes the result of the message processing
performed by the addressed unit:
- 0  All OK
- 51  This command code is not implemented.
- 52  This parameter code does not exist.
- 53  Parameter value is out of range.
- 54  Device memory is not accessible (for memory read/write commands).
- 55  Unit is not in service-mode (for commands which require service-mode).
- 56  The internal microcontroller is not accessible.
- 57  The command was not executed.
- 58  The required value is not available (retry).
- 60  The required value is not available yet (wait for startup process to end and retry).
- 61  Cannot enter service mode (external password for service mode was not set).
*/

/* cmd_50 RX message format
Byte 0             Byte 1       Byte 2         Byte 3               Byte 4               Byte 5      Byte 6 Byte 7
Transmission_State Global_State Inverter_State DC/DC_Channel1_State DC/DC_Channel2_State Alarm_State CRC_L  CRC_H

Global State 		DcDc State 		Inverter State
0 Sending Parameters 	0 DcDc OFF 		0 Stand By
1 Wait Sun/Grid 	1 Ramp Start 		1 Checking Grid
2 Checking Grid 	2 MPPT 			2 Run
3 Measuring Riso 	3 Not Used 		3 Bulk OV
4 DcDc Start 		4 Input OC 		4 Out OC
5 Inverter Start 	5 Input UV 		5 IGBT Sat
6 Run 			6 Input OV 		6 Bulk UV
7 Recovery 		7 Input Low 		7 Degauss Error
8 Pause 		8 No Parameters 	8 No Parameters
9 Ground Fault 		9 Bulk OV 		9 Bulk Low
10 OTH Fault 		10 Communication Error 	10 Grid OV
11 Address Setting 	11 Ramp Fail 		11 Communication Error
12 Self Test 		12 Internal Error 	12 Degaussing
13 Self Test Fail 	13 Input mode Error 	13 Starting
14 Sensor Test+Mea.Riso 14 Ground Fault 	14 Bulk Cap Fail
15 Leak Fault 		15 Inverter Fail 	15 Leak Fail
16 Waiting for manual reset 16 DcDc IGBT Sat 	16 DcDc Fail
17 Internal Error E026 	17 DcDc ILEAK Fail 	17 Ileak Sensor Fail
18 Internal Error E027 	18 DcDc Grid Fail 	18 SelfTest: relay inverter
19 Internal Error E028 	19 DcDc Comm. Error 	19 SelfTest: wait for sensor test
20 Internal Error E029 	¢®| ¢®| 			20 SelfTest: test relay DcDc+sensor
21 Internal Error E030 	255 DcDc Dsp not programmed21 SelfTest: relay inverter fail
22 Sending Wind Table 				22 SelfTest timeout fail
23 Failed Sending table 			23 SelfTest: relay DcDc fail
24 UTH Fault 					24 Self Test 1
25 Remote OFF 					25 Waiting self test start
26 Interlock Fail 				26 Dc Injection
27 Executing Autotest 				27 Self Test 2
30 Waiting Sun 					28 Self Test 3
31 Temperature Fault 				29 Self Test 4
32 Fan Staucked 				30 Internal Error
33 Int. Com. Fault 				31 Internal Error
34 Slave Insertion 				40 Forbidden State
35 DC Switch Open 				41 Input UC
36 TRAS Switch Open 				42 Zero Power
37 MASTER Exclusion 				43 Grid Not Present
38 Auto Exclusion 				44 Waiting Start
50 Wait DC input 				45 MPPT
98 Erasing Internal EEprom 			46 Grid Fail
99 Erasing External EEprom 			47 Input OC
100 Counting EEprom 				¢®| ¢®|
101 Freeze 					255 Inverter Dsp not programmed
110 Forbidden partner board was found
111 DC string self-test fault
112 Service mode
113 ¢®¡ÆSafety¢®¡¾ memory area error
114 Too many leak fault events for the day
115 Arc fault
120 Wrong model code
150 DSP communication error
200 Dsp Programming

Alarm State
Value Description Code Value Description Code Value Description Code
0 No Alarm 		41 Error Meas F E028 	82 ¢®¡ÆAF self test fail¢®¡¾ E053
1 Sun Low W001 		42 Error Meas Z E029
2 Input OC E001 	43 Error Meas Ileak E030
3 Input UV W002 	44 Error Read V E031
4 Input OV E002 	45 Error Read I E032
5 Sun Low W001 		46 Table fail W009
6 No Parameters E003 	47 Fan Fail W010
7 Bulk OV E004 		48 UTH E033
8 Comm.Error E005 	49 Interlock fail E034
9 Output OC E006 	50 Remote Off E035
10 IGBT Sat E007 	51 Vout Avg errror E036
11 Bulk UV W011 	52 Battery low W012
12 Internal error E009 	53 Clk fail W013
13 Grid Fail W003 	54 Input UC E037
14 Bulk Low E010 	55 Zero Power W014
15 Ramp Fail E011 	56 Fan Stuck E038
16 Dc/Dc Fail 1 E012 	57 DC Switch Open E039
17 Wrong Mode E013 	58 Tras Switch Open E040
18 Ground Fault --- 	59 AC Switch Open E041
19 Over Temp. E014 	60 Bulk UV E042
20 Bulk Cap Fail E015 	61 Autoexclusion E043
21 Inverter Fail E016 	62 Grid df/dt W015
22 Start Timeout E017 	63 Den switch Open W016
23 Ground Fault E018 	64 Jbox fail W017
24 AC feed forward E049 65 DC Door Open E044
25 Ileak sens.fail E019 66 AC Door Open E045
26 DcDc Fail 2 E012 	67 Anti islanding E047
27 Self Test Error 1 E020 68 Fuse DC Fail W020
28 Self Test Error 2 E021 69 Liquid Cooler Fail E048
29 Self Test Error 3 E019 70 SPD AC protection open W018
30 Self Test Error 4 E022 71 SPD DC protection open W019
31 DC inj error E023 	72 String selftest fail E046
32 Grid OV W004 	73 Power reduction start W021
33 Grid UV W005 	74 Power reduction end W025
34 Grid OF W006 	75 React. power mode changed W022
35 Grid UF W007 	76 date/time changed W023
36 Z grid Hi W008 	77 Energy data reset W024
37 Internal error E024 	78 RESERVED NOT U.
38 Riso Low E025 	79 Arc fault E050
39 Vref Error E026 	80 Bad ¢®¡Æsafety¢®¡¾ memory area E051
40 Error Meas V E027 	81 Module Door Open E052
*/

// request option
#define ABBI_TYPE_GRIDV		1
#define ABBI_TYPE_GRIDI		2
#define ABBI_TYPE_GRIDP		3
#define ABBI_TYPE_GRIDFR	4
#define ABBI_TYPE_PVV1		23
#define ABBI_TYPE_PVI1		25
#define ABBI_TYPE_PVV2		26
#define ABBI_TYPE_PVI2		27
#define ABBI_TYPE_PVP1		8
#define ABBI_TYPE_PVP2		9
#define ABBI_TYPE_GRIDRV	61
#define ABBI_TYPE_GRIDSV	62
#define ABBI_TYPE_GRIDTV	63
#define ABBI_TYPE_GRIDRI	39
#define ABBI_TYPE_GRIDSI	40
#define ABBI_TYPE_GRIDTI	41
//request option
#define ABBI_PARTIAL_ENERGYD	0
#define ABBI_PARTIAL_ENERGYW	1
#define ABBI_PARTIAL_ENERGYM	3
#define ABBI_PARTIAL_ENERGYY	4
#define ABBI_PARTIAL_ENERGYT	5
#define ABBI_PARTIAL_ENERGYP	6

#define ABBI_VALUE_GRIDV	1
#define ABBI_VALUE_GRIDI	2
#define ABBI_VALUE_GRIDP	3
#define ABBI_VALUE_GRIDFR	4
#define ABBI_VALUE_PVV1		5
#define ABBI_VALUE_PVI1		6
#define ABBI_VALUE_PVV2		7
#define ABBI_VALUE_PVI2		8
#define ABBI_VALUE_PVP1		9
#define ABBI_VALUE_PVP2		10
#define ABBI_VALUE_GRIDRV	11
#define ABBI_VALUE_GRIDSV	12
#define ABBI_VALUE_GRIDTV	13
#define ABBI_VALUE_GRIDRI	14
#define ABBI_VALUE_GRIDSI	15
#define ABBI_VALUE_GRIDTI	16
#define ABBI_VALUE_ENERGYD	30
#define ABBI_VALUE_ENERGYW	31
#define ABBI_VALUE_ENERGYM	32
#define ABBI_VALUE_ENERGYY	33
#define ABBI_VALUE_ENERGYT	34 // energy total
#define ABBI_VALUE_ENERGYP	35 // energy partial
#define ABBI_VALUE_STATE	36
#define ABBI_VALUE_PN		37
#define ABBI_VALUE_VERSION	38
#define ABBI_VALUE_SERIALN	39
#define ABBI_VALUE_PDAY		40
#define ABBI_STATE_GLOBAL	41
#define ABBI_STATE_INVERTER	42
#define ABBI_STATE_DCDCCH1	43
#define ABBI_STATE_DCDCCH2	44
#define ABBI_STATE_ALARM	45

#define ABBI_CMD_STATE		0x32
#define ABBI_CMD_PN		0x34 // product number(ASCII 6 character)
#define ABBI_CMD_VERSION	0x3A // ASCII
#define ABBI_CMD_DSP		0x3B // ANSI standard float value
#define ABBI_CMD_SERIALN	0x3F // serial number(ASCII 6 character)
#define ABBI_CMD_PDAY		0x41 // production week/year
// cmd 0x4E, sub_cmd 0_daily energy, 1_weekly energy, 2_not used, 3_monthly energy, 
//                   4_yearly energy, 5_total energy 6_ patial energy(since last reset)
#define ABBI_CMD_ENERGY		0x4E

// error code
#define ABBI_ERROR_PACKET	(-2)
#define ABBI_ERROR_CRC16_X25	(-5)

int GetInvABBIRequest
(unsigned char *dest, unsigned int szDest, unsigned char ID, unsigned char cmdCode, unsigned char subCode);

int VerifyInvABBIResponse(const unsigned char *src);

int GetInvABBIValue(unsigned long *dest, const unsigned char *src, char sort);

int GetInvABBIValueStr(unsigned char *dest, unsigned char szDest, const unsigned char *src, char sort);

#endif