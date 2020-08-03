#pragma once
#include <Windows.h>

#define READ_COORDI		1
#define WRITE_COORDI	2

#define CMD_ENTER		1
#define CMD_EXIT		2
#define CMD_C1ES		3
#define CMD_C1CD		4
#define CMD_MDXY		5
#define CMD_INVX		6
#define CMD_CH0			7
#define CMD_CH1			8
#define CMD_ZBMD		9
#define CMD_INMD		10
#define CMD_INEH		11
#define CMD_INEL		12
#define CMD_PVEM		13
#define CMD_LOMM		14
#define CMD_DBUG		15
#define CMD_BVER		16
#define CMD_INFO		17
#define CMD_EEPR		18
#define CMD_EEPI		19
#define CMD_TIME		20
#define CMD_IDSC		21
#define CMD_TXPK		22
#define CMD_CH0S		23
#define CMD_CH0R		24
#define CMD_CH1S		25
#define CMD_CH1R		26

#define CMD_TIMEOUT		30

#define CMD_ZB_IS		50

struct tagFlagCoordi {	
	TCHAR enterCmdMode;
	TCHAR exitCmdMode;
	TCHAR C1ES;
	TCHAR C1CD;
	TCHAR MDXY;
	TCHAR INVX;		
	TCHAR ZBMD;
	TCHAR INMD;
	TCHAR INEH;
	TCHAR INEL;
	TCHAR PVEM;
	TCHAR LOMM;
	TCHAR DBUG;
	TCHAR BVER;
	TCHAR INFO;
	TCHAR EEPR;
	TCHAR EEPI;
	TCHAR TIME;
	TCHAR IDSC;
	TCHAR TXPK;
	TCHAR CH0S;
	TCHAR CH0R;
	TCHAR CH1S;
	TCHAR CH1R;
	TCHAR cmdZBis;
};

struct tagInvData {
	TCHAR type[5];
	TCHAR id[4];
	TCHAR phase[2];
	TCHAR power[4];
};

struct tagDeviceMode {
	TCHAR INMD[3];
	TCHAR ZBMD[3];
	TCHAR PVEM[3];
	TCHAR LOMM[3];
};

struct tagMSInfo {
	TCHAR moduleDataStr[33];
	INT flagModuleNum;
	INT moduleCount;
	BOOL flagSingle;
};

class CoordinatorReal
{
private:
	struct tagFlagCoordi FlagCoordi;
	struct tagInvData InvData[16];
	struct tagDeviceMode DeviceMode;
	struct tagMSInfo MSInfo;	
	TCHAR flagInvNum;	
	TCHAR flagMode;
	TCHAR *writeData;
	TCHAR sizeWriteData;
	TCHAR *portName;
	TCHAR sizePortName;
public:
	CoordinatorReal();
	BOOL SetMSIFlagModuleNum(INT num);
	BOOL SetMSIModuleCount(INT num);
	BOOL SetPortName(TCHAR *src);
	BOOL SetMode(TCHAR mode);
	BOOL SetFlag(TCHAR flag, UCHAR option);	
	BOOL SetInvData(const TCHAR *src, INT InvNum);
	BOOL SetDeviceModeData(const TCHAR *src, INT option);
	BOOL SetWriteData(const TCHAR *src);
	BOOL GetMSIFlagSingle(void);
	UCHAR GetMSIFlagModuleNum(void);
	INT GetMSIModuleDataStr(TCHAR *dest);
	TCHAR GetFlagInvNum(void);
	BOOL GetPortName(TCHAR *dest);
	TCHAR GetMode();
	BOOL GetInvData(TCHAR *dest, INT invNum);
	BOOL GetWriteData(TCHAR *dest);
	INT ComparePortName(const TCHAR *src);
	INT ProcessING();
	INT GetTXCmd();
	BOOL VerifyOK(const TCHAR *src);
	//BOOL ProcessFlag();
	void ResetFlag();
	~CoordinatorReal();
};

INT GetInvTypeIndex(const TCHAR *src);