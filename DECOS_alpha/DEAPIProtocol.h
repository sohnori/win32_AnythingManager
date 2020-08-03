#pragma once

#define DEAPI_DATAMAXLENGTH		300
#define DEAPI_STX				1
#define DEAPI_LENGTH			2
#define DEAPI_APIID				4
#define DEAPI_CMD				5
#define DEAPI_STATUS			6
#define DEAPI_FRAMEID			7
#define DEAPI_OPTIONH			8
#define DEAPI_OPTIONL			9
#define DEAPI_DATA				10
#define DEAPI_CKSUM				11

struct tagDEAPIPacketStr {
	TCHAR stx[3];
	TCHAR lengthmsb[3];
	TCHAR lengthlsb[3];
	TCHAR apiID[3];
	TCHAR cmd[3];
	TCHAR status[3];
	TCHAR frameID[3];
	TCHAR optionH[3];
	TCHAR optionL[3];
	TCHAR data[DEAPI_DATAMAXLENGTH];
	TCHAR ckSum[3];
};

struct tagDEAPIPacket {
	TCHAR stx;
	USHORT length;
	TCHAR apiID;
	TCHAR cmd;
	TCHAR status;
	TCHAR frameID;
	TCHAR optionH;
	TCHAR optionL;
	TCHAR data[DEAPI_DATAMAXLENGTH/2];
	TCHAR ckSum;
};

INT GetDEAPIPacketStr(tagDEAPIPacketStr *DAPdest, const TCHAR *src, INT cbSourceMAX);

INT GetDEAPIPacket(tagDEAPIPacket *DAPdest, const TCHAR *src, INT cbSourceMAX);

INT GetMSGVerifiedDEAPIPacket(TCHAR *destMSG, const tagDEAPIPacket *DAPsrc, INT optionflag, INT cbMSGMAX);

INT GetMSGVerifiedDEAPIPacket(TCHAR *destMSG, const tagDEAPIPacketStr *DAPsrc, INT optionflag, INT cbMSGMAX);