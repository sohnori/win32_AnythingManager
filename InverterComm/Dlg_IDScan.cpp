#include "Dlg_IDScan.h"
#include "SerialCommHandler.h"
#include "Inverter_ABBI.h"
#include "Inverter_DASS.h"
#include "Inverter_E_P3.h"
#include "Inverter_E_P5.h"
#include "Inverter_EKOS.h"
#include "Inverter_HANS.h"
#include "Inverter_HEXP.h"
#include "Inverter_REFU.h"
#include "Inverter_SUNG.h"
#include "Inverter_WILL.h"
#include "Equipment_REMS.h"

#define IDSCAN_ABBI		0x0001
#define IDSCAN_DASS		0x0002
#define IDSCAN_E_P3		0x0004
#define IDSCAN_E_P5		0x0008
#define IDSCAN_EKOS		0x0010
#define IDSCAN_HANS		0x0020
#define IDSCAN_HEXP		0x0040
#define IDSCAN_REFU		0x0080
#define IDSCAN_SUNG		0x0100
#define IDSCAN_WILL		0x0200
#define IDSCAN_REMSPV1P		0x0400
#define IDSCAN_REMSPV3P		0x0800

extern SerialCommHandler Serial;
extern UCHAR portNum;
extern TCHAR SerialBuff[512];
extern DWORD dwBytesRead;
extern TCHAR flagRX;

static BOOL flagIDScan = 0;
static UCHAR cntSearchingID = 0;
static UCHAR cntSearchedID = 0;
static INT flagInverter = 0;

// 대화상자 핸들
static HWND hDlgCurrent;

// 스레드 핸들
static HANDLE hIDScanThread;

// 스레드 함수
DWORD WINAPI IDScanThreadFunc(LPVOID prc);

// 기타 함수
INT IDScanABBI(UCHAR portNum);
INT IDScanDASS(UCHAR portNum);
INT IDScanE_P3(UCHAR portNum);
INT IDScanE_P5(UCHAR portNum);
INT IDScanEKOS(UCHAR portNum);
INT IDScanHANS(UCHAR portNum);
INT IDScanHEXP(UCHAR portNum);
INT IDScanREFU(UCHAR portNum);
INT IDScanSUNG(UCHAR portNum);
INT IDScanWILL(UCHAR portNum);
INT IDScanREMSPV1P(UCHAR portNum);
INT IDScanREMSPV3P(UCHAR portNum);

BOOL CALLBACK IDScanDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG:
		DWORD ThreadID;
		hDlgCurrent = hDlg;
		CheckDlgButton(hDlg, IDC_CHECK_ABBI, BST_CHECKED);
		hIDScanThread = CreateThread(NULL, 0, IDScanThreadFunc, &portNum, 0, &ThreadID);
		CloseHandle(hIDScanThread);
		//SuspendThread(hIDScanThread);
		//ResumeThread(hIDScanThread);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_IDSCAN:
			flagIDScan = 0;
			cntSearchingID = 0;
			cntSearchedID = 0;
			flagInverter = 0;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_IDALL) == BST_CHECKED) cntSearchingID = 0xFF;
			else cntSearchingID = GetDlgItemInt(hDlg, IDC_EDIT_INVCNT, NULL, FALSE);
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_ABBI) == BST_CHECKED) flagInverter |= (INT)IDSCAN_ABBI;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_DASS) == BST_CHECKED) flagInverter |= (INT)IDSCAN_DASS;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_E_P3) == BST_CHECKED) flagInverter |= (INT)IDSCAN_E_P3;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_E_P5) == BST_CHECKED) flagInverter |= (INT)IDSCAN_E_P5;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_EKOS) == BST_CHECKED) flagInverter |= (INT)IDSCAN_EKOS;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_HANS) == BST_CHECKED) flagInverter |= (INT)IDSCAN_HANS;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_HEXP) == BST_CHECKED) flagInverter |= (INT)IDSCAN_HEXP;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_REFU) == BST_CHECKED) flagInverter |= (INT)IDSCAN_REFU;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_SUNG) == BST_CHECKED) flagInverter |= (INT)IDSCAN_SUNG;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_WILL) == BST_CHECKED) flagInverter |= (INT)IDSCAN_WILL;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_REMSPV1P) == BST_CHECKED) flagInverter |= (INT)IDSCAN_REMSPV1P;
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_REMSPV3P) == BST_CHECKED) flagInverter |= (INT)IDSCAN_REMSPV3P;
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, 0, -1);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
			flagIDScan = TRUE;
			return TRUE;
		case IDC_BUTTON_IDSTOP:
			flagIDScan = 0;
			cntSearchingID = 0;
			cntSearchedID = 0;
			flagInverter = 0;
			return TRUE;
		case IDC_CHECK_IDALL:
			if (IsDlgButtonChecked(hDlg, IDC_CHECK_IDALL) == TRUE) {
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_INVCNT), FALSE);
			}
			else EnableWindow(GetDlgItem(hDlg, IDC_EDIT_INVCNT), TRUE);
		}
		break;
	}
	return FALSE;
}

DWORD WINAPI IDScanThreadFunc(LPVOID prc)
{
	UCHAR portNum;			
	for (;;) {
		Sleep(500);
		portNum = *(UCHAR*)prc;
		if (portNum != 0 && flagIDScan==TRUE) {
			if (flagInverter&IDSCAN_ABBI) IDScanABBI(portNum);
			if (flagInverter&IDSCAN_DASS) IDScanDASS(portNum);
			if (flagInverter&IDSCAN_E_P3) IDScanE_P3(portNum);
			if (flagInverter&IDSCAN_E_P5) IDScanE_P5(portNum);
			if (flagInverter&IDSCAN_EKOS) IDScanEKOS(portNum);
			if (flagInverter&IDSCAN_HANS) IDScanHANS(portNum);
			if (flagInverter&IDSCAN_HEXP) IDScanHEXP(portNum);
			if (flagInverter&IDSCAN_REFU) IDScanREFU(portNum);
			if (flagInverter&IDSCAN_SUNG) IDScanSUNG(portNum);
			if (flagInverter&IDSCAN_WILL) IDScanWILL(portNum);
			if (flagInverter&IDSCAN_REMSPV1P) IDScanREMSPV1P(portNum);
			if (flagInverter&IDSCAN_REMSPV3P) IDScanREMSPV3P(portNum);
			flagIDScan = 0;
		}
	}
	return 0;
}

INT IDScanABBI(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 64; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), cnt, ABBI_CMD_DSP, ABBI_TYPE_GRIDV);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			wsprintf(msg, "ABBI: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanDASS(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	TCHAR invID[4];
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		wsprintf(invID, "%03d", cnt);
		buffLen = GetInvDASSRequest((char*)buffTX, sizeof(buffTX), invID, DASS_CMD_MOD);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvDASSResponse((unsigned char*)SerialBuff, invID, DASS_CMD_MOD) > 0) {
			wsprintf(msg, "DASS: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanE_P3(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;		
		buffLen = GetInvE_P3Request((unsigned char*)buffTX, sizeof(buffTX), cnt);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvE_P3Response((unsigned char*)SerialBuff, cnt) > 0) {
			wsprintf(msg, "E_P3: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanE_P5(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetInvE_P5Request((unsigned char*)buffTX, sizeof(buffTX), cnt);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvE_P5Response((unsigned char*)SerialBuff, cnt) > 0) {
			wsprintf(msg, "E_P5: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanEKOS(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetInvEKOSRequest((unsigned char*)buffTX, sizeof(buffTX), cnt, EKOS_FC_04, EKOS_ADDR_PVV, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, cnt, EKOS_FC_04) > 0) {
			wsprintf(msg, "EKOS: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanHANS(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetInvHANSRequest((unsigned char*)buffTX, sizeof(buffTX), cnt, HANS_PHASE_ONE);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvHANSResponse((unsigned char*)SerialBuff, cnt) > 0) {
			wsprintf(msg, "HANS1P: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanHEXP(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetInvHEXPRequest((unsigned char*)buffTX, sizeof(buffTX), cnt, HEXP_PARA_FAULT);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvHEXPResponse((unsigned char*)SerialBuff, cnt, HEXP_PARA_FAULT) > 0) {
			wsprintf(msg, "HEXP: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanREFU(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 33; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetInvREFURequest((unsigned char*)buffTX, sizeof(buffTX), cnt, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_FWVER, 0);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, cnt, REFU_PARAMETER_FWVER) > 0) {
			wsprintf(msg, "REFU: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanSUNG(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetInvSUNGRequest((unsigned char*)buffTX, sizeof(buffTX), cnt, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_DEVTYPE, 1);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, cnt, SUNG_FUNCTION_CODE_READONLY) > 0) {
			wsprintf(msg, "SUNG: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanWILL(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetInvWILLRequest((unsigned char*)buffTX, sizeof(buffTX), cnt,(unsigned char*) WILL_CMD_READ, (unsigned char*)WILL_ADDR_PVST, 1);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyInvWILLResponse((unsigned char*)SerialBuff, cnt) > 0) {
			wsprintf(msg, "WILL: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanREMSPV1P(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetREMSEquipRequest((unsigned char*)buffTX, sizeof(buffTX), cnt, REMS_CMD_PV_ONE);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyREMSEquipResponse((unsigned char*)SerialBuff, cnt, REMS_CMD_PV_ONE) > 0) {
			wsprintf(msg, "REMS PV 1P: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}

INT IDScanREMSPV3P(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	INT cnt;
	INT editLength;
	for (cnt = 0; cnt < 100; cnt++) {
		if (cntSearchedID >= cntSearchingID || flagIDScan != TRUE) break;
		flagRX = 0;
		buffLen = GetREMSEquipRequest((unsigned char*)buffTX, sizeof(buffTX), cnt, REMS_CMD_PV_THREE);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(500);
		if (flagRX != 0 && VerifyREMSEquipResponse((unsigned char*)SerialBuff, cnt, REMS_CMD_PV_THREE) > 0) {
			wsprintf(msg, "REMS PV 3P: %d, ", cnt);
			editLength = GetWindowTextLength(GetDlgItem(hDlgCurrent, IDC_EDIT_RESULT_ID));
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlgCurrent, IDC_EDIT_RESULT_ID, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)msg);
			cntSearchedID++;
		}
	}
	return 1;
}
