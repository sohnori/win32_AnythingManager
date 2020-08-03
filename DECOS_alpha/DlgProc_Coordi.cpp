#include "DlgProc_Coordi.h"
#include <CommCtrl.h>
#include "CoordinatorReal.h"
#include "SerialComm.h"
#include "SerialCommHandler.h"
#include "MyDevice.h"
#include "MyDeviceHandler.h"
#include "MyFunc.h"
#include "resource.h"

extern HINSTANCE g_hInst;
extern CoordinatorReal *CoordiR;
extern HWND hCRT;
extern HWND hCRB;
extern HWND hCoordi;
extern HWND hList;
extern MyDeviceHandler MyDev;
extern SerialCommHandler MySerial;

extern TCHAR rxBuff[1024];
extern DWORD dwBytesRead;
extern TCHAR txBuff[1024];
extern INT dwBytesWrite;
extern INT indexDevice;

BOOL CALLBACK CoordiDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	TCHAR str[MAX_PATH] = "";
	TCHAR retMes[MAX_PATH] = "";
	TCHAR *ptrStr;
	TCHAR valueInt[6];
	INT indexPort;
	INT i;
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH MyBrush, OldBrush;
	HPEN MyPen, OldPen;
	LVCOLUMN col;
	LVITEM litem;
	//RECT rt;
	//RECT crt;	

	switch (iMessage) {
	case WM_INITDIALOG:
		CoordiR = new CoordinatorReal;
		i = IDC_COMBO_INV0_TYPE;
		for (; i != IDC_COMBO_INVf_TYPE + 1; i++) {
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"    ");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"DASS");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"E&P3");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"E&P5");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"HANS");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"HEXP");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"EKOS");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"WILL");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"ABBI");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"REFU");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"SUNG");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"REMS");
			SendMessage(GetDlgItem(hDlg, i), CB_SETCURSEL, 0, 0);
		}
		i = IDC_COMBO_INV0_PHASE;
		for (; i != IDC_COMBO_INVF_PHASE + 1; i++) {
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)" ");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"1");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"3");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"O");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"T");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"F");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"M");
			SendMessage(GetDlgItem(hDlg, i), CB_ADDSTRING, 0, (LPARAM)"S");
			SendMessage(GetDlgItem(hDlg, i), CB_SETCURSEL, 0, 0);
		}
		CheckDlgButton(hDlg, IDC_RADIO_INMD00, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_RADIO_ZBMD00, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_RADIO_PVEM00, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_RADIO_LOMM00, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_RADIO_MSCH1, BST_CHECKED);
		ListView_SetExtendedListViewStyle(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		col.fmt = LVCFMT_LEFT;
		col.cx = 70;
		col.pszText = (LPSTR)TEXT("모듈센서ID");
		col.iSubItem = 0;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 0, (LPARAM)&col);
		col.pszText = (LPSTR) "Array";
		col.cx = 50;
		col.iSubItem = 1;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 1, (LPARAM)&col);
		col.pszText = (LPSTR) "X";
		col.cx = 30;
		col.iSubItem = 2;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 2, (LPARAM)&col);
		col.pszText = (LPSTR) "Y";
		col.cx = 30;
		col.iSubItem = 3;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 3, (LPARAM)&col);
		col.pszText = (LPSTR) "DCPATH";
		col.cx = 70;
		col.iSubItem = 4;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 4, (LPARAM)&col);
		col.pszText = (LPSTR) "시리얼번호";
		col.cx = 120;
		col.iSubItem = 5;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 5, (LPARAM)&col);
		col.pszText = (LPSTR) "중계기ID";
		col.cx = 70;
		col.iSubItem = 6;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 6, (LPARAM)&col);
		col.pszText = (LPSTR) "INDEX";
		col.cx = 55;
		col.iSubItem = 7;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 7, (LPARAM)&col);
		col.pszText = (LPSTR) "STATUS";
		col.cx = 130;
		col.iSubItem = 8;
		SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), LVM_INSERTCOLUMN, 8, (LPARAM)&col);
		return TRUE;
	case WM_MOUSEWHEEL:
		i = (INT)wParam;
		if (i > 0) {
			if (LOWORD(wParam) && MK_SHIFT) SendMessage(hCRT, WM_HSCROLL, (WPARAM)LOWORD(SB_LINELEFT), 0);
			else SendMessage(hCRT, WM_VSCROLL, (WPARAM)LOWORD(SB_LINEUP), 0);
		}
		else if (i < 0) {
			if (LOWORD(wParam) && MK_SHIFT) SendMessage(hCRT, WM_HSCROLL, (WPARAM)LOWORD(SB_LINERIGHT), 0);
			else SendMessage(hCRT, WM_VSCROLL, (WPARAM)LOWORD(SB_LINEDOWN), 0);
		}
		return TRUE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		LPNMLISTVIEW nlv;
		LPNMITEMACTIVATE nia;
		hdr = (LPNMHDR)lParam;
		nlv = (LPNMLISTVIEW)lParam;
		if (hdr->hwndFrom == GetDlgItem(hDlg, IDC_LIST_TEXTFILE)) {
			switch (hdr->code) {
			case LVN_ITEMCHANGED:
				break;
			case LVN_ITEMCHANGING:
				break;
			case NM_CLICK:
				nia = (LPNMITEMACTIVATE)lParam;
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), nia->iItem, 0, str, sizeof(str));
				wsprintf(valueInt, "%d", nia->iItem + 1);
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SRN, valueInt);
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SID, str);
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), nia->iItem, 1, str, sizeof(str));
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_A_SID, str);
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), nia->iItem, 2, str, sizeof(str));
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_X_SPOS, str);
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), nia->iItem, 3, str, sizeof(str));
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_Y_SPOS, str);
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), nia->iItem, 4, str, sizeof(str));
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SDCPATH, str);
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), nia->iItem, 5, str, sizeof(str));
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SSN, str);
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), nia->iItem, 6, str, sizeof(str));
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SCID, str);
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), nia->iItem, 7, str, sizeof(str));
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SINDEX, str);
				break;
			}
		}
		return TRUE;
	case WM_COMMAND:
		i = SendMessage(hList, LB_GETCURSEL, 0, 0);
		i = MyDev.GetDeviceNum(i);
		ptrStr = MyDev.MyDev[i]->GetDeviceTerminalName();
		if (MySerial.IsConnectedPort(ptrStr) != TRUE) return TRUE;
		switch (LOWORD(wParam)) {		
		case IDC_BUTTON_STOP2:
			CoordiR->ResetFlag();
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		case IDC_BUTTON_READALL2:
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_MDXY, 0);
			CoordiR->SetMSIFlagModuleNum(1);
			i = GetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_CNT, NULL, FALSE);
			CoordiR->SetMSIModuleCount(i);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			ListView_DeleteAllItems(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE));
			break;
		case IDC_BUTTON_WRITEALL2:
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_MDXY, 0);
			CoordiR->SetMSIFlagModuleNum(1);
			i = GetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_CNT, NULL, FALSE);
			CoordiR->SetMSIModuleCount(i);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(WRITE_COORDI);
			break;
		case IDC_BUTTON_MSENSOR_SREAD:
			CoordiR->SetPortName(ptrStr);
			GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SRN, retMes, sizeof(retMes));
			i = atoi(retMes);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_MDXY, 1);
			CoordiR->SetMSIFlagModuleNum(i);
			CoordiR->SetMSIModuleCount(1);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			break;
		case IDC_BUTTON_MSENSOR_SWRITE:
			CoordiR->SetPortName(ptrStr);
			GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SRN, retMes, sizeof(retMes));
			i = atoi(retMes);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_MDXY, 1);
			CoordiR->SetMSIFlagModuleNum(i);
			CoordiR->SetMSIModuleCount(1);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(WRITE_COORDI);
			break;
		case IDC_BUTTON_MSENSOR_READADC:
			CoordiR->SetPortName(ptrStr);			
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_ZB_IS, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			break;
		case IDC_BUTTON_INSERTFILE:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_INSERTFILE), hDlg, InsertFileDlgProc);
			break;
		case IDC_COMBO_INV0_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV0_TYPE, IDC_COMBO_INV0_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV1_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV1_TYPE, IDC_COMBO_INV1_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV2_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV2_TYPE, IDC_COMBO_INV2_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV3_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV3_TYPE, IDC_COMBO_INV3_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV4_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV4_TYPE, IDC_COMBO_INV4_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV5_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV5_TYPE, IDC_COMBO_INV5_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV6_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV6_TYPE, IDC_COMBO_INV6_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV7_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV7_TYPE, IDC_COMBO_INV7_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV8_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV8_TYPE, IDC_COMBO_INV8_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INV9_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INV9_TYPE, IDC_COMBO_INV9_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INVA_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INVA_TYPE, IDC_COMBO_INVA_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INVB_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INVB_TYPE, IDC_COMBO_INVB_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INVC_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INVC_TYPE, IDC_COMBO_INVC_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INVD_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INVD_TYPE, IDC_COMBO_INVD_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INVE_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INVE_TYPE, IDC_COMBO_INVE_PHASE);
				break;
			}
			break;
		case IDC_COMBO_INVf_TYPE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SetInvPhaseCombobox(hDlg, IDC_COMBO_INVf_TYPE, IDC_COMBO_INVF_PHASE);
				break;
			}
			break;
		case IDC_BUTTON_STOP1:
			CoordiR->ResetFlag();
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		case IDC_BUTTON_READALL:
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_C1ES, 0);
			CoordiR->SetFlag(CMD_C1CD, 0);
			CoordiR->SetFlag(CMD_INEH, 0);
			CoordiR->SetFlag(CMD_INEL, 0);
			CoordiR->SetFlag(CMD_INVX, 0);
			CoordiR->SetFlag(CMD_INMD, 0);
			CoordiR->SetFlag(CMD_ZBMD, 0);
			CoordiR->SetFlag(CMD_PVEM, 0);
			CoordiR->SetFlag(CMD_LOMM, 0);
			CoordiR->SetFlag(CMD_INFO, 0);
			CoordiR->SetFlag(CMD_BVER, 0);
			CoordiR->SetFlag(CMD_DBUG, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			break;
		case IDC_BUTTON_WRITEALL:
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_C1ES, 0);
			CoordiR->SetFlag(CMD_C1CD, 0);
			CoordiR->SetFlag(CMD_INEH, 0);
			CoordiR->SetFlag(CMD_INEL, 0);
			CoordiR->SetFlag(CMD_INVX, 0);
			for (i = 0; i < 16; i++) {
				GetDlgItemText(hDlg, IDC_COMBO_INV0_TYPE + i, valueInt, 5);
				strncpy_s(retMes, valueInt, 5);
				GetDlgItemText(hDlg, IDC_EDIT_INV_ID0 + i, valueInt, 4);
				strncat_s(retMes, valueInt, 3);
				GetDlgItemText(hDlg, IDC_COMBO_INV0_PHASE + i, valueInt, 2);
				strncat_s(retMes, valueInt, 1);
				GetDlgItemText(hDlg, IDC_EDIT_INV0_POWER + i, valueInt, 4);
				strncat_s(retMes, valueInt, 3);
				CoordiR->SetInvData(retMes, i);
			}
			CoordiR->SetFlag(CMD_INMD, 0);
			CoordiR->SetFlag(CMD_ZBMD, 0);
			CoordiR->SetFlag(CMD_PVEM, 0);
			CoordiR->SetFlag(CMD_LOMM, 0);
			CoordiR->SetFlag(CMD_INFO, 0);
			CoordiR->SetFlag(CMD_BVER, 0);
			CoordiR->SetFlag(CMD_DBUG, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(WRITE_COORDI);
			break;
		case IDC_BUTTON_DEFAULT: // 값 초기화 버튼			
			CoordiR->SetPortName(ptrStr);
			SetDlgItemInt(hDlg, IDC_EDIT_COUNTRY_N, 82, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_AREA_N, 64, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_TE_N, 731, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_PHONE_N, 8446, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_INVERTER_C, 1, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_COORDI_C, 1, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_ARRAY_C, 1, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_C, 100, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_DCPATH, 1, NULL);
			SetDlgItemInt(hDlg, IDC_EDIT_COORDI_ID, 1, NULL);
			strcpy_s(str, "0013A20040000000");
			SetDlgItemText(hDlg, IDC_EDIT_COORDI_SN, str);
			CheckDlgButton(hDlg, IDC_CHECK_INV0, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV1, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV2, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV3, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV4, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV5, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV6, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV7, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV8, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INV9, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INVA, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INVB, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INVC, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INVE, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_INVF, BST_UNCHECKED);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV0_TYPE, CB_SETCURSEL, 1, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV1_TYPE, CB_SETCURSEL, 1, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV2_TYPE, CB_SETCURSEL, 1, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV3_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV4_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV5_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV6_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV7_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV8_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV9_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVA_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVB_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVC_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVD_TYPE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVf_TYPE, CB_SETCURSEL, 0, 0);
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID0, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID1, "001");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID2, "002");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID3, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID4, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID5, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID6, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID7, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID8, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID9, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID10, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID11, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID12, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID13, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID14, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV_ID15, "000");
			SendDlgItemMessage(hDlg, IDC_COMBO_INV0_PHASE, CB_SETCURSEL, 1, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV1_PHASE, CB_SETCURSEL, 1, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV2_PHASE, CB_SETCURSEL, 1, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV3_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV4_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV5_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV6_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV7_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV8_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INV9_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVA_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVB_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVC_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVD_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVE_PHASE, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_INVF_PHASE, CB_SETCURSEL, 0, 0);
			SetDlgItemText(hDlg, IDC_EDIT_INV0_POWER, "003");
			SetDlgItemText(hDlg, IDC_EDIT_INV1_POWER, "003");
			SetDlgItemText(hDlg, IDC_EDIT_INV2_POWER, "003");
			SetDlgItemText(hDlg, IDC_EDIT_INV3_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV4_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV5_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV6_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV7_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV8_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INV9_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INVA_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INVB_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INVC_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INVD_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INVE_POWER, "000");
			SetDlgItemText(hDlg, IDC_EDIT_INVF_POWER, "000");
			CheckRadioButton(hDlg, IDC_RADIO_INMD00, IDC_RADIO_INMD21, IDC_RADIO_INMD00);
			CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD22, IDC_RADIO_ZBMD00);
			CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM00);
			CheckRadioButton(hDlg, IDC_RADIO_LOMM00, IDC_RADIO_LOMM03, IDC_RADIO_LOMM00);
			SendDlgItemMessage(hDlg, IDC_EDIT_FIRMWARE_V, EM_SETSEL, 0, -1);
			SendDlgItemMessage(hDlg, IDC_EDIT_FIRMWARE_V, WM_CLEAR, 0, 0);
			SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_V, "Rev.0H");
			CheckDlgButton(hDlg, IDC_CHECK_DEBUG_MODE, BST_UNCHECKED);
			break;
		case IDC_BUTTON_EPSD_R: // 발전소 정보 읽기 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_C1ES, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			break;
		case IDC_BUTTON_EPSD_W: // 발전소 정보 쓰기 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_C1ES, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(WRITE_COORDI);
			break;
		case IDC_BUTTON_COORDI_DR: // 중계기 정보 읽기 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_C1CD, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			break;
		case IDC_BUTTON_COORDI_DW: // 중계기 정보 쓰기 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_C1CD, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(WRITE_COORDI);
			break;
		case IDC_BUTTON_INV_READ: // 인버터 정보 읽기 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_INVX, 0);
			CoordiR->SetFlag(CMD_INEH, 0);
			CoordiR->SetFlag(CMD_INEL, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			break;
		case IDC_BUTTON_INV_WRITE: // 인버터 정보 쓰기 버튼			
			CoordiR->SetPortName(ptrStr);
			for (i = 0; i < 16; i++) {
				GetDlgItemText(hDlg, IDC_COMBO_INV0_TYPE + i, valueInt, 5);
				strncpy_s(retMes, valueInt, 5);
				GetDlgItemText(hDlg, IDC_EDIT_INV_ID0 + i, valueInt, 4);
				strncat_s(retMes, valueInt, 3);
				GetDlgItemText(hDlg, IDC_COMBO_INV0_PHASE + i, valueInt, 2);
				strncat_s(retMes, valueInt, 1);
				GetDlgItemText(hDlg, IDC_EDIT_INV0_POWER + i, valueInt, 4);
				strncat_s(retMes, valueInt, 3);
				CoordiR->SetInvData(retMes, i);
			}
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_INVX, 0);
			CoordiR->SetFlag(CMD_INEH, 0);
			CoordiR->SetFlag(CMD_INEL, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(WRITE_COORDI);
			break;
		case IDC_BUTTON_CDMD_READ: // 중계기 모드 읽기 버튼		
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_INMD, 0);
			CoordiR->SetFlag(CMD_ZBMD, 0);
			CoordiR->SetFlag(CMD_PVEM, 0);
			CoordiR->SetFlag(CMD_LOMM, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			break;
		case IDC_BUTTON_CDMD_WRITE: // 중계기 모드 쓰기 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_INMD, 0);
			CoordiR->SetFlag(CMD_ZBMD, 0);
			CoordiR->SetFlag(CMD_PVEM, 0);
			CoordiR->SetFlag(CMD_LOMM, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(WRITE_COORDI);
			break;
		case IDC_BUTTON_ETC_READ: // 기타 정보 읽기 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_INFO, 0);
			CoordiR->SetFlag(CMD_BVER, 0);
			CoordiR->SetFlag(CMD_DBUG, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(READ_COORDI);
			break;
		case IDC_BUTTON_ETC_WRITE: // 기타 정보 쓰기 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_BVER, 0);
			CoordiR->SetFlag(CMD_DBUG, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			CoordiR->SetMode(WRITE_COORDI);
			break;
		case IDC_BUTTON_ETCCMD_CH0S: // 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_CH0S, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		case IDC_BUTTON_ETCCMD_CH0R: // 버튼
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_CH0R, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		case IDC_BUTTON_ETCCMD_CH1S: // 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_CH1S, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		case IDC_BUTTON_ETCCMD_CH1R: // 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_CH1R, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		case IDC_BUTTON_ETCCMD_TIME: // 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_TIME, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		case IDC_BUTTON_ETCCMD_IDSC: // 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_IDSC, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		case IDC_BUTTON_ETCCMD_TXPK: // 버튼			
			CoordiR->SetPortName(ptrStr);
			CoordiR->SetFlag(CMD_ENTER, 0);
			CoordiR->SetFlag(CMD_TXPK, 0);
			CoordiR->SetFlag(CMD_EXIT, 0);
			break;
		}
		return TRUE;
	case WM_USER + 1: // receive - 타이머1 루프_셋팅된 플래그 명령을 보낸 후 응답확인
		switch (LOWORD(wParam)) {
		case CMD_ENTER: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->VerifyOK(rxBuff) == TRUE) SetDlgItemText(hDlg, IDC_STATIC_EEPROM_STATUS, "CMD mode in~~");
			else SetDlgItemText(hDlg, IDC_STATIC_EEPROM_STATUS, "CMD mode fail");
			break;
		case CMD_C1ES: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_EPSD_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				strncpy_s(retMes, str, 2);
				AsciiHextoHarr(valueInt, retMes, 2);
				SetDlgItemInt(hDlg, IDC_EDIT_COUNTRY_N, (UCHAR)valueInt[0], FALSE);
				strncpy_s(retMes, &str[2], 2);
				AsciiHextoHarr(valueInt, retMes, 2);
				SetDlgItemInt(hDlg, IDC_EDIT_AREA_N, (UCHAR)valueInt[0], FALSE);
				strncpy_s(retMes, &str[4], 4);
				AsciiHextoHarr(valueInt, retMes, 4);
				i = (UCHAR)valueInt[0];
				i <<= 8;
				i |= (UCHAR)valueInt[1];
				SetDlgItemInt(hDlg, IDC_EDIT_TE_N, i, FALSE);
				strncpy_s(retMes, &str[8], 4);
				AsciiHextoHarr(valueInt, retMes, 4);
				i = (UCHAR)valueInt[0];
				i <<= 8;
				i |= (UCHAR)valueInt[1];
				SetDlgItemInt(hDlg, IDC_EDIT_PHONE_N, i, FALSE);
				strncpy_s(retMes, &str[12], 2);
				AsciiHextoHarr(valueInt, retMes, 2);
				SetDlgItemInt(hDlg, IDC_EDIT_INVERTER_C, (UCHAR)valueInt[0], FALSE);
				strncpy_s(retMes, &str[14], 2);
				AsciiHextoHarr(valueInt, retMes, 2);
				SetDlgItemInt(hDlg, IDC_EDIT_COORDI_C, (UCHAR)valueInt[0], FALSE);
				strncpy_s(retMes, &str[16], 2);
				AsciiHextoHarr(valueInt, retMes, 2);
				SetDlgItemInt(hDlg, IDC_EDIT_ARRAY_C, (UCHAR)valueInt[0], FALSE);
				strncpy_s(retMes, &str[18], 4);
				AsciiHextoHarr(valueInt, retMes, 4);
				i = (UCHAR)valueInt[0];
				i <<= 8;
				i |= (UCHAR)valueInt[1];
				SetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_C, i, FALSE);
				strncpy_s(retMes, &str[22], 2);
				AsciiHextoHarr(valueInt, retMes, 2);
				SetDlgItemInt(hDlg, IDC_EDIT_DCPATH, (UCHAR)valueInt[0], FALSE);
				SetDlgItemText(hDlg, IDC_STATIC_EPSD_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_EPSD_STATUS, "Fail~!");
			break;
		case CMD_C1CD: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_COORDI_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				strncpy_s(retMes, str, 2);
				AsciiHextoHarr(valueInt, retMes, 2);
				SetDlgItemInt(hDlg, IDC_EDIT_COORDI_ID, (UCHAR)valueInt[0], FALSE);
				strncpy_s(retMes, &str[2], 16);
				SetDlgItemText(hDlg, IDC_EDIT_COORDI_SN, retMes);
				SetDlgItemText(hDlg, IDC_STATIC_COORDI_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_COORDI_STATUS, "Fail~!");
			break;
		case CMD_MDXY:
			i = CoordiR->GetMSIFlagModuleNum() - 1;
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE) {
				if (CoordiR->GetMSIFlagSingle()) {
					SetDlgItemText(hDlg, IDC_STATIC_MSENSOR_SSTATUS, "          ");
					SetDlgItemText(hDlg, IDC_STATIC_MSENSOR_SSTATUS, "WriteOK");
				}
				else {
					litem.mask = LVIF_TEXT;
					litem.iItem = i - 1;
					litem.iSubItem = 8;
					litem.pszText = (LPSTR)"WriteOK~";
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
				}
			}
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0) {
				if (CoordiR->GetMSIFlagSingle()) {
					CopyData(str, rxBuff, dwBytesRead);
					strncpy_s(retMes, str, 4);
					AsciiHextoHarr(valueInt, retMes, 4);
					indexPort = (UCHAR)valueInt[0];
					indexPort <<= 8;
					indexPort |= (UCHAR)valueInt[1];
					SetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_SID, i, FALSE);
					strncpy_s(retMes, &str[4], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					SetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_A_SID, (UCHAR)valueInt[0], FALSE);
					strncpy_s(retMes, &str[6], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					SetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_X_SPOS, (UCHAR)valueInt[0], FALSE);
					strncpy_s(retMes, &str[8], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					SetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_Y_SPOS, (UCHAR)valueInt[0], FALSE);
					strncpy_s(retMes, &str[10], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					SetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_SDCPATH, (UCHAR)valueInt[0], FALSE);
					strncpy_s(retMes, &str[12], 16);
					SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SSN, retMes);
					strncpy_s(retMes, &str[28], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					SetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_SCID, (UCHAR)valueInt[0], FALSE);
					strncpy_s(retMes, &str[30], 2);					
					SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SINDEX, retMes);					
					SetDlgItemText(hDlg, IDC_STATIC_MSENSOR_SSTATUS, "          ");
					SetDlgItemText(hDlg, IDC_STATIC_MSENSOR_SSTATUS, "ReadOK");
				}
				else {
					CopyData(str, rxBuff, dwBytesRead);
					strncpy_s(retMes, str, 4);
					AsciiHextoHarr(valueInt, retMes, 4);
					indexPort = (UCHAR)valueInt[0];
					indexPort <<= 8;
					indexPort |= (UCHAR)valueInt[1];
					//wsprintf(retMes, "%d", indexPort);
					_itoa_s(indexPort, retMes, 10);
					litem.mask = LVIF_TEXT;
					litem.iItem = i - 1;
					litem.iSubItem = 0;
					litem.pszText = retMes;
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_INSERTITEM, 0, (LPARAM)&litem);
					strncpy_s(retMes, &str[4], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					//wsprintf(retMes, "%d", (UCHAR)valueInt[0]);
					_itoa_s((UCHAR)valueInt[0], retMes, 10);
					litem.iSubItem = 1;
					litem.pszText = retMes;
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
					strncpy_s(retMes, &str[6], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					//wsprintf(retMes, "%d", (UCHAR)valueInt[0]);
					_itoa_s((UCHAR)valueInt[0], retMes, 10);
					litem.iSubItem = 2;
					litem.pszText = retMes;
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
					strncpy_s(retMes, &str[8], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					//wsprintf(retMes, "%d", (UCHAR)valueInt[0]);
					_itoa_s((UCHAR)valueInt[0], retMes, 10);
					litem.iSubItem = 3;
					litem.pszText = retMes;
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
					strncpy_s(retMes, &str[10], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					//wsprintf(retMes, "%d", (UCHAR)valueInt[0]);
					_itoa_s((UCHAR)valueInt[0], retMes, 10);
					litem.iSubItem = 4;
					litem.pszText = retMes;
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
					strncpy_s(retMes, &str[12], 16);
					litem.iSubItem = 5;
					litem.pszText = retMes;
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
					strncpy_s(retMes, &str[28], 2);
					AsciiHextoHarr(valueInt, retMes, 2);
					//wsprintf(retMes, "%d", (UCHAR)valueInt[0]);
					_itoa_s((UCHAR)valueInt[0], retMes, 10);
					litem.iSubItem = 6;
					litem.pszText = retMes;
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
					strncpy_s(retMes, &str[30], 2);					
					litem.iSubItem = 7;
					litem.pszText = retMes;
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
					litem.iSubItem = 8;
					litem.pszText = (LPSTR)"ReadOK~";
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
					//ListView_SetTextBkColor(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), RGB(255, 0, 255));
				}
			}
			else {
				if (CoordiR->GetMSIFlagSingle()) {
					SetDlgItemText(hDlg, IDC_STATIC_MSENSOR_SSTATUS, "          ");
					SetDlgItemText(hDlg, IDC_STATIC_MSENSOR_SSTATUS, "Failed");
				}
				else {
					litem.mask = LVIF_TEXT;
					litem.iItem = i - 1;
					litem.iSubItem = 8;
					litem.pszText = (LPSTR)"Failed~";
					SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_INSERTITEM, 0, (LPARAM)&litem);
				}
			}
			break;
		case CMD_INVX: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0) {
				i = CoordiR->GetFlagInvNum();
				i -= 1;
				if (i == -1) i = 15;
				CopyData(str, rxBuff, dwBytesRead);
				strncpy_s(retMes, str, 4);
				indexPort = SendMessage(GetDlgItem(hDlg, IDC_COMBO_INV0_TYPE + i), CB_SELECTSTRING, 0, (WPARAM)retMes);
				if (indexPort == -1) SendMessage(GetDlgItem(hDlg, IDC_COMBO_INV0_TYPE + i), CB_SETCURSEL, 0, 0);
				strncpy_s(retMes, &str[4], 3);
				SetDlgItemText(hDlg, IDC_EDIT_INV_ID0 + i, retMes);
				strncpy_s(retMes, &str[7], 1);
				indexPort = SendMessage(GetDlgItem(hDlg, IDC_COMBO_INV0_PHASE + i), CB_SELECTSTRING, 0, (WPARAM)retMes);
				if (indexPort == -1) SendMessage(GetDlgItem(hDlg, IDC_COMBO_INV0_PHASE + i), CB_SETCURSEL, 0, 0);
				strncpy_s(retMes, &str[8], 3);
				SetDlgItemText(hDlg, IDC_EDIT_INV0_POWER + i, retMes);
				SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "Fail~!");
			break;
		case CMD_INEH: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				i = (INT)A2CtoHex(str[0], str[1]);
				if (i & 0b10000000) CheckDlgButton(hDlg, IDC_CHECK_INVF, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INVF, BST_UNCHECKED);
				if (i & 0b01000000) CheckDlgButton(hDlg, IDC_CHECK_INVE, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INVE, BST_UNCHECKED);
				if (i & 0b00100000) CheckDlgButton(hDlg, IDC_CHECK_INVD, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INVD, BST_UNCHECKED);
				if (i & 0b00010000) CheckDlgButton(hDlg, IDC_CHECK_INVC, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INVC, BST_UNCHECKED);
				if (i & 0b00001000) CheckDlgButton(hDlg, IDC_CHECK_INVB, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INVB, BST_UNCHECKED);
				if (i & 0b00000100) CheckDlgButton(hDlg, IDC_CHECK_INVA, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INVA, BST_UNCHECKED);
				if (i & 0b00000010) CheckDlgButton(hDlg, IDC_CHECK_INV9, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV9, BST_UNCHECKED);
				if (i & 0b00000001) CheckDlgButton(hDlg, IDC_CHECK_INV8, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV8, BST_UNCHECKED);
				SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "Fail~!");
			break;
		case CMD_INEL: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				i = (INT)A2CtoHex(str[0], str[1]);
				if (i & 0b10000000) CheckDlgButton(hDlg, IDC_CHECK_INV7, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV7, BST_UNCHECKED);
				if (i & 0b01000000) CheckDlgButton(hDlg, IDC_CHECK_INV6, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV6, BST_UNCHECKED);
				if (i & 0b00100000) CheckDlgButton(hDlg, IDC_CHECK_INV5, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV5, BST_UNCHECKED);
				if (i & 0b00010000) CheckDlgButton(hDlg, IDC_CHECK_INV4, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV4, BST_UNCHECKED);
				if (i & 0b00001000) CheckDlgButton(hDlg, IDC_CHECK_INV3, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV3, BST_UNCHECKED);
				if (i & 0b00000100) CheckDlgButton(hDlg, IDC_CHECK_INV2, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV2, BST_UNCHECKED);
				if (i & 0b00000010) CheckDlgButton(hDlg, IDC_CHECK_INV1, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV1, BST_UNCHECKED);
				if (i & 0b00000001) CheckDlgButton(hDlg, IDC_CHECK_INV0, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_INV0, BST_UNCHECKED);
				SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_INVERTER_STATUS, "Fail~!");
			break;
		case CMD_INMD: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				i = (INT)A2CtoHex(str[0], str[1]);
				if (i == 0x01) CheckRadioButton(hDlg, IDC_RADIO_INMD00, IDC_RADIO_INMD21, IDC_RADIO_INMD01);
				else if (i == 0x11) CheckRadioButton(hDlg, IDC_RADIO_INMD00, IDC_RADIO_INMD21, IDC_RADIO_INMD11);
				else if (i == 0x21) CheckRadioButton(hDlg, IDC_RADIO_INMD00, IDC_RADIO_INMD21, IDC_RADIO_INMD21);
				else CheckRadioButton(hDlg, IDC_RADIO_INMD00, IDC_RADIO_INMD21, IDC_RADIO_INMD00);
				SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "Fail~!");
			break;
		case CMD_ZBMD: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				i = (INT)A2CtoHex(str[0], str[1]);
				if (i == 0x01) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD01);
				else if (i == 0x02) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD02);
				else if (i == 0x04) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD04);
				else if (i == 0x11) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD11);
				else if (i == 0x12) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD12);
				else if (i == 0x14) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD14);
				else if (i == 0x21) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD21);
				else if (i == 0x22) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD22);
				else if (i == 0x24) CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD24);
				else CheckRadioButton(hDlg, IDC_RADIO_ZBMD00, IDC_RADIO_ZBMD24, IDC_RADIO_ZBMD00);
				SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "Fail~!");
			break;
		case CMD_PVEM: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				i = (INT)A2CtoHex(str[0], str[1]);
				if (i == 0x01) CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM01);
				else if (i == 0x02) CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM02);
				else if (i == 0x03) CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM03);
				else if (i == 0x11) CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM11);
				else if (i == 0x12) CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM12);
				else if (i == 0x21) CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM21);
				else if (i == 0x22) CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM22);
				else CheckRadioButton(hDlg, IDC_RADIO_PVEM00, IDC_RADIO_PVEM22, IDC_RADIO_PVEM00);
				SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "Fail~!");
			break;
		case CMD_LOMM: // 해당 명령 보낸 후 수신된 데이터 확인
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				i = (INT)A2CtoHex(str[0], str[1]);
				if (i == 0x01) CheckRadioButton(hDlg, IDC_RADIO_LOMM00, IDC_RADIO_LOMM03, IDC_RADIO_LOMM01);
				else if (i == 0x02) CheckRadioButton(hDlg, IDC_RADIO_LOMM00, IDC_RADIO_LOMM03, IDC_RADIO_LOMM02);
				else if (i == 0x03) CheckRadioButton(hDlg, IDC_RADIO_LOMM00, IDC_RADIO_LOMM03, IDC_RADIO_LOMM03);
				else CheckRadioButton(hDlg, IDC_RADIO_LOMM00, IDC_RADIO_LOMM03, IDC_RADIO_LOMM00);
				SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_CDMD_STATUS, "Fail~!");
			break;
		case CMD_INFO: // 해당 명령 보낸 후 수신된 데이터 확인			
			if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				str[6] = NULL;
				SetDlgItemText(hDlg, IDC_EDIT_FIRMWARE_V, str);
				SetDlgItemText(hDlg, IDC_STATIC_ETC_STATUS, "ROK~!");
			}
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead == 0) SetDlgItemText(hDlg, IDC_STATIC_ETC_STATUS, "Fail~!");
			break;
		case CMD_BVER:
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_ETC_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				str[6] = NULL;
				SetDlgItemText(hDlg, IDC_EDIT_MSENSOR_V, str);
				SetDlgItemText(hDlg, IDC_STATIC_ETC_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_ETC_STATUS, "Fail~!");
			break;
		case CMD_DBUG:
			if (CoordiR->GetMode() == WRITE_COORDI && CoordiR->VerifyOK(rxBuff) == TRUE)
				SetDlgItemText(hDlg, IDC_STATIC_ETC_STATUS, "WOK~!");
			else if (CoordiR->GetMode() == READ_COORDI && dwBytesRead != 0)
			{
				CopyData(str, rxBuff, dwBytesRead);
				i = (INT)A2CtoHex(str[0], str[1]);
				if (i == 0x01) CheckDlgButton(hDlg, IDC_CHECK_DEBUG_MODE, BST_CHECKED);
				else CheckDlgButton(hDlg, IDC_CHECK_DEBUG_MODE, BST_UNCHECKED);
				SetDlgItemText(hDlg, IDC_STATIC_ETC_STATUS, "ROK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_ETC_STATUS, "Fail~!");
			break;
		case CMD_EXIT:
			if (strncmp("CMD time out", rxBuff, 12) == 0) SetDlgItemText(hDlg, IDC_STATIC_EEPROM_STATUS, "CMD mode out");
			break;
		case CMD_CH0S:
			if (dwBytesRead != 0) {
				CopyData(str, rxBuff, dwBytesRead);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_CH0S), EM_SETSEL, 0, -1);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_CH0S), EM_REPLACESEL, 0, (LPARAM)str);
				SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "OK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "Fail~!");
			break;
		case CMD_CH0R:
			if (dwBytesRead != 0) {
				CopyData(str, rxBuff, dwBytesRead);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_CH0R), EM_SETSEL, 0, -1);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_CH0R), EM_REPLACESEL, 0, (LPARAM)str);
				SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "OK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "Fail~!");
			break;
		case CMD_CH1S:
			if (dwBytesRead != 0) {
				CopyData(str, rxBuff, dwBytesRead);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_CH1S), EM_SETSEL, 0, -1);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_CH1S), EM_REPLACESEL, 0, (LPARAM)str);
				SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "OK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "Fail~!");
			break;
		case CMD_CH1R:
			if (dwBytesRead != 0) {
				CopyData(str, rxBuff, dwBytesRead);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_CH1R), EM_SETSEL, 0, -1);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_CH1R), EM_REPLACESEL, 0, (LPARAM)str);
				SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "OK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "Fail~!");
			break;
		case CMD_TIME:
			if (dwBytesRead != 0) {
				CopyData(str, rxBuff, dwBytesRead);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_TIME), EM_SETSEL, 0, -1);
				SendMessage(GetDlgItem(hDlg, IDC_EDIT_TIME), EM_REPLACESEL, 0, (LPARAM)str);
				SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "OK~!");
			}
			else SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "Fail~!");
			break;
		case CMD_IDSC:
			SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "OK~!");
			break;
		case CMD_TXPK:
			SetDlgItemText(hDlg, IDC_STATIC_ETCCMD_STATUS, "OK~!");
			break;
		case CMD_ZB_IS:
			CopyData(str, rxBuff, dwBytesRead);
			str[dwBytesRead] = 0;
			SetDlgItemText(hDlg, IDC_STATIC_MSENSOR_SSTATUS, str);
			break;
		case CMD_TIMEOUT:
			SetDlgItemText(hDlg, IDC_STATIC_EEPROM_STATUS, "CMD mode out");
			break;
		}
		return TRUE;
	case WM_USER + 2: // send - 타이머1 루프_명령 플래그 확인 후 생성된 해당 명령을 연결된 시리얼포트로 보냄
					  // 장치에 설정된 시리얼 포트 이름 확인하여 터미널 인덱스 값 얻음
		i = SendMessage(hList, LB_GETCURSEL, 0, 0);
		i = MyDev.GetDeviceNum(i);
		ptrStr = MyDev.MyDev[i]->GetDeviceTerminalName();
		if (MySerial.IsConnectedPort(ptrStr) != TRUE) return TRUE;
		indexPort = MySerial.GetConnectedPortNum(ptrStr);
		// 에러 상태 퍼지
		MySerial.SerialPort[indexPort]->Purge(retMes);
		switch (LOWORD(wParam)) {
		case CMD_ENTER:	// 커맨드 생성 후 보냄
			strcpy_s(txBuff, "###");
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_C1ES: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEC1ES");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = (UCHAR)GetDlgItemInt(hDlg, IDC_EDIT_COUNTRY_N, NULL, FALSE);
				wsprintf(str, "%02X", i);
				strncat_s(txBuff, str, 2);
				i = (UCHAR)GetDlgItemInt(hDlg, IDC_EDIT_AREA_N, NULL, FALSE);
				wsprintf(str, "%02X", i);
				strncat_s(txBuff, str, 2);
				i = (USHORT)GetDlgItemInt(hDlg, IDC_EDIT_TE_N, NULL, FALSE);
				wsprintf(str, "%04X", i);
				strncat_s(txBuff, str, 4);
				i = (USHORT)GetDlgItemInt(hDlg, IDC_EDIT_PHONE_N, NULL, FALSE);
				wsprintf(str, "%04X", i);
				strncat_s(txBuff, str, 4);
				i = (UCHAR)GetDlgItemInt(hDlg, IDC_EDIT_INVERTER_C, NULL, FALSE);
				wsprintf(str, "%02X", i);
				strncat_s(txBuff, str, 2);
				i = (UCHAR)GetDlgItemInt(hDlg, IDC_EDIT_COORDI_C, NULL, FALSE);
				wsprintf(str, "%02X", i);
				strncat_s(txBuff, str, 2);
				i = (UCHAR)GetDlgItemInt(hDlg, IDC_EDIT_ARRAY_C, NULL, FALSE);
				wsprintf(str, "%02X", i);
				strncat_s(txBuff, str, 2);
				i = (USHORT)GetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_C, NULL, FALSE);
				wsprintf(str, "%04X", i);
				strncat_s(txBuff, str, 4);
				i = (UCHAR)GetDlgItemInt(hDlg, IDC_EDIT_DCPATH, NULL, FALSE);
				wsprintf(str, "%02X", i);
				strncat_s(txBuff, str, 2);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_C1CD: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEC1CD");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = (UCHAR)GetDlgItemInt(hDlg, IDC_EDIT_COORDI_ID, NULL, FALSE);
				wsprintf(str, "%02X", i);
				strncat_s(txBuff, str, 2);
				GetDlgItemText(hDlg, IDC_EDIT_COORDI_SN, str, 17);
				strncat_s(txBuff, str, 16);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_MDXY: // 커맨드 생성 후 보냄
			i = CoordiR->GetMSIFlagModuleNum();
			wsprintf(txBuff, "DEMD%02X", i);
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				if (CoordiR->GetMSIFlagSingle()) {
					GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SID, retMes, sizeof(retMes));
					i = atoi(retMes);
					wsprintf(retMes, "%04X", i);
					strcpy_s(str, retMes);
					GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_A_SID, retMes, sizeof(retMes));
					i = atoi(retMes);
					wsprintf(retMes, "%02X", i);
					strcat_s(str, retMes);
					GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_X_SPOS, retMes, sizeof(retMes));
					i = atoi(retMes);
					wsprintf(retMes, "%02X", i);
					strcat_s(str, retMes);
					GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_Y_SPOS, retMes, sizeof(retMes));
					i = atoi(retMes);
					wsprintf(retMes, "%02X", i);
					strcat_s(str, retMes);
					GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SDCPATH, retMes, sizeof(retMes));
					i = atoi(retMes);
					wsprintf(retMes, "%02X", i);
					strcat_s(str, retMes);
					GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SSN, retMes, sizeof(retMes));
					strcat_s(str, retMes);
					GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SCID, retMes, sizeof(retMes));
					i = atoi(retMes);
					wsprintf(retMes, "%02X", i);
					strcat_s(str, retMes);
					GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_SINDEX, retMes, sizeof(retMes));					
					strcat_s(str, retMes);
				}
				else
				{
					ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), i - 1, 0, retMes, sizeof(retMes));
					wsprintf(retMes, "%04X", atoi(retMes));
					strcpy_s(str, retMes);
					ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), i - 1, 1, retMes, sizeof(retMes));
					wsprintf(retMes, "%02X", atoi(retMes));
					strcat_s(str, retMes);
					ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), i - 1, 2, retMes, sizeof(retMes));
					wsprintf(retMes, "%02X", atoi(retMes));
					strcat_s(str, retMes);
					ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), i - 1, 3, retMes, sizeof(retMes));
					wsprintf(retMes, "%02X", atoi(retMes));
					strcat_s(str, retMes);
					ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), i - 1, 4, retMes, sizeof(retMes));
					wsprintf(retMes, "%02X", atoi(retMes));
					strcat_s(str, retMes);
					ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), i - 1, 5, retMes, sizeof(retMes));
					strcat_s(str, retMes);
					ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), i - 1, 6, retMes, sizeof(retMes));
					wsprintf(retMes, "%02X", atoi(retMes));
					strcat_s(str, retMes);
					ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST_TEXTFILE), i - 1, 7, retMes, sizeof(retMes));					
					strcat_s(str, retMes);
				}
				strcat_s(txBuff, str);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_INVX: // 커맨드 생성 후 보냄		
			wsprintf(txBuff, "DEINV%X", CoordiR->GetFlagInvNum());
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = CoordiR->GetFlagInvNum();
				CoordiR->GetInvData(str, i);
				strncat_s(txBuff, str, 11);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_INEH: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEINEH");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = 0;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INVF) == BST_CHECKED) i |= 0b10000000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INVE) == BST_CHECKED) i |= 0b01000000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INVD) == BST_CHECKED) i |= 0b00100000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INVC) == BST_CHECKED) i |= 0b00010000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INVB) == BST_CHECKED) i |= 0b00001000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INVA) == BST_CHECKED) i |= 0b00000100;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV9) == BST_CHECKED) i |= 0b00000010;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV8) == BST_CHECKED) i |= 0b00000001;
				HtoA2(i, &valueInt[0], &valueInt[1]);
				valueInt[3] = NULL;
				strncat_s(txBuff, valueInt, 2);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_INEL: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEINEL");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = 0;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV7) == BST_CHECKED) i |= 0b10000000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV6) == BST_CHECKED) i |= 0b01000000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV5) == BST_CHECKED) i |= 0b00100000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV4) == BST_CHECKED) i |= 0b00010000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV3) == BST_CHECKED) i |= 0b00001000;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV2) == BST_CHECKED) i |= 0b00000100;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV1) == BST_CHECKED) i |= 0b00000010;
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_INV0) == BST_CHECKED) i |= 0b00000001;
				HtoA2(i, &valueInt[0], &valueInt[1]);
				valueInt[3] = NULL;
				strncat_s(txBuff, valueInt, 2);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_INMD: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEINMD");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = 0;
				if (IsDlgButtonChecked(hDlg, IDC_RADIO_INMD01) == BST_CHECKED) i = 0x01;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_INMD11) == BST_CHECKED) i = 0x11;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_INMD21) == BST_CHECKED) i = 0x21;
				else i = 0;
				HtoA2(i, &valueInt[0], &valueInt[1]);
				valueInt[3] = NULL;
				strncat_s(txBuff, valueInt, 2);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_ZBMD: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEZBMD");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = 0;
				if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD01) == BST_CHECKED) i = 0x01;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD02) == BST_CHECKED) i = 0x02;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD04) == BST_CHECKED) i = 0x04;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD11) == BST_CHECKED) i = 0x11;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD12) == BST_CHECKED) i = 0x12;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD14) == BST_CHECKED) i = 0x14;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD21) == BST_CHECKED) i = 0x21;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD22) == BST_CHECKED) i = 0x22;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_ZBMD24) == BST_CHECKED) i = 0x24;
				else i = 0;
				HtoA2(i, &valueInt[0], &valueInt[1]);
				valueInt[3] = NULL;
				strncat_s(txBuff, valueInt, 2);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_PVEM: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEPVEM");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = 0;
				if (IsDlgButtonChecked(hDlg, IDC_RADIO_PVEM01) == BST_CHECKED) i = 0x01;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_PVEM02) == BST_CHECKED) i = 0x02;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_PVEM03) == BST_CHECKED) i = 0x03;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_PVEM11) == BST_CHECKED) i = 0x11;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_PVEM12) == BST_CHECKED) i = 0x12;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_PVEM21) == BST_CHECKED) i = 0x21;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_PVEM22) == BST_CHECKED) i = 0x22;
				else i = 0;
				HtoA2(i, &valueInt[0], &valueInt[1]);
				valueInt[3] = NULL;
				strncat_s(txBuff, valueInt, 2);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_LOMM: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DELOMM");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				i = 0;
				if (IsDlgButtonChecked(hDlg, IDC_RADIO_LOMM01) == BST_CHECKED) i = 0x01;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_LOMM02) == BST_CHECKED) i = 0x02;
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO_LOMM03) == BST_CHECKED) i = 0x03;
				else i = 0;
				HtoA2(i, &valueInt[0], &valueInt[1]);
				valueInt[3] = NULL;
				strncat_s(txBuff, valueInt, 2);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_INFO: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEINFO");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_BVER: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEBVER");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				GetDlgItemText(hDlg, IDC_EDIT_MSENSOR_V, str, 7);
				if (lstrlen(str) == 0) strcat_s(txBuff, "      \r");
				else { strncat_s(txBuff, str, 6); strcat_s(txBuff, "\r"); }
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_DBUG: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "DEDBUG");
			if (CoordiR->GetMode() == READ_COORDI) strcat_s(txBuff, "\r");
			else if (CoordiR->GetMode() == WRITE_COORDI) {
				if (IsDlgButtonChecked(hDlg, IDC_CHECK_DEBUG_MODE) == BST_CHECKED) strcpy_s(str, "01");
				else strcpy_s(str, "00");
				strncat_s(txBuff, str, 2);
				strcat_s(txBuff, "\r");
			}
			else break;
			dwBytesWrite = lstrlen(txBuff);
			break;
			//case CMD_EEPI:
		case CMD_CH0S:
			strcpy_s(txBuff, "DECH0S\r");
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_CH0R:
			strcpy_s(txBuff, "DECH0R\r");
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_CH1S:
			strcpy_s(txBuff, "DECH1S\r");
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_CH1R:
			strcpy_s(txBuff, "DECH1R\r");
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_TIME:
			strcpy_s(txBuff, "DETIME\r");
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_IDSC:
			strcpy_s(txBuff, "DEIDSC\r");
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_TXPK:
			strcpy_s(txBuff, "DETXPK\r");
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_ZB_IS:
			i = GetDlgItemInt(hDlg, IDC_EDIT_MSENSOR_SRN, 0, 0);
			wsprintf(txBuff, "readzbr -is%d", i);
			dwBytesWrite = lstrlen(txBuff);
			break;
		case CMD_EXIT: // 커맨드 생성 후 보냄
			strcpy_s(txBuff, "***");
			dwBytesWrite = lstrlen(txBuff);
			break;
		}
		MySerial.SerialPort[indexPort]->Send(txBuff, dwBytesWrite, retMes);
		SendMessage(hCRB, WM_COMMAND, (WPARAM)12, indexPort);
		return TRUE;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		//MyBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		MyBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 200, 0));
		OldBrush = (HBRUSH)SelectObject(hdc, MyBrush);
		MyPen = (HPEN)GetStockObject(NULL_PEN);
		//MyPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		OldPen = (HPEN)SelectObject(hdc, MyPen);
		//SetBkColor(hdc, RGB(0, 200, 0));
		//GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_COUNTRY_N), &rt);
		//GetClientRect(GetDlgItem(hDlg, IDC_STATIC_COUNTRY_N), &crt);	 
		//DrawFocusRect(hdc, &rt);
		//GetWindowRect(GetDlgItem(hDlg, IDC_BUTTON_EPSD_R), &rt);		
		//Rectangle(hdc, rt.left - rtDlg.left, rt.top - rtDlg.top, rt.right - rtDlg.right, rt.bottom - rtDlg.bottom);
		//Rectangle(hdc, rt.left, rt.top, rt.right, rt.bottom);
		//Rectangle(hdc, crt.left, crt.top, crt.right, crt.bottom);
		//GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_EPSD_G), &rt);
		//Rectangle(hdc, rt.left, rt.top, rt.right, rt.bottom);
		SelectObject(hdc, OldBrush);
		SelectObject(hdc, OldPen);
		DeleteObject(MyBrush);
		DeleteObject(MyPen);
		EndPaint(hDlg, &ps);
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK InsertFileDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	INT index;
	TCHAR str[MAX_PATH];
	UCHAR indexSensor;
	HANDLE hFile;
	TCHAR *buff;
	LVITEM litem;
	DWORD dwRead;
	INT readCnt;
	INT cnt = 0;
	INT cnt2 = 0;
	INT cnt3 = 0;
	INT row = 0;
	INT column = 0;
	switch (iMessage) {
	case WM_INITDIALOG:
		// 텍스트 형태 파일을 대화상자 리스트에 추가
		FindDeviceFileEX(GetDlgItem(hDlg, IDC_LIST_TEXTFILENAME), (TCHAR*) "txt");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			buff = (TCHAR*)malloc(1048576); // 10k 힙 할당
			index = SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILENAME), LB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hDlg, IDC_LIST_TEXTFILENAME), LB_GETTEXT, (WPARAM)index, (LPARAM)str);
			hFile = CreateFile(str, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE) {
				ReadFile(hFile, buff, 1048576, &dwRead, NULL);
				readCnt = (INT)dwRead;
				CloseHandle(hFile);
				ListView_DeleteAllItems(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE));
				while (readCnt) { // 읽어들인 파일 버퍼가 0이 될때까지 루프
					// 줄바뀜 조건
					if (buff[cnt] == '\r' && buff[cnt + 1] == '\n') { row++; cnt += 2; readCnt--; column = 0; }
					// 첫 줄 조건
					else if (row == 0) { cnt++; readCnt--; }					
					else { // 두 번째 줄부터 진행
						while (buff[cnt] != '\t') {
							if (buff[cnt] == '\r' || cnt >= dwRead) break;
							str[cnt2] = buff[cnt];
							cnt++;
							cnt2++;
							readCnt--;
						}
						if (buff[cnt] != '\r') { cnt++;	readCnt--; }
						str[cnt2] = NULL;
						column++;
						cnt2 = 0;						
						// 1, 2, 4채널 첫 열 텍스트 삽입 조건, 최대 255개 삽입 가능
						if (column == 1 && row < 1020) {
							litem.mask = LVIF_TEXT;
							litem.iItem = row - 1;
							litem.iSubItem = column - 1;
							litem.pszText = (LPSTR)str;
							SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_INSERTITEM, 0, (LPARAM)&litem);
							cnt3++;
						}
						else if (row < 1020) { // 두번째 열부터 텍스트 삽입 조건
							litem.iSubItem = column - 1;
							litem.pszText = (LPSTR)str;
							SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
							if (column == 8) { // 1, 2, 4채널 마지막 열에서의 조건
								litem.iSubItem = column;
								litem.pszText = (LPSTR)TEXT("INSERT"); // 상태표시 텍스트
								SendMessage(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), LVM_SETITEM, 0, (LPARAM)&litem);
							}
						}													
					}					
				}				
				for (cnt = cnt3; cnt > 0;cnt--) {										
					ListView_GetItemText(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), cnt - 1, 7, str, MAX_PATH);
					indexSensor = (UCHAR)A2CtoHex(str[0], str[1]);
					if (indexSensor!=0xFF && (indexSensor & 0b10000000)) { ListView_DeleteItem(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE), cnt-1); }
				}
				cnt3 = ListView_GetItemCount(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE));
				if (cnt3 > 255) ListView_DeleteAllItems(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE));
			}
			wsprintf(str, "%d", ListView_GetItemCount(GetDlgItem(hCoordi, IDC_LIST_TEXTFILE)));
			SetDlgItemText(hCoordi, IDC_EDIT_MSENSOR_CNT, (LPCSTR)str);
			// 삽입된 정보 행 갯수 표시
			free(buff); // 힙 소멸
			EndDialog(hDlg, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;

	}
	return FALSE;
}

VOID SetInvPhaseCombobox(HWND hDlg, INT indexType, INT indexPhase)
{
	INT i;
	TCHAR str[MAX_PATH];
	i = SendMessage(GetDlgItem(hDlg, indexType), CB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(hDlg, indexType), CB_GETLBTEXT, i, (LPARAM)str);
	if (strcmp(str, "HEXP") == 0) {
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_RESETCONTENT, 0, 0);
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)" ");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"1");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"3");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"S");
	}
	else if (strcmp(str, "WILL") == 0) {
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_RESETCONTENT, 0, 0);
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)" ");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"1");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"3");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"M");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"S");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"T");
	}
	else if (strcmp(str, "REMS") == 0) {
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_RESETCONTENT, 0, 0);
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)" ");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"1");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"3");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"O");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"T");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"F");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"S");
	}
	else {
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_RESETCONTENT, 0, 0);
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)" ");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"1");
		SendMessage(GetDlgItem(hDlg, indexPhase), CB_ADDSTRING, 0, (LPARAM)"3");
	}
}