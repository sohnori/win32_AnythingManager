#include "DlgProc_AddDevice.h"
#include "MyDeviceHandler.h"
#include "MyFunc.h"
#include "resource.h"

const TCHAR textHostPC[] = TEXT("None");
const TCHAR textCoordi[] = TEXT("CoordinatorIoT");
const TCHAR textInverter[] = TEXT("Inverter");
const TCHAR textetc[] = TEXT("etcDevice");

extern MyDeviceHandler MyDev;
extern HWND hList;

// ��ġ �߰� ��ȭ���� �޽��� ���ν��� ó�� �Լ�
INT OnInitDialogAddDeviceProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT OnCommandAddDeviceProc(HWND hDlg, WPARAM wParam, LPARAM lParam);

// ��ġ�߰� ��ȭ���� �޽��� ���ν���
BOOL CALLBACK AddDeviceDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG: OnInitDialogAddDeviceProc(hDlg, wParam, lParam); return TRUE;
	case WM_COMMAND: OnCommandAddDeviceProc(hDlg, wParam, lParam); return TRUE;
	}
	return FALSE;
}

INT OnInitDialogAddDeviceProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DT), CB_ADDSTRING, 0, (LPARAM)textHostPC);
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DT), CB_ADDSTRING, 0, (LPARAM)textCoordi);
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DT), CB_ADDSTRING, 0, (LPARAM)textInverter);
	SendMessage(GetDlgItem(hDlg, IDC_COMBO_DT), CB_ADDSTRING, 0, (LPARAM)textetc);
	return TRUE;
}

INT OnCommandAddDeviceProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	tagDeviceInfo devInfo = { 0, };
	TCHAR buffStr[MAX_PATH];
	INT cnt;
	INT i;
	switch (LOWORD(wParam)) {
	case IDOK:
		i = SendMessage(hList, LB_GETCOUNT, 0, 0);
		if (i>=MYDEVICE_COUNT_MAX) {
			MessageBox(hDlg, TEXT("���̻� ��ġ�� ������ �� �����ϴ�."), TEXT("���"), MB_OK);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}		
		GetDlgItemText(hDlg, IDC_EDIT_DN, buffStr, sizeof(buffStr));
		if (MyDev.CheckOverlapDevName(buffStr) == TRUE) {
			MessageBox(hDlg, TEXT("���� �̸��� ��ġ�� �����մϴ�."), TEXT("���"), MB_OK);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		GetDlgItemText(hDlg, IDC_EDIT_DN, devInfo.deviceName, sizeof(devInfo.deviceName));		
		i = SendMessage(GetDlgItem(hDlg, IDC_COMBO_DT), CB_GETCURSEL, 0, 0);
		devInfo.deviceType = (dType)i;		
		if (IsDlgButtonChecked(hDlg, IDC_CHECK_SIMUL) == TRUE) devInfo.flagSimulator = TRUE;
		else devInfo.flagSimulator = FALSE;
		i = MyDev.GetAvailableMyDevice();
		MyDev.NewMyDevice(devInfo, i);
		SendMessage(hList, LB_RESETCONTENT, 0, 0);
		for (cnt = 0; cnt < MYDEVICE_COUNT_MAX; cnt++) {
			if (MyDev.GetFlagIsDevice(cnt) == TRUE) {
				i = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)MyDev.MyDev[cnt]->GetDeviceName());
				MyDev.MyDev[cnt]->SetDeviceIndex(i);
			}
		}
		EndDialog(hDlg, IDOK);
		return TRUE;
	case IDCANCEL:
		EndDialog(hDlg, IDCANCEL);
		return TRUE;
	}
	return TRUE;
}
