#include "CoordinatorReal.h"

CoordinatorReal::CoordinatorReal()
{	
	this->MSInfo.moduleDataStr[0] = NULL;
	this->FlagCoordi = { 0, };
	this->flagInvNum = 0;
	this->flagMode = 0;
	this->MSInfo.moduleCount = 0;
	this->MSInfo.flagModuleNum = 0;
	this->sizeWriteData = 34;
	this->sizePortName = 15;	
	this->writeData = (TCHAR*)malloc(this->sizeWriteData);
	this->portName = (TCHAR*)malloc(this->sizePortName);
	this->writeData[0] = NULL;
	this->portName[0] = NULL;
}

BOOL CoordinatorReal::SetMSIFlagModuleNum(INT num)
{
	if (num > 255 || num <= 0) return FALSE;
	this->MSInfo.flagModuleNum = num;
	return TRUE;

}
BOOL CoordinatorReal::SetMSIModuleCount(INT num)
{
	if (num > 255 || num <= 0) return FALSE;
	this->MSInfo.moduleCount = num;
	return TRUE;
}

BOOL CoordinatorReal::SetPortName(TCHAR *src)
{
	INT cnt;
	INT size = this->sizePortName;
	for (cnt = 0; *(src + cnt) != NULL; cnt++) {
		if (cnt > size) return FALSE;
		this->portName[cnt] = *(src + cnt);
	}
	this->portName[cnt] = NULL;
	return TRUE;
}

BOOL CoordinatorReal::SetMode(TCHAR mode)
{
	this->flagMode = mode;
	return TRUE;
}

BOOL CoordinatorReal::SetFlag(TCHAR flag, UCHAR option)
{	
	if (flag == CMD_ENTER) { this->FlagCoordi.enterCmdMode = 2; return TRUE; }
	else if (flag == CMD_EXIT) { this->FlagCoordi.exitCmdMode = 2; return TRUE; }
	else if (flag == CMD_C1ES) { this->FlagCoordi.C1ES = 2; return TRUE; }
	else if (flag == CMD_C1CD) { this->FlagCoordi.C1CD = 2; return TRUE; }
	else if (flag == CMD_MDXY) {
		this->FlagCoordi.MDXY = 2; 
		this->MSInfo.flagSingle = option;
		return TRUE; 
	}
	else if (flag == CMD_INVX) {
		this->FlagCoordi.INVX = 2;
		this->flagInvNum = option;
		return TRUE; 
	}	
	else if (flag == CMD_ZBMD) { this->FlagCoordi.ZBMD = 2; return TRUE; }
	else if (flag == CMD_INMD) { this->FlagCoordi.INMD = 2; return TRUE; }
	else if (flag == CMD_INEH) { this->FlagCoordi.INEH = 2; return TRUE; }
	else if (flag == CMD_INEL) { this->FlagCoordi.INEL = 2; return TRUE; }
	else if (flag == CMD_PVEM) { this->FlagCoordi.PVEM = 2; return TRUE; }
	else if (flag == CMD_LOMM) { this->FlagCoordi.LOMM = 2; return TRUE; }
	else if (flag == CMD_DBUG) { this->FlagCoordi.DBUG = 2; return TRUE; }
	else if (flag == CMD_BVER) { this->FlagCoordi.BVER = 2; return TRUE; }
	else if (flag == CMD_INFO) { this->FlagCoordi.INFO = 2; return TRUE; }
	//else if (flag == CMD_EEPR) { this->FlagCoordi.EEPR = 2; return TRUE; }
	//else if (flag == CMD_EEPI) { this->FlagCoordi.EEPI = 2; return TRUE; }
	else if (flag == CMD_CH0S) { this->FlagCoordi.CH0S = 2; return TRUE; }
	else if (flag == CMD_CH0R) { this->FlagCoordi.CH0R = 2; return TRUE; }
	else if (flag == CMD_CH1S) { this->FlagCoordi.CH1S = 2; return TRUE; }
	else if (flag == CMD_CH1R) { this->FlagCoordi.CH1R = 2; return TRUE; }
	else if (flag == CMD_TIME) { this->FlagCoordi.TIME = 2; return TRUE; }
	else if (flag == CMD_IDSC) { this->FlagCoordi.IDSC = 2; return TRUE; }
	else if (flag == CMD_TXPK) { this->FlagCoordi.TXPK = 2; return TRUE; }
	else if (flag == CMD_ZB_IS) { this->FlagCoordi.cmdZBis = 2; return TRUE; }
	else return FALSE;
}

BOOL CoordinatorReal::SetInvData(const TCHAR *src, INT InvNum)
{
	this->InvData[InvNum].type[0] = src[0];
	this->InvData[InvNum].type[1] = src[1];
	this->InvData[InvNum].type[2] = src[2];
	this->InvData[InvNum].type[3] = src[3];
	this->InvData[InvNum].id[0] = src[4];
	this->InvData[InvNum].id[1] = src[5];
	this->InvData[InvNum].id[2] = src[6];
	this->InvData[InvNum].phase[0] = src[7];
	this->InvData[InvNum].power[0] = src[8];
	this->InvData[InvNum].power[1] = src[9];
	this->InvData[InvNum].power[2] = src[10];
	return TRUE;
}

BOOL CoordinatorReal::SetDeviceModeData(const TCHAR *src, INT option)
{
	if (option == 0) {
		this->DeviceMode.INMD[0] = src[0];
		this->DeviceMode.INMD[1] = src[1];
		this->DeviceMode.INMD[2] = NULL;
		this->DeviceMode.ZBMD[0] = src[2];
		this->DeviceMode.ZBMD[1] = src[3];
		this->DeviceMode.ZBMD[2] = NULL;
		this->DeviceMode.PVEM[0] = src[4];
		this->DeviceMode.PVEM[1] = src[5];
		this->DeviceMode.PVEM[2] = NULL;
		this->DeviceMode.LOMM[0] = src[6];
		this->DeviceMode.LOMM[1] = src[7];
		this->DeviceMode.LOMM[2] = NULL;
	}
	else if (option == 1) {
		this->DeviceMode.INMD[0] = src[0];
		this->DeviceMode.INMD[1] = src[1];
		this->DeviceMode.INMD[2] = NULL;
	}
	else if (option == 2) {
		this->DeviceMode.ZBMD[0] = src[0];
		this->DeviceMode.ZBMD[1] = src[1];
		this->DeviceMode.ZBMD[2] = NULL;
	}
	else if (option == 3) {
		this->DeviceMode.PVEM[0] = src[0];
		this->DeviceMode.PVEM[1] = src[1];
		this->DeviceMode.PVEM[2] = NULL;
	}
	else if (option == 4) {
		this->DeviceMode.LOMM[0] = src[0];
		this->DeviceMode.LOMM[1] = src[1];
		this->DeviceMode.LOMM[2] = NULL;
	}
	else return FALSE;
	return TRUE;
}

BOOL CoordinatorReal::SetWriteData(const TCHAR *src)
{
	INT cnt;
	INT size = this->sizeWriteData;
	for (cnt = 0; *(src + cnt) != NULL; cnt++) {
		if (cnt > size) return FALSE;
		this->writeData[cnt] = *(src + cnt);
	}
	this->writeData[cnt] = NULL;
	return TRUE;
}

BOOL CoordinatorReal::GetMSIFlagSingle(void)
{
	return this->MSInfo.flagSingle;
}

UCHAR CoordinatorReal::GetMSIFlagModuleNum(void)
{
	return this->MSInfo.flagModuleNum;
}

INT CoordinatorReal::GetMSIModuleDataStr(TCHAR *dest)
{
	INT cnt;
	for (cnt = 0; cnt < 32; cnt++)
	{
		*(dest + cnt) = this->MSInfo.moduleDataStr[cnt];
	}
	*(dest + cnt) = NULL;
	return TRUE;
}

TCHAR CoordinatorReal::GetFlagInvNum(void)
{
	return this->flagInvNum;
}

BOOL CoordinatorReal::GetPortName(TCHAR *dest)
{
	INT cnt;
	INT size = this->sizePortName;
	for (cnt = 0; this->portName[cnt] != NULL; cnt++) {
		if (cnt > size) return FALSE;
		*(dest + cnt) = this->portName[cnt];
	}
	*(dest + cnt)  = NULL;
	return TRUE;
}

TCHAR CoordinatorReal::GetMode()
{
	return this->flagMode;
}

BOOL CoordinatorReal::GetInvData(TCHAR *dest, INT invNum)
{
	dest[0] = this->InvData[invNum].type[0];
	dest[1] = this->InvData[invNum].type[1];
	dest[2] = this->InvData[invNum].type[2];
	dest[3] = this->InvData[invNum].type[3];
	dest[4] = this->InvData[invNum].id[0];
	dest[5] = this->InvData[invNum].id[1];
	dest[6] = this->InvData[invNum].id[2];
	dest[7] = this->InvData[invNum].phase[0];
	dest[8] = this->InvData[invNum].power[0];
	dest[9] = this->InvData[invNum].power[1];
	dest[10] = this->InvData[invNum].power[2];
	return TRUE;
}

BOOL CoordinatorReal::GetWriteData(TCHAR *dest)
{
	INT cnt;
	for (cnt = 0; this->writeData[cnt] != NULL; cnt++) {
		if (cnt > this->sizeWriteData) return FALSE;
		dest[cnt] = this->writeData[cnt];
	}
	return TRUE;
}

INT CoordinatorReal::ComparePortName(const TCHAR *src)
{
	return strcmp(this->portName, src);
}

INT CoordinatorReal::ProcessING()
{
	if (this->FlagCoordi.enterCmdMode == 1) { this->FlagCoordi.enterCmdMode--; return CMD_ENTER; }
	else if (this->FlagCoordi.C1ES == 1) { this->FlagCoordi.C1ES--; return CMD_C1ES; }
	else if (this->FlagCoordi.C1CD == 1) { this->FlagCoordi.C1CD--; return CMD_C1CD; }
	else if (this->FlagCoordi.MDXY == 1) {
		if (this->MSInfo.flagModuleNum > 255 || this->MSInfo.moduleCount < 0) {
			this->FlagCoordi.MDXY--;
			return FALSE;
		}
		else if (this->MSInfo.flagModuleNum < 256 && this->MSInfo.moduleCount > 1) {
			this->MSInfo.flagModuleNum++;
			this->MSInfo.moduleCount--;
			this->FlagCoordi.MDXY++;
			return CMD_MDXY;
		}
		else if (this->MSInfo.flagModuleNum == 255 || this->MSInfo.moduleCount == 1){
			this->MSInfo.flagModuleNum++;
			this->MSInfo.moduleCount--;
			this->FlagCoordi.MDXY--;
			return CMD_MDXY;
		}			
	}
	else if (this->FlagCoordi.INVX == 1) {
		if (this->flagInvNum > 15) {
			this->FlagCoordi.INVX--; 
			return FALSE; 
		}
		else if (this->flagInvNum < 15) { 
			this->flagInvNum++; 
			this->FlagCoordi.INVX++;
			return CMD_INVX;
		}
		else { 
			this->FlagCoordi.INVX--; 
			this->flagInvNum = 0;
			return CMD_INVX;
		}		
	}	
	else if (this->FlagCoordi.ZBMD == 1) { this->FlagCoordi.ZBMD--; return CMD_ZBMD; }
	else if (this->FlagCoordi.INMD == 1) { this->FlagCoordi.INMD--; return CMD_INMD; }
	else if (this->FlagCoordi.INEH == 1) { this->FlagCoordi.INEH--; return CMD_INEH; }
	else if (this->FlagCoordi.INEL == 1) { this->FlagCoordi.INEL--; return CMD_INEL; }
	else if (this->FlagCoordi.PVEM == 1) { this->FlagCoordi.PVEM--; return CMD_PVEM; }
	else if (this->FlagCoordi.LOMM == 1) { this->FlagCoordi.LOMM--; return CMD_LOMM; }
	else if (this->FlagCoordi.DBUG == 1) { this->FlagCoordi.DBUG--; return CMD_DBUG; }
	else if (this->FlagCoordi.BVER == 1) { this->FlagCoordi.BVER--; return CMD_BVER; }
	else if (this->FlagCoordi.INFO == 1) { this->FlagCoordi.INFO--; return CMD_INFO; }
	//else if (this->FlagCoordi.EEPR == 1) { this->FlagCoordi.EEPR--; return CMD_EEPR; }
	//else if (this->FlagCoordi.EEPI == 1) { this->FlagCoordi.EEPI--; return CMD_EEPI; }
	else if (this->FlagCoordi.TIME == 1) { this->FlagCoordi.TIME--; return CMD_TIME; }
	else if (this->FlagCoordi.IDSC == 1) { this->FlagCoordi.IDSC--; return CMD_IDSC; }
	else if (this->FlagCoordi.TXPK == 1) { this->FlagCoordi.TXPK--; return CMD_TXPK; }
	else if (this->FlagCoordi.CH0S == 1) { this->FlagCoordi.CH0S--; return CMD_CH0S; }
	else if (this->FlagCoordi.CH0R == 1) { this->FlagCoordi.CH0R--; return CMD_CH0R; }
	else if (this->FlagCoordi.CH1S == 1) { this->FlagCoordi.CH1S--; return CMD_CH1S; }
	else if (this->FlagCoordi.CH1R == 1) { this->FlagCoordi.CH1R--; return CMD_CH1R; }
	else if (this->FlagCoordi.cmdZBis == 1) { this->FlagCoordi.cmdZBis--; return CMD_ZB_IS; }
	else if (this->FlagCoordi.exitCmdMode == 1) { this->FlagCoordi.exitCmdMode--; return CMD_EXIT; }
	else return FALSE;
}

INT CoordinatorReal::GetTXCmd()
{	
	//if (this->FlagCoordi.enterCmdMode == 2) { strcpy_s(str, "###");  this->FlagCoordi.enterCmdMode--; retValue = CMD_ENTER; }
	if (this->FlagCoordi.enterCmdMode == 2) { this->FlagCoordi.enterCmdMode--; return CMD_ENTER; }
	else if (this->FlagCoordi.C1ES == 2) { this->FlagCoordi.C1ES--; return CMD_C1ES; }
	else if (this->FlagCoordi.C1CD == 2) { this->FlagCoordi.C1CD--; return CMD_C1CD; }
	else if (this->FlagCoordi.MDXY == 2) { this->FlagCoordi.MDXY--; return CMD_MDXY; }
	else if (this->FlagCoordi.INEH == 2) { this->FlagCoordi.INEH--; return CMD_INEH; }
	else if (this->FlagCoordi.INEL == 2) { this->FlagCoordi.INEL--; return CMD_INEL; }
	else if (this->FlagCoordi.INVX == 2) { this->FlagCoordi.INVX--; return CMD_INVX; }	
	else if (this->FlagCoordi.INMD == 2) { this->FlagCoordi.INMD--; return CMD_INMD; }
	else if (this->FlagCoordi.ZBMD == 2) { this->FlagCoordi.ZBMD--; return CMD_ZBMD; }
	else if (this->FlagCoordi.PVEM == 2) { this->FlagCoordi.PVEM--; return CMD_PVEM; }
	else if (this->FlagCoordi.LOMM == 2) { this->FlagCoordi.LOMM--; return CMD_LOMM; }
	else if (this->FlagCoordi.INFO == 2) { this->FlagCoordi.INFO--; return CMD_INFO; }
	else if (this->FlagCoordi.BVER == 2) { this->FlagCoordi.BVER--; return CMD_BVER; }
	else if (this->FlagCoordi.DBUG == 2) { this->FlagCoordi.DBUG--; return CMD_DBUG; }
	//else if (this->FlagCoordi.EEPR == 2) { strcpy_s(str, "DEEEPR"); this->FlagCoordi.EEPR--; retValue = CMD_EEPI; }
	//else if (this->FlagCoordi.EEPI == 2) { strcpy_s(str, "DEEEPI"); this->FlagCoordi.EEPI--; retValue = CMD_EEPI; }
	else if (this->FlagCoordi.TIME == 2) { this->FlagCoordi.TIME--; return CMD_TIME; }
	else if (this->FlagCoordi.IDSC == 2) { this->FlagCoordi.IDSC--; return CMD_IDSC; }
	else if (this->FlagCoordi.TXPK == 2) { this->FlagCoordi.TXPK--; return CMD_TXPK; }
	else if (this->FlagCoordi.CH0S == 2) { this->FlagCoordi.CH0S--; return CMD_CH0S; }
	else if (this->FlagCoordi.CH0R == 2) { this->FlagCoordi.CH0R--; return CMD_CH0R; }
	else if (this->FlagCoordi.CH1S == 2) { this->FlagCoordi.CH1S--; return CMD_CH1S; }
	else if (this->FlagCoordi.CH1R == 2) { this->FlagCoordi.CH1R--; return CMD_CH1R; }
	else if (this->FlagCoordi.cmdZBis == 2) { this->FlagCoordi.cmdZBis--; return CMD_ZB_IS; }
	else if (this->FlagCoordi.exitCmdMode == 2) { this->FlagCoordi.exitCmdMode--; return CMD_EXIT; }
	else return FALSE;		
}

BOOL CoordinatorReal::VerifyOK(const TCHAR *src)
{	
	if (*(src + 0) == 'O' && *(src + 1) == 'K' && *(src + 2) == '\r') return TRUE;
	else return FALSE;	
}

void CoordinatorReal::ResetFlag()
{
	this->FlagCoordi = { 0, };
	this->flagInvNum = 0;
	this->flagMode = 0;
	this->MSInfo.flagModuleNum = 0;
	this->writeData[0] = NULL;
	this->portName[0] = NULL;
}

CoordinatorReal::~CoordinatorReal()
{
	free(this->writeData);
	free(this->portName);
}

INT GetInvTypeIndex(const TCHAR *src)
{
	if (strcmp("DASS", src) == 0) return 1;
	else if (strcmp("E&P3", src) == 0) return 2;
	else if (strcmp("E&P5", src) == 0) return 3;
	else if (strcmp("HANS", src) == 0) return 4;
	else if (strcmp("HEXP", src) == 0) return 5;
	else if (strcmp("EKOS", src) == 0) return 6;
	else if (strcmp("WILL", src) == 0) return 7;
	else if (strcmp("ABBI", src) == 0) return 8;
	else if (strcmp("REFU", src) == 0) return 9;
	else if (strcmp("SUNG", src) == 0) return 10;
	else if (strcmp("REMS", src) == 0) return 11;
	else return -1;
}