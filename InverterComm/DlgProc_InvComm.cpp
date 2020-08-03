#include "DlgProc_InvComm.h"
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

#define REQDATA_ABBI		0
#define REQDATA_DASS		1
#define REQDATA_E_P3		2
#define REQDATA_E_P5		3
#define REQDATA_EKOS		4
#define REQDATA_HANS		5
#define REQDATA_HEXP		6
#define REQDATA_REFU		7
#define REQDATA_SUNG		8
#define REQDATA_WILL		9
#define REQDATA_REMSPV1P		10
#define REQDATA_REMSPV3P		11

static BOOL HtoA2(TCHAR value, TCHAR *retHexHigh, TCHAR *retHexLow);
static INT AtoHstr(TCHAR *dest, TCHAR *src, INT dataCount);

extern SerialCommHandler Serial;
extern UCHAR portNum;
extern TCHAR SerialBuff[512];
extern DWORD dwBytesRead;
extern TCHAR flagRX;

static BOOL flagReqData = 0;
static INT invID = 0;
static INT flagInverter = 0;

// 대화상자 핸들
static HWND hDlgCurrent;

// 스레드 핸들
static HANDLE hReqDataThread;

// 스레드 함수
DWORD WINAPI ReqDataThreadFunc(LPVOID prc);

//// 데이터 요청 버튼 서브클래스 프로시저
//WNDPROC OldDlgProc;
//LRESULT CALLBACK ReqButtonSubProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// 인버터 통신 처리 함수
INT ReqDataABBI(UCHAR portNum);
INT ReqDataDASS(UCHAR portNum);
INT ReqDataE_P3(UCHAR portNum);
INT ReqDataE_P5(UCHAR portNum);
INT ReqDataEKOS(UCHAR portNum);
INT ReqDataHANS(UCHAR portNum);
INT ReqDataHEXP(UCHAR portNum);
INT ReqDataREFU(UCHAR portNum);
INT ReqDataSUNG(UCHAR portNum);
INT ReqDataWILL(UCHAR portNum);
INT ReqDataREMSPV1P(UCHAR portNum);
INT ReqDataREMSPV3P(UCHAR portNum);

struct tagInvData { // PV(one or three) data
	unsigned short pvv; // V
	unsigned short pvi; // A
	unsigned char pvifloat;
	unsigned long pvp; // W
	unsigned char pvpfloat;
	unsigned short outrv; // V
	unsigned short outsv; // V
	unsigned short outtv; // V
	unsigned short outri; // A
	unsigned char outrifloat;
	unsigned short outsi; // A
	unsigned char outsifloat;
	unsigned short outti; // A
	unsigned char outtifloat;
	unsigned long outp; // W
	unsigned char outpfloat;
	unsigned short pf; // %_one decimal point
	unsigned char pffloat;
	unsigned short fr; // Hz_one decimal point
	unsigned char frfloat;
	unsigned long totalP; // kWh
	unsigned short status;	
}; // 26 or 36 bytes

BOOL CALLBACK InvCommDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	TCHAR buffStr[1024];
	INT buffCnt;
	INT editLength;
	SYSTEMTIME st;
	TCHAR sTime[50];
	DWORD ThreadID;
	switch (iMessage) {
	case WM_INITDIALOG:
		hDlgCurrent = hDlg;
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("ABBI"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("DASS"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("E_P3"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("E_P5"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("EKOS"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("HANS"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("HEXP"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("REFU"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("SUNG"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("WILL"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("REMSPV1P"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_ADDSTRING, 0, (LPARAM)TEXT("REMSPV3P"));
		SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_SETCURSEL, 0, 0);
		CheckDlgButton(hDlg, IDC_RADIO_ASCII, BST_CHECKED);		
		SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_SETSEL, 0, -1);
		SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
		hReqDataThread = CreateThread(NULL, 0, ReqDataThreadFunc, &portNum, 0, &ThreadID);
		CloseHandle(hReqDataThread);
		SetDlgItemText(hDlg, IDC_EDIT_LOG, TEXT("Program start~~!"));		
		//SuspendThread(hReqDataThread);
		//ResumeThread(hReqDataThread);
		//OldDlgProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(hDlg, IDC_BUTTON_REQDATA), GWLP_WNDPROC, (LONG_PTR)ReqButtonSubProc);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_REQDATA:
			flagReqData = FALSE;
			SendDlgItemMessage(hDlg, IDC_EDIT_RESULT_INVDATA, EM_SETSEL, 0, -1);
			SendDlgItemMessage(hDlg, IDC_EDIT_RESULT_INVDATA, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
			flagInverter = SendDlgItemMessage(hDlg, IDC_COMBO_INVTYPE, CB_GETCURSEL, 0, 0);
			invID = GetDlgItemInt(hDlg, IDC_EDIT_INVID, 0, FALSE);
			if (invID > 99) {
				MessageBox(hDlg, TEXT("유효 인버터 아이디 범위 초과~!"), TEXT("경고"), MB_OK);
				return TRUE;
			}
			flagReqData = TRUE;
			return TRUE;
		case IDC_BUTTON_DEL:
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_SETSEL, 0, -1);
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
			return TRUE;
		}
		break;	
	case WM_USER:
		GetLocalTime(&st);
		wsprintf(sTime, TEXT("[%d-%02d-%02d %02d:%02d:%02d_RX]"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		if (IsDlgButtonChecked(hDlg, IDC_RADIO_HEX) == BST_CHECKED) {
			buffCnt = AtoHstr(buffStr, (TCHAR*)lParam, (INT)wParam);						
			editLength = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_LOG));
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			editLength = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_LOG));
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)buffStr);			
		}
		else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ASCII) == BST_CHECKED) {
			editLength = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_LOG));
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sTime);
			editLength = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_LOG));			
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_SETSEL, editLength, editLength);
			SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)lParam);						
		}
		editLength = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_LOG));
		SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_SETSEL, editLength, editLength);
		SendDlgItemMessage(hDlg, IDC_EDIT_LOG, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"\r\n");
		return TRUE;	
	}
	return FALSE;
}

//LRESULT CALLBACK ReqButtonSubProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
//{
//	switch (iMessage) {
//	case WM_KEYDOWN:
//		if (wParam == VK_RETURN) {
//			SendMessage(hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_REQDATA, 0);
//			SetFocus(GetDlgItem(hWnd, IDC_BUTTON_REQDATA));
//		}
//		break;
//	}
//	return CallWindowProc(OldDlgProc, hWnd, iMessage, wParam, lParam);
//}

DWORD WINAPI ReqDataThreadFunc(LPVOID prc)
{
	UCHAR portNum;
	for (;;) {
		Sleep(600);
		portNum = *(UCHAR*)prc;
		if (portNum != 0 && flagReqData == TRUE) {
			if (flagInverter == REQDATA_ABBI) ReqDataABBI(portNum);
			if (flagInverter == REQDATA_DASS) ReqDataDASS(portNum);
			if (flagInverter == REQDATA_E_P3) ReqDataE_P3(portNum);
			if (flagInverter == REQDATA_E_P5) ReqDataE_P5(portNum);
			if (flagInverter == REQDATA_EKOS) ReqDataEKOS(portNum);
			if (flagInverter == REQDATA_HANS) ReqDataHANS(portNum);
			if (flagInverter == REQDATA_HEXP) ReqDataHEXP(portNum);
			if (flagInverter == REQDATA_REFU) ReqDataREFU(portNum);
			if (flagInverter == REQDATA_SUNG) ReqDataSUNG(portNum);
			if (flagInverter == REQDATA_WILL) ReqDataWILL(portNum);
			if (flagInverter == REQDATA_REMSPV1P) ReqDataREMSPV1P(portNum);
			if (flagInverter == REQDATA_REMSPV3P) ReqDataREMSPV3P(portNum);			
			flagReqData = 0;			
		}
	}
	return 0;
}

INT ReqDataABBI(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_PVV1);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_PVV1);
			invData.pvv = (unsigned short)invValue; // V
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_PVV2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_PVV2);
			invData.pvv += (unsigned short)invValue; // V
			invData.pvv /= 2;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_PVI1);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_PVI1);
			invData.pvi = (unsigned short)invValue; // A
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_PVI2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_PVI2);
			invData.pvi += (unsigned short)invValue; // A
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_PVP1);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_PVP1);
			invData.pvp = (unsigned short)invValue; // W
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_PVP2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_PVP2);
			invData.pvp += (unsigned short)invValue; // W
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_GRIDRV);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_GRIDRV);
			invData.outrv = (unsigned short)invValue; // V
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_GRIDSV);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_GRIDSV);
			invData.outsv = (unsigned short)invValue; // V
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_GRIDTV);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_GRIDTV);
			invData.outtv = (unsigned short)invValue; // V
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_GRIDRI);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_GRIDRI);
			invData.outri = (unsigned short)invValue; // A
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_GRIDSI);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_GRIDSI);
			invData.outsi = (unsigned short)invValue; // A
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_GRIDTI);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_GRIDTI);
			invData.outti = (unsigned short)invValue; // A
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_GRIDP);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_GRIDP);
			invData.outp = (unsigned short)invValue; // W
			invData.pf = 0; //  no table
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_DSP, ABBI_TYPE_GRIDFR);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_GRIDFR);
			invData.fr = (unsigned short)invValue; // Hz
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvABBIRequest(buffTX, sizeof(buffTX), invID, ABBI_CMD_PDAY, ABBI_PARTIAL_ENERGYT);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvABBIResponse((unsigned char*)SerialBuff) > 0) {
			GetInvABBIValue(&invValue, (unsigned char*)SerialBuff, ABBI_VALUE_ENERGYT);
			invData.totalP = invValue / 1000; // Wh >> kWh
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("ABB 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %dA, 태양광발전량: %dW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %dA, 계통전류S: %dA, 계통전류T: %dA\r\n"
			"계통출력: %dW \r\n"
			"역률: %d, 주파수: %dHz\r\n"
			"누적발전량: %dkWh")
			, invID, invData.pvv, invData.pvi, invData.pvp, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outsi, invData.outti, invData.outp, invData.pf, invData.fr, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataDASS(UCHAR portNum)
{
	char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	TCHAR ID[4];
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		wsprintf(ID, "%03d", invID);
		flagRX = 0;
		buffLen = GetInvDASSRequest(buffTX, sizeof(buffTX), ID, DASS_CMD_ST1);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvDASSResponse((unsigned char*)SerialBuff, ID, DASS_CMD_ST1) > 0) {
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_PVV);
			invData.pvv = (unsigned short)invValue; // V
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_PVI);
			invData.pvi = (unsigned short)invValue / 10; // A
			invData.pvifloat = (unsigned short)invValue % 10;
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_PVP);
			invData.pvp = (unsigned short)invValue / 10; // kW
			invData.pvpfloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvDASSRequest(buffTX, sizeof(buffTX), ID, DASS_CMD_ST2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvDASSResponse((unsigned char*)SerialBuff, ID, DASS_CMD_ST2) > 0) {
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_GRIDRSV);
			invData.outrv = (unsigned short)invValue; // V
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_GRIDSTV);
			invData.outsv = (unsigned short)invValue; // V			
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_GRIDTRV);
			invData.outtv = (unsigned short)invValue; // V			
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_GRIDFR);
			invData.fr = (unsigned short)invValue / 10; // Hz	
			invData.frfloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvDASSRequest(buffTX, sizeof(buffTX), ID, DASS_CMD_ST3);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvDASSResponse((unsigned char*)SerialBuff, ID, DASS_CMD_ST3) > 0) {
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_GRIDRI);
			invData.outri = (unsigned short)invValue / 10; // A
			invData.outrifloat = (unsigned short)invValue % 10;
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_GRIDSI);
			invData.outsi = (unsigned short)invValue / 10; // A
			invData.outsifloat = (unsigned short)invValue % 10;
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_GRIDTI);
			invData.outti = (unsigned short)invValue / 10; // A
			invData.outtifloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvDASSRequest(buffTX, sizeof(buffTX), ID, DASS_CMD_ST4);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvDASSResponse((unsigned char*)SerialBuff, ID, DASS_CMD_ST4) > 0) {
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_GENP);
			invData.outp = (unsigned short)invValue / 10; // kW
			invData.outpfloat = (unsigned short)invValue % 10;
			GetInvDASSValue(&invValue, (unsigned char*)SerialBuff, DASS_VALUE_TOTALP);
			invData.totalP = (unsigned short)invValue; // kWh
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("DASS 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %d.%dkW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %d.%dA, 계통전류S: %d.%dA, 계통전류T: %d.%dA\r\n"
			"계통출력: %dW \r\n"
			"주파수: %d.%dHz\r\n"
			"누적발전량: %dkWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.pvpfloat, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outrifloat, invData.outsi, invData.outsifloat, invData.outti, invData.outtifloat, invData.outp, invData.outpfloat, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataE_P3(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {		
		flagRX = 0;
		buffLen = GetInvE_P3Request(buffTX, sizeof(buffTX), invID);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvE_P3Response((unsigned char*)SerialBuff, invID) > 0) {
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_PVV1);
			invData.pvv = (unsigned short)invValue/10; // V
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_PVI);
			invData.pvi = (unsigned short)invValue / 10; // A
			invData.pvifloat = (unsigned short)invValue % 10;
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_INPUTP);
			invData.pvp = (unsigned short)invValue/10; // kW
			invData.pvpfloat = (unsigned short)invValue % 10;
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_OUTPUTV);
			invData.outrv = (unsigned short)invValue / 10; // V
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_OUTPUTI);
			invData.outri = (unsigned short)invValue / 10; // A
			invData.outrifloat = (unsigned short)invValue % 10;
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_OUTPUTP);
			invData.outp = (unsigned short)invValue / 10; // kW
			invData.outpfloat = (unsigned short)invValue % 10;
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_POWERFACTOR);
			invData.pf = (unsigned short)invValue;
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_FREQUENCY);
			invData.fr = (unsigned short)invValue/10;
			invData.frfloat = (unsigned short)invValue % 10;
			GetInvE_P3Value(&invValue, (unsigned char*)SerialBuff, E_P3_VALUE_TOTALP);
			invData.totalP = (unsigned short)invValue;			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("E&P3 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %d.%dkW\r\n"
			"계통전압: %dV\r\n"
			"계통전류: %d.%dA\r\n"
			"계통출력: %d.%dkW\r\n"
			"역률: %d, 주파수: %d.%dHz\r\n"
			"누적발전량: %dkWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.pvpfloat, invData.outrv,
			invData.outri, invData.outrifloat, invData.outp, invData.outpfloat, invData.pf, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataE_P5(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetInvE_P5Request(buffTX, sizeof(buffTX), invID);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvE_P5Response((unsigned char*)SerialBuff, invID) > 0) {
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_PVV1);
			invData.pvv = (unsigned short)invValue / 10; // V
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_PVI1);
			invData.pvi = (unsigned short)invValue; // A
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_PVI2);
			invData.pvi += (unsigned short)invValue; // A
			invData.pvifloat = invData.pvi % 100;
			invData.pvi /= 100;
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_PVP1);
			invData.pvp = (unsigned short)invValue; // kW
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_PVP2);
			invData.pvp += (unsigned short)invValue; // kW
			invData.pvp /= 100;
			invData.pvpfloat = invData.pvp % 10;
			invData.pvp /= 10;
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_OUTPUTV);
			invData.outrv = (unsigned short)invValue / 10; // V
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_OUTPUTI);
			invData.outri = (unsigned short)invValue / 100; // A
			invData.outrifloat = (unsigned short)invValue % 100;
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_OUTPUTP);
			invData.outp = (unsigned short)invValue; // kW
			invData.outp /= 100;
			invData.outpfloat = invData.pvp % 10;
			invData.outp /= 10;			
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_FREQUENCY);
			invData.fr = (unsigned short)invValue / 10;
			invData.frfloat = (unsigned short)invValue % 10;
			GetInvE_P5Value(&invValue, (unsigned char*)SerialBuff, E_P5_VALUE_TODAYP);
			invData.totalP = (unsigned short)invValue;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("E&P5 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %d.%dkW\r\n"
			"계통전압: %dV\r\n"
			"계통전류: %d.%dA\r\n"
			"계통출력: %d.%dkW\r\n"
			"주파수: %d.%dHz\r\n"
			"누적발전량: %dkWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.pvpfloat, invData.outrv,
			invData.outri, invData.outrifloat, invData.outp, invData.outpfloat, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataEKOS(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetInvEKOSRequest(buffTX, sizeof(buffTX), invID, EKOS_FC_04, EKOS_ADDR_PVV, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, invID, EKOS_FC_04) > 0) {
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[3], EKOS_ADDR_PVV);
			invData.pvv = (unsigned short)invValue; // V
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvEKOSRequest(buffTX, sizeof(buffTX), invID, EKOS_FC_04, EKOS_ADDR_PVI, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, invID, EKOS_FC_04) > 0) {
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[3], EKOS_ADDR_PVI);
			invData.pvi = (unsigned short)invValue/10; // A
			invData.pvifloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvEKOSRequest(buffTX, sizeof(buffTX), invID, EKOS_FC_04, EKOS_ADDR_PVP, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, invID, EKOS_FC_04) > 0) {
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[3], EKOS_ADDR_PVP);
			invData.pvp = (unsigned short)invValue; // W			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvEKOSRequest(buffTX, sizeof(buffTX), invID, EKOS_FC_04, EKOS_ADDR_LINERV, 12);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, invID, EKOS_FC_04) > 0) {
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[3], EKOS_ADDR_LINERV);
			invData.outrv = (unsigned short)invValue; // V
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[7], EKOS_ADDR_LINESV);
			invData.outsv = (unsigned short)invValue; // V
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[11], EKOS_ADDR_LINETV);
			invData.outtv = (unsigned short)invValue; // V
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[15], EKOS_ADDR_LINERI);
			invData.outri = (unsigned short)invValue/10; // A
			invData.outrifloat = (unsigned short)invValue%10;
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[19], EKOS_ADDR_LINESI);
			invData.outsi = (unsigned short)invValue / 10; // A
			invData.outsifloat = (unsigned short)invValue % 10;
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[23], EKOS_ADDR_LINETI);
			invData.outti = (unsigned short)invValue / 10; // A
			invData.outtifloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvEKOSRequest(buffTX, sizeof(buffTX), invID, EKOS_FC_04, EKOS_ADDR_LINEP, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, invID, EKOS_FC_04) > 0) {
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[3], EKOS_ADDR_LINEP);
			invData.outp = (unsigned short)invValue; // W			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvEKOSRequest(buffTX, sizeof(buffTX), invID, EKOS_FC_04, EKOS_ADDR_PF, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, invID, EKOS_FC_04) > 0) {
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[3], EKOS_ADDR_PF);
			invData.pf = (unsigned short)invValue;			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvEKOSRequest(buffTX, sizeof(buffTX), invID, EKOS_FC_04, EKOS_ADDR_FR, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, invID, EKOS_FC_04) > 0) {
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[3], EKOS_ADDR_FR);
			invData.fr = (unsigned short)invValue/10; // W
			invData.frfloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvEKOSRequest(buffTX, sizeof(buffTX), invID, EKOS_FC_04, EKOS_ADDR_TOTALP, 4);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvEKOSResponse((unsigned char*)SerialBuff, invID, EKOS_FC_04) > 0) {
			GetInvEKOSValue(&invValue, (unsigned char*)&SerialBuff[3], EKOS_ADDR_TOTALP);
			invData.totalP = (unsigned short)invValue; // kWh			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }

		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("EKOS 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %dW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %d.%dA, 계통전류S: %d.%dA, 계통전류T: %d.%dA\r\n"
			"계통출력: %dW \r\n"
			"역률: %d, 주파수: %d.%dHz\r\n"
			"누적발전량: %dkWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outrifloat, invData.outsi, invData.outsifloat, invData.outti, invData.outtifloat, invData.outp, invData.pf, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataHANS(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetInvHANSRequest(buffTX, sizeof(buffTX), invID, HANS_PHASE_ONE);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvHANSResponse((unsigned char*)SerialBuff, invID) > 0) {
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_PVV1);
			invData.pvv = (unsigned short)invValue; // V
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_PVI1);
			invData.pvi = (unsigned short)invValue; // A
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_PVI2);
			invData.pvi += (unsigned short)invValue; // A
			invData.pvifloat = invData.pvi%10;
			invData.pvi /= 10;
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_PVP);
			invData.pvp = (unsigned short)invValue; // W
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_OUTPUTRV);
			invData.outrv = (unsigned short)invValue; // V
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_OUTPUTSV);
			invData.outsv = (unsigned short)invValue; // V
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_OUTPUTTV);
			invData.outtv = (unsigned short)invValue; // V
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_OUTPUTRI);
			invData.outri = (unsigned short)invValue/10; // A
			invData.outrifloat = (unsigned short)invValue % 10;
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_OUTPUTSI);
			invData.outsi = (unsigned short)invValue / 10; // A
			invData.outsifloat = (unsigned short)invValue % 10;
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_OUTPUTTI);
			invData.outti = (unsigned short)invValue / 10; // A
			invData.outtifloat = (unsigned short)invValue % 10;
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_OUTPUTP);
			invData.outp = (unsigned short)invValue; // W
			GetInvHANSValue(&invValue, (unsigned char*)SerialBuff, HANS_VALUE_TOTALP);
			invData.totalP = (unsigned short)invValue;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }

		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("HANS 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %dW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %d.%dA, 계통전류S: %d.%dA, 계통전류T: %d.%dA\r\n"
			"계통출력: %dW\r\n"
			"역률: %d, 주파수: %d.%dHz\r\n"
			"누적발전량: %dWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outrifloat, invData.outsi, invData.outsifloat, invData.outti, invData.outtifloat, invData.outp, invData.pf, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataHEXP(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetInvHEXPRequest(buffTX, sizeof(buffTX), invID, HEXP_PARA_PV);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvHEXPResponse((unsigned char*)SerialBuff, invID, HEXP_PARA_PV) > 0) {
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_SOLARV, HEXP_TYPE_H30XXS_ML);
			invData.pvv = (unsigned short)invValue; // V
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_SOLARI, HEXP_TYPE_H30XXS_ML);
			invData.pvi = (unsigned short)invValue/10; // A
			invData.pvifloat = (unsigned short)invValue%10;			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvHEXPRequest(buffTX, sizeof(buffTX), invID, HEXP_PARA_GRID);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvHEXPResponse((unsigned char*)SerialBuff, invID, HEXP_PARA_GRID) > 0) {			
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_LINERSV, HEXP_TYPE_H30XXS_ML);
			invData.outrv = (unsigned short)invValue; // V
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_LINESTV, HEXP_TYPE_H30XXS_ML);
			invData.outsv = (unsigned short)invValue; // V
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_LINETRV, HEXP_TYPE_H30XXS_ML);
			invData.outtv = (unsigned short)invValue; // V
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_LINEFR, HEXP_TYPE_H30XXS_ML);
			invData.fr = (unsigned short)invValue/10; // Hz
			invData.frfloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvHEXPRequest(buffTX, sizeof(buffTX), invID, HEXP_PARA_POWER);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvHEXPResponse((unsigned char*)SerialBuff, invID, HEXP_PARA_POWER) > 0) {
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_SOLARP, HEXP_TYPE_H30XXS_ML);
			invData.pvp = (unsigned short)invValue/10; // kW
			invData.pvpfloat = (unsigned short)invValue % 10;
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_LINEP, HEXP_TYPE_H30XXS_ML);
			invData.outp = (unsigned short)invValue / 10; // kW
			invData.outpfloat = (unsigned short)invValue % 10;
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_TODAYP, HEXP_TYPE_H30XXS_ML);
			invData.totalP = (unsigned short)invValue; // kWh
			GetInvHEXPValue(&invValue, (unsigned char*)SerialBuff, HEXP_VALUE_PF, HEXP_TYPE_H30XXS_ML);
			invData.pf = (unsigned short)invValue;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }

		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("HEXP 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %d.%dkW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %d.%dA, 계통전류S: %d.%dA, 계통전류T: %d.%dA\r\n"
			"계통출력: %d.%dkW \r\n"
			"역률: %d, 주파수: %d.%dHz\r\n"
			"누적발전량: %dkWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.pvpfloat, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outrifloat, invData.outsi, invData.outsifloat, invData.outti, invData.outtifloat, invData.outp, invData.outpfloat, invData.pf, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataREFU(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_SOLARV, 0);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_SOLARV) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_PVV);
			invData.pvv = (unsigned short)invValue; // V
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_SOLARI, 0);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_SOLARI) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_PVI);
			invData.pvi = (unsigned short)invValue/10; // A
			invData.pvifloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_DCPOWER, 0);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_DCPOWER) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_PVP);
			invData.pvi = (unsigned short)invValue / 10; // W
			invData.pvifloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_PHASEUACV, 0);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_PHASEUACV) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_GRIDRV);
			invData.outrv = (unsigned short)invValue; // V			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_PHASEUACV, 1);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_PHASEUACV) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_GRIDSV);
			invData.outsv = (unsigned short)invValue; // V			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_PHASEUACV, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_PHASEUACV) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_GRIDTV);
			invData.outtv = (unsigned short)invValue; // V			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_ACTUALACI, 0);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_ACTUALACI) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_GRIDRI);
			invData.outri = (unsigned short)invValue/10; // A
			invData.outrifloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_ACTUALACI, 1);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_ACTUALACI) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_GRIDSI);
			invData.outsi = (unsigned short)invValue / 10; // A
			invData.outsifloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_ACTUALACI, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_ACTUALACI) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_GRIDTI);
			invData.outti = (unsigned short)invValue / 10; // A
			invData.outtifloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_LINEPOWER, 0);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_LINEPOWER) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_GRIDP);
			invData.outp = (unsigned short)invValue; // W			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvREFURequest(buffTX, sizeof(buffTX), invID, REFU_AK_REQUEST_PWEARRAY2, REFU_PARAMETER_TOTALP, 0);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvREFUResponse((unsigned char*)SerialBuff, invID, REFU_PARAMETER_TOTALP) > 0) {
			GetInvREFUValue(&invValue, (unsigned char*)SerialBuff, REFU_VALUE_TOTALP);
			invData.totalP = (unsigned short)invValue; // kWh
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }

		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("REFUSOL 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %dW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %d.%dA, 계통전류S: %d.%dA, 계통전류T: %d.%dA\r\n"
			"계통출력: %dW \r\n"			
			"누적발전량: %dkWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outrifloat, invData.outsi, invData.outsifloat, invData.outti, invData.outtifloat, invData.outp, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataSUNG(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetInvSUNGRequest(buffTX, sizeof(buffTX), invID, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_DCV1, 7);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, invID, SUNG_FUNCTION_CODE_READONLY) > 0) {
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[3], SUNG_VALUE_DCV1);
			invData.pvv = (unsigned short)invValue/10; // V
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[5], SUNG_VALUE_DCI1);
			invData.pvi = (unsigned short)invValue; // A
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[9], SUNG_VALUE_DCI2);
			invData.pvi += (unsigned short)invValue; // A
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[13], SUNG_VALUE_DCI3);
			invData.pvi += (unsigned short)invValue; // A
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvSUNGRequest(buffTX, sizeof(buffTX), invID, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_DCV4, 4);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, invID, SUNG_FUNCTION_CODE_READONLY) > 0) {			
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[5], SUNG_VALUE_DCI4);
			invData.pvi += (unsigned short)invValue; // A
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[9], SUNG_VALUE_DCI5);
			invData.pvi += (unsigned short)invValue; // A
			invData.pvifloat = invData.pvi % 10;
			invData.pvi /= 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvSUNGRequest(buffTX, sizeof(buffTX), invID, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_ABV, 6);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, invID, SUNG_FUNCTION_CODE_READONLY) > 0) {
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[3], SUNG_VALUE_ABV);
			invData.outrv = (unsigned short)invValue/10; // V
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[5], SUNG_VALUE_BCV);
			invData.outsv = (unsigned short)invValue/10; // V
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[7], SUNG_VALUE_CAV);
			invData.outtv = (unsigned short)invValue / 10; // V
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[9], SUNG_VALUE_AI);
			invData.outri = (unsigned short)invValue / 10; // A
			invData.outrifloat = (unsigned short)invValue % 10; // A
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[11], SUNG_VALUE_BI);
			invData.outsi = (unsigned short)invValue / 10; // A
			invData.outsifloat = (unsigned short)invValue % 10; // A
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[13], SUNG_VALUE_CI);
			invData.outti = (unsigned short)invValue / 10; // A
			invData.outtifloat = (unsigned short)invValue % 10; // A
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvSUNGRequest(buffTX, sizeof(buffTX), invID, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_DCP, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, invID, SUNG_FUNCTION_CODE_READONLY) > 0) {
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[3], SUNG_VALUE_DCP);
			invData.pvp = (unsigned short)invValue; // W		
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvSUNGRequest(buffTX, sizeof(buffTX), invID, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_GRIDP, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, invID, SUNG_FUNCTION_CODE_READONLY) > 0) {
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[3], SUNG_VALUE_GRIDP);
			invData.outp = (unsigned short)invValue; // W		
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvSUNGRequest(buffTX, sizeof(buffTX), invID, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_TOTALP, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, invID, SUNG_FUNCTION_CODE_READONLY) > 0) {
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[3], SUNG_VALUE_TOTALP);
			invData.totalP = (unsigned short)invValue; // kWh		
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvSUNGRequest(buffTX, sizeof(buffTX), invID, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_PF, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, invID, SUNG_FUNCTION_CODE_READONLY) > 0) {
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[3], SUNG_VALUE_PF);
			invData.pf = (unsigned short)invValue/10;
			invData.pffloat = (unsigned short)invValue%10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvSUNGRequest(buffTX, sizeof(buffTX), invID, SUNG_FUNCTION_CODE_READONLY, SUNG_ADDR_FR, 2);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyInvSUNGResponse((unsigned char*)SerialBuff, invID, SUNG_FUNCTION_CODE_READONLY) > 0) {
			GetInvSUNGValue(&invValue, (unsigned char*)&SerialBuff[3], SUNG_VALUE_FR);
			invData.fr = (unsigned short)invValue / 10;
			invData.frfloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }

		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("SUNGROW 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %dW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %d.%dA, 계통전류S: %d.%dA, 계통전류T: %d.%dA\r\n"
			"계통출력: %dW \r\n"
			"역률: %d.%d, 주파수: %d.%dHz\r\n"
			"누적발전량: %dkWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outrifloat, invData.outsi, invData.outsifloat, invData.outti, invData.outtifloat, invData.outp, invData.pf, invData.pffloat, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataWILL(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];
	ULONG invValue;
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetInvWILLRequest(buffTX, sizeof(buffTX), invID, (unsigned char*)WILL_CMD_READ, (unsigned char*)WILL_ADDR_PVV, 3);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(800);
		if (flagRX != 0 && VerifyInvWILLResponse((unsigned char*)SerialBuff, invID) > 0) {
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[10], WILL_VALUE_PVV);
			invData.pvv = (unsigned short)invValue; // V
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[14], WILL_VALUE_PVI);
			invData.pvi = (unsigned short)invValue/10; // A
			invData.pvifloat = (unsigned short)invValue % 10;
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[18], WILL_VALUE_PVP);
			invData.pvp = (unsigned short)invValue/10; // kW for three phase
			invData.pvpfloat = (unsigned short)invValue % 10;
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }
		flagRX = 0;
		buffLen = GetInvWILLRequest(buffTX, sizeof(buffTX), invID, (unsigned char*)WILL_CMD_READ, (unsigned char*)WILL_ADDR_GRIDRV, 10);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(800);
		if (flagRX != 0 && VerifyInvWILLResponse((unsigned char*)SerialBuff, invID) > 0) {
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[10], WILL_VALUE_RV);
			invData.outrv = (unsigned short)invValue; // V
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[14], WILL_VALUE_SV);
			invData.outsv = (unsigned short)invValue; // V			
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[18], WILL_VALUE_TV);
			invData.outtv = (unsigned short)invValue; // V
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[22], WILL_VALUE_RI);
			invData.outri = (unsigned short)invValue/10; // A
			invData.outrifloat = (unsigned short)invValue % 10;
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[26], WILL_VALUE_SI);
			invData.outsi = (unsigned short)invValue / 10; // A
			invData.outsifloat = (unsigned short)invValue % 10;
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[30], WILL_VALUE_TI);
			invData.outti = (unsigned short)invValue / 10; // A
			invData.outtifloat = (unsigned short)invValue % 10;
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[34], WILL_VALUE_FR);
			invData.fr = (unsigned short)invValue / 10; // Hz
			invData.frfloat = (unsigned short)invValue % 10;
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[38], WILL_VALUE_PF);
			invData.pf = (unsigned short)invValue / 10; // %
			invData.pffloat = (unsigned short)invValue % 10;
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[42], WILL_VALUE_GRIDP);
			invData.outp = (unsigned short)invValue / 10; // kW
			invData.outpfloat = (unsigned short)invValue % 10;
			GetInvWILLValue(&invValue, (unsigned char*)&SerialBuff[46], WILL_VALUE_TOTALP);
			invData.totalP = (unsigned short)invValue; // Wh			
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }

		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("WILLINGS 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %d.%dA, 태양광발전량: %d.%dkW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %d.%dA, 계통전류S: %d.%dA, 계통전류T: %d.%dA\r\n"
			"계통출력: %d.%dkW \r\n"
			"역률: %d.%d%, 주파수: %d.%dHz\r\n"
			"누적발전량: %dWh")
			, invID, invData.pvv, invData.pvi, invData.pvifloat, invData.pvp, invData.pvpfloat, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outrifloat, invData.outsi, invData.outsifloat, invData.outti, invData.outtifloat, invData.outp, invData.outpfloat, invData.pf, invData.pffloat, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataREMSPV1P(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];	
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetREMSEquipRequest(buffTX, sizeof(buffTX), invID, REMS_CMD_PV_ONE);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyREMSEquipResponse((unsigned char*)SerialBuff, invID, REMS_CMD_PV_ONE) > 0) {			
			invData.pvv = MAKEWORD(SerialBuff[5], SerialBuff[6]); // V
			invData.pvi = MAKEWORD(SerialBuff[7], SerialBuff[8]); // A
			invData.pvp = MAKEWORD(SerialBuff[9], SerialBuff[10]); // W
			invData.outrv = MAKEWORD(SerialBuff[11], SerialBuff[12]); // V
			invData.outri = MAKEWORD(SerialBuff[13], SerialBuff[14]); // A
			invData.outp = MAKEWORD(SerialBuff[15], SerialBuff[16]); // W
			invData.pf = MAKEWORD(SerialBuff[17], SerialBuff[18]); // %
			invData.pffloat = invData.pf % 10;
			invData.pf /= 10;
			invData.fr = MAKEWORD(SerialBuff[19], SerialBuff[20]); // Hz
			invData.frfloat = invData.fr % 10;
			invData.fr /= 10;
			invData.totalP = MAKELONG(MAKEWORD(SerialBuff[25], SerialBuff[26]), MAKEWORD(SerialBuff[27], SerialBuff[28]));
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }		

		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("REMS 단상 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %dA, 태양광발전량: %dW\r\n"
			"계통전압: %dV\r\n"
			"계통전류: %dA\r\n"
			"계통출력: %dW \r\n"
			"역률: %d.%d%, 주파수: %d.%dHz\r\n"
			"누적발전량: %dWh")
			, invID, invData.pvv, invData.pvi, invData.pvp, invData.outrv,
			invData.outri, invData.outp, invData.pf, invData.pffloat, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

INT ReqDataREMSPV3P(UCHAR portNum)
{
	unsigned char buffTX[256];
	INT buffLen;
	TCHAR msg[MAX_PATH];	
	struct tagInvData invData = { 0, };
	BOOL flagPacketVerifyerr = 0;
	if (portNum == 0 || flagReqData != TRUE) return 0;
	for (;;) {
		flagRX = 0;
		buffLen = GetREMSEquipRequest(buffTX, sizeof(buffTX), invID, REMS_CMD_PV_THREE);
		Serial.SerialPort[portNum]->Send((TCHAR*)buffTX, buffLen, msg);
		Sleep(600);
		if (flagRX != 0 && VerifyREMSEquipResponse((unsigned char*)SerialBuff, invID, REMS_CMD_PV_THREE) > 0) {
			invData.pvv = MAKEWORD(SerialBuff[5], SerialBuff[6]); // V
			invData.pvi = MAKEWORD(SerialBuff[7], SerialBuff[8]); // A
			invData.pvp = MAKELONG(MAKEWORD(SerialBuff[9], SerialBuff[10]), MAKEWORD(SerialBuff[11], SerialBuff[12])); // W
			invData.outrv = MAKEWORD(SerialBuff[13], SerialBuff[14]); // V
			invData.outsv = MAKEWORD(SerialBuff[15], SerialBuff[16]); // V
			invData.outtv = MAKEWORD(SerialBuff[17], SerialBuff[18]); // V
			invData.outri = MAKEWORD(SerialBuff[19], SerialBuff[20]); // A
			invData.outsi = MAKEWORD(SerialBuff[21], SerialBuff[22]); // A
			invData.outti = MAKEWORD(SerialBuff[23], SerialBuff[24]); // A
			invData.outp = MAKELONG(MAKEWORD(SerialBuff[25], SerialBuff[26]), MAKEWORD(SerialBuff[27], SerialBuff[28])); // W
			invData.pf = MAKEWORD(SerialBuff[29], SerialBuff[30]); // %
			invData.pffloat = invData.pf % 10;
			invData.pf /= 10;
			invData.fr = MAKEWORD(SerialBuff[31], SerialBuff[32]); // Hz
			invData.frfloat = invData.fr % 10;
			invData.fr /= 10;
			invData.totalP = MAKELONG(MAKEWORD(SerialBuff[37], SerialBuff[38]), MAKEWORD(SerialBuff[39], SerialBuff[40]));
		}
		else if (flagRX == 0) break;
		else { flagPacketVerifyerr = 1; break; }

		break;
	}
	if (flagRX == 0) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 없음!!"));
	else if (flagPacketVerifyerr == 1) SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, TEXT("응답 패킷이 형식에 맞지 않습니다!"));
	else {
		wsprintf(msg, TEXT("REMS 삼상 인버터 %d번 데이터\r\n"
			"태양광전압: %dV, 태양광전류: %dA, 태양광발전량: %dW\r\n"
			"계통전압R: %dV, 계통전압S: %dV, 계통전압T: %dV\r\n"
			"계통전류R: %dA, 계통전류S: %dA, 계통전류T: %dA\r\n"
			"계통출력: %dW \r\n"
			"역률: %d.%d%, 주파수: %d.%dHz\r\n"
			"누적발전량: %dWh")
			, invID, invData.pvv, invData.pvi, invData.pvp, invData.outrv, invData.outsv, invData.outtv,
			invData.outri, invData.outsi, invData.outti, invData.outp, invData.pf, invData.pffloat, invData.fr, invData.frfloat, invData.totalP);
		SetDlgItemText(hDlgCurrent, IDC_EDIT_RESULT_INVDATA, msg);
	}
	return 1;
}

BOOL HtoA2(TCHAR value, TCHAR *retHexHigh, TCHAR *retHexLow)
{
	*retHexHigh = (value >> 4) & 0x0F;
	*retHexLow = value & 0x0F;
	if (*retHexHigh > 9) *retHexHigh += 0x37;
	else *retHexHigh += 0x30;
	if (*retHexLow > 9) *retHexLow += 0x37;
	else *retHexLow += 0x30;
	return TRUE;
}

INT AtoHstr(TCHAR *dest, TCHAR *src, INT dataCount)
{
	INT cnt;
	INT cnt1 = 0;
	TCHAR oneChar[4];
	for (cnt = 0; cnt<dataCount; cnt++) {
		if (cnt > 1023) return -1;
		//sprintf_s(oneChar, TEXT("%02X "), *(src + cnt));
		HtoA2(*(src + cnt), &oneChar[0], &oneChar[1]);
		*(dest + cnt1) = oneChar[0];
		*(dest + cnt1 + 1) = oneChar[1];
		*(dest + cnt1 + 2) = ' ';
		cnt1 += 3;
	}
	*(dest + cnt1) = NULL;
	return cnt1;
}