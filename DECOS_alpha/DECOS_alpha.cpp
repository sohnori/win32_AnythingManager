#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Dbt.h>
#include "Procedure_CLB.h"
#include "Procedure_CLT.h"
#include "Procedure_CRB.h"
#include "Procedure_CRT.h"
#include "SerialComm.h"
#include "SerialCommHandler.h"
#include "MyDevice.h"
#include "MyDeviceHandler.h"
#include "DEAPIProtocol.h"
#include "MyFunc.h"
#include "CRC.h"
#include "resource.h"
#include "DECOS_alpha.h"

#define PROGRAM_NAME_VERSION	TEXT("DECOS_alpha_v1_0")

// ī��ǥ��� - analogWrite(����)
// �Ľ�Įǥ��� - SoftwareSerial(Ŭ����, �Լ�?, ����?)
// ������ũ ǥ��� - apple_banana_jam
// �밡���� ǥ��� - cbBuff
// path = C:\Users\sohno\source\repos\APIEXAM\DECOS_alpha
// ��ű���������� - https://docs.microsoft.com/ko-kr/windows/desktop/DevIO/communications-functions 
// https://docs.microsoft.com/ko-kr/windows/desktop/DevIO/communications-resources
#define DWSTYLE	WS_OVERLAPPEDWINDOW 
#define CHILD_GAP	5
#define INIT_WX		0
#define INIT_WY		40

static int RCHRatio = 50;
static int LCHRatio = 50;
static int LCWidth  = 300;
enum SPLIT { SP_NONE, SP_VERT, SP_HORZ };
SPLIT NowSplit = SP_NONE;

// ���� ������ �޽��� ���ν���
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//VOID CALLBACK Timer1Proc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);


// ��ȭ���� �޽��� ���ν���
BOOL CALLBACK DECMDDlgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CRCDlgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// ���� ������ �޽��� ���ν��� ó���Լ�
INT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnSetCursor(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnDeviceChange(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);

// ������ ���� �ڵ�
HWND hWndMain;

// ���ϵ� ������ �ڵ�
HWND hCLT, hCRT, hCRB;

// ���ϵ� ��ư �ڵ�
HWND hConnButton, hButtonCRC;
HWND hBtnTest;

// �𵨸��� ��ȭ���� �ڵ�
HWND hHostPC, hCoordi, hInverter, hEtc, hDECMD, hCRC;
// ���ϵ� ������ ���ϵ� ����Ʈ �ڵ�
HWND hCmdEdit;

// ������ ���ν��� ������
WNDPROC OldWndProc;
// ȸ�� �ΰ�
static HBITMAP MyBitmap;

HINSTANCE g_hInst;
LPCTSTR lpszClass = PROGRAM_NAME_VERSION;

MyDeviceHandler MyDev;
SerialCommHandler MySerial;

// ��Ʈ�� �׸��� �Լ� ����
void DrawBitmap(HWND hWnd, HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;
	RECT rt;

	GetClientRect(hWnd, &rt);
	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, MyBitmap);
	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;
	//BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY); // if want to set start position
	BitBlt(hdc, rt.right - bx - 5, rt.top + 5, bx, by, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}

// ���ϵ� ������ â ���� �缳�� �����Լ�
SPLIT GetSplitter(POINT pt)
{
	RECT crt, hrt, vrt;
	int Height;
	GetClientRect(hWndMain, &crt);
	Height = crt.bottom*LCHRatio / 100;
	SetRect(&vrt, INIT_WX + LCWidth - CHILD_GAP, INIT_WY, INIT_WX + LCWidth, INIT_WY + crt.bottom);
	if (PtInRect(&vrt, pt)) {
		return SP_VERT;
	}
	SetRect(&hrt, INIT_WX, INIT_WY + Height - CHILD_GAP, INIT_WX + crt.right, INIT_WY + Height);
	if (PtInRect(&hrt, pt)) {
		return SP_HORZ;
	}
	return SP_NONE;
}

// ���� Ŭ���� �Լ� ����
LRESULT CALLBACK CMDeditProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{ // ���ϴ� ���ϵ� �������� ������ ���â�� Ű �Է� ó��
	switch (iMessage) {
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			//SetFocus(hWnd);
			SendMessage(hCRB, WM_COMMAND, 3, 0);
		}
		if (wParam == VK_UP) {
			SendMessage(hCmdEdit, EM_UNDO, 0, 0);
		}
		if (wParam == VK_DOWN) {
			SendMessage(hCmdEdit, EM_CANUNDO, 0, 0);
		}
		break;
	}
	return CallWindowProc(OldWndProc, hWnd, iMessage, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	HACCEL hAccel;
	g_hInst = hInstance;
	HBRUSH hCLTBrush, hCRTBrush, hCRBBrush;

	hCLTBrush = CreateSolidBrush(RGB(245, 255, 255));
	hCRTBrush = CreateSolidBrush(RGB(255, 245, 255));
	hCRBBrush = CreateSolidBrush(RGB(255, 255, 245));	

	// ���� ������ Ŭ����
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //NULL;	
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	//WndClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_TEST));
	//WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_TEST));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	// ������ ���ϵ� Ŭ����
	WndClass.cbWndExtra = sizeof(INT);
	WndClass.hbrBackground = hCLTBrush;
	WndClass.lpfnWndProc = ChildLTProc;
	WndClass.lpszClassName = TEXT("ChildLTop");
	RegisterClass(&WndClass);	

	// �������� ���ϵ� Ŭ����
	WndClass.cbWndExtra = sizeof(INT);
	WndClass.hbrBackground = hCRTBrush;
	WndClass.lpfnWndProc = ChildRTProc;
	WndClass.lpszClassName = TEXT("ChildRTop");
	RegisterClass(&WndClass);

	// �����Ʒ��� ���ϵ� Ŭ����
	WndClass.cbWndExtra = sizeof(INT);
	WndClass.hbrBackground = hCRBBrush;
	WndClass.lpfnWndProc = ChildRBProc;
	WndClass.lpszClassName = TEXT("ChildRBottom");
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, DWSTYLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL); 
	ShowWindow(hWnd, nCmdShow);
	hWndMain = hWnd;
	// origin form - CreateWindow(lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeght, hWndparent, hMenu, hInstance, lpParam);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	while (GetMessage(&Message, NULL, 0, 0)) {
		//if (Message.message == WM_USER) ;
			// �޽��� ����ó��
		if (!TranslateAccelerator(hWnd, hAccel, &Message) ||			
			!IsWindow(hDECMD) || !IsDialogMessage(hDECMD, &Message) ||
			!IsWindow(hCRC) || !IsDialogMessage(hCRC, &Message) ||
			!IsWindow(hInverter) || !IsDialogMessage(hInverter, &Message) ||
			!IsWindow(hEtc) || !IsDialogMessage(hEtc, &Message) ||
			!IsWindow(hHostPC) || !IsDialogMessage(hHostPC, &Message) ||
			!IsWindow(hCoordi) || !IsDialogMessage(hCoordi, &Message)) {		
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}	
	}
	DeleteObject(hCLTBrush);	
	DeleteObject(hCRTBrush);
	DeleteObject(hCRBBrush);
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_CREATE: return OnCreate(hWnd, wParam, lParam);
	case WM_SIZE: OnSize(hWnd, wParam, lParam); return 0;
	case WM_SETCURSOR: if (OnSetCursor(hWnd, wParam, lParam)) return 0; break;
	case WM_COMMAND: OnCommand(hWnd, wParam, lParam); break;
	case WM_LBUTTONDOWN: OnLButtonDown(hWnd, wParam, lParam); return 0;
	case WM_MOUSEMOVE: OnMouseMove(hWnd, wParam, lParam); return 0;
	case WM_LBUTTONUP: OnLButtonUp(hWnd, wParam, lParam); return 0;
	//case WM_RBUTTONDOWN:
	case WM_DEVICECHANGE: OnDeviceChange(hWnd, wParam, lParam); return 0;
	case WM_PAINT: OnPaint(hWnd, wParam, lParam); return 0;		
	case WM_DESTROY: OnDestroy(hWnd, wParam, lParam); return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	hWndMain = hWnd;
	hConnButton = CreateWindow(TEXT("button"), TEXT("DE cmd"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		10, 10, 80, 20, hWnd, (HMENU)1, g_hInst, NULL);
	hButtonCRC = CreateWindow(TEXT("button"), TEXT("CRC������"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		100, 10, 80, 20, hWnd, (HMENU)2, g_hInst, NULL);
	hCLT = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("ChildLTop"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER,
		0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
	SetWindowLong(hCLT, 0, TRUE);
	hCRT = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("ChildRTop"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER
		| WS_VSCROLL, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);	
	SetWindowLong(hCRT, 0, TRUE);
	hCRB = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("ChildRBottom"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
		0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
	hBtnTest = CreateWindow(TEXT("button"), TEXT("Test"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		200, 10, 100, 20, hWnd, (HMENU)3, g_hInst, NULL);
	SetWindowLong(hCRB, 0, TRUE);
	//EnableWindow(hConnButton, FALSE);	

	MyBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));

	// ���� �ø�����Ʈ ���� �б� ���� Ÿ�̸� 1 per 200ms
	//SetTimer(hWnd, 1, 600, Timer1Proc);
	//SetTimer(hWnd, 1, 600, (TIMERPROC)Timer1Proc);

	// ���� �Ʒ� ������ ���â�� ���� ����Ŭ���� ���ν��� ����
	OldWndProc = (WNDPROC)SetWindowLongPtr(hCmdEdit, GWLP_WNDPROC, (LONG_PTR)CMDeditProc);
	return 0;
}

INT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT crt;
	INT Height;
	if (GetWindowLong(hCLT, 0) == FALSE || GetWindowLong(hCRT, 0) == FALSE || GetWindowLong(hCRB, 0) == FALSE) return 0;
	if (wParam != SIZE_MINIMIZED) {
		GetClientRect(hWnd, &crt);
		Height = crt.bottom*LCHRatio / 100;
		MoveWindow(hCLT, INIT_WX, INIT_WY, LCWidth - CHILD_GAP, crt.bottom - 40, TRUE);				
		MoveWindow(hCRT, INIT_WX + LCWidth, INIT_WY, crt.right - LCWidth, Height - CHILD_GAP, TRUE);				
		MoveWindow(hCRB, INIT_WX + LCWidth, INIT_WY + Height, crt.right - LCWidth, crt.bottom - Height, TRUE);
	}
	return 0;
}

INT OnSetCursor(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	SPLIT sp;
	if (GetWindowLong(hCLT, 0) == FALSE || GetWindowLong(hCRT, 0) == FALSE || GetWindowLong(hCRB, 0) == FALSE) return 0;
	if (LOWORD(lParam) == HTCLIENT) {
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		sp = GetSplitter(pt);
		if (sp == SP_VERT) {
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			return TRUE;
		}
		if (sp == SP_HORZ) {
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			return TRUE;
		}
	}
	return FALSE;
}

INT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//RECT rt;
	switch (LOWORD(wParam)) {	
	case 1:
		if (!IsWindow(hDECMD)) {
			//hDECMD = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_DECMD), hWnd, DECMDDlgProc);
			hDECMD = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_DECMD), hWnd, (DLGPROC)DECMDDlgProc);
			ShowWindow(hDECMD, SW_SHOW);
		}
		else {			
			//GetWindowRect(hDECMD, &rt);
			//MoveWindow(hDECMD, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
			SetWindowPos(hDECMD, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);
		}		
		break;
	case 2:
		if (!IsWindow(hCRC)) {
			//hCRC = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_DECMD), hWnd, DECMDDlgProc);
			hCRC = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_CRC), hWnd, (DLGPROC)CRCDlgProc);
			ShowWindow(hCRC, SW_SHOW);
		}
		else {
			//GetWindowRect(hCRC, &rt);
			//MoveWindow(hCRC, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
			SetWindowPos(hCRC, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);
		}
		break;
	case 3: // ��� �׽�Ʈ ��ư		
		break;
	case ID_FILE_OPEN:
		OPENFILENAME OFN;
		TCHAR str[300];
		TCHAR lpstrFile[MAX_PATH];
		TCHAR szFileTitle[MAX_PATH];
		TCHAR InitDir[MAX_PATH];
		memset(&OFN, 0, sizeof(OPENFILENAME));
		OFN.lStructSize = sizeof(OPENFILENAME);
		OFN.hwndOwner = hWnd;
		OFN.lpstrFilter = TEXT("�ؽ�Ʈ ����(.txt)\0*.txt;*.doc\0��� ����(*.*)\0*.*\0");
		OFN.lpstrFile = lpstrFile;
		OFN.nMaxFile = MAX_PATH;
		OFN.lpstrTitle = TEXT("������ �����Ͻʽÿ�.");
		OFN.lpstrFileTitle = szFileTitle;
		OFN.nMaxFileTitle = MAX_PATH;
		OFN.lpstrDefExt = "txt";
		//GetWindowsDirectory(InitDir, MAX_PATH);
		strcpy_s(InitDir, ".\\");
		OFN.lpstrInitialDir = InitDir;
		if (GetOpenFileName(&OFN) != 0) {
			wsprintf(str, TEXT("%s ���� ���õ�. ���ϸ��� %s, ���ϸ� �ɼ� = %d, Ȯ���� �ɼ� = %d")
				, lpstrFile, szFileTitle, OFN.nFileOffset, OFN.nFileExtension);
			MessageBox(hWnd, str, TEXT("���� ���� ����"), MB_OK);
		}
		else {
			DWORD errCode = CommDlgExtendedError();
			wsprintf(str, TEXT("�����ڵ� : %d"), errCode);
			MessageBox(hWnd, str, TEXT("���� ���� ����"), MB_OK);
		}
		break;
	case ID_FILE_EXIT:
		SendMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case ID_EDIT_SETPOS0:
		//GetWindowRect(hWnd, &rt);
		//MoveWindow(hWnd, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);
		break;
	case ID_VIEW_CHANGEBKCOLOR:
		HBRUSH bkBrush;
		COLORREF colorBk = RGB(0,0,255);
		CHOOSECOLOR COL;
		COLORREF crTemp[16];
		INT cnt;
		for (cnt = 0; cnt < 16; cnt++) {
			crTemp[cnt] = RGB(0, 0, cnt * 8 + 128);
		}
		memset(&COL, 0, sizeof(CHOOSECOLOR));
		COL.lStructSize = sizeof(CHOOSECOLOR);
		COL.hwndOwner = hWnd;
		COL.lpCustColors = crTemp;
		if (ChooseColor(&COL) != 0) {
			colorBk = COL.rgbResult;
			bkBrush = CreateSolidBrush(colorBk);
			SetWindowLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)bkBrush);
			InvalidateRect(hWnd, NULL, TRUE);
		}		
		break;
	}
	return 0;
}

INT OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;	
	if (GetWindowLong(hCLT, 0) == FALSE || GetWindowLong(hCRT, 0) == FALSE || GetWindowLong(hCRB, 0) == FALSE) return 0;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	NowSplit = GetSplitter(pt);
	if (NowSplit != SP_NONE) {
		SetCapture(hWnd);
	}
	return 0;
}

INT OnMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT crt;
	int Height;
	if (GetWindowLong(hCLT, 0) == FALSE || GetWindowLong(hCRT, 0) == FALSE || GetWindowLong(hCRB, 0) == FALSE) return 0;
	switch (NowSplit) {
	case SP_VERT:
		LCWidth = min(max((int)(short)LOWORD(lParam), 20), 300);
		SendMessage(hWnd, WM_SIZE, SIZE_RESTORED, 0);
		break;
	case SP_HORZ:
		Height = (int)(short)HIWORD(lParam);
		GetClientRect(hWnd, &crt);
		LCHRatio = Height * 100 / crt.bottom;
		LCHRatio = min(max(LCHRatio, 20), 80);
		SendMessage(hWnd, WM_SIZE, SIZE_RESTORED, 0);
		break;
	}
	return 0;
}

INT OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (GetWindowLong(hCLT, 0) == FALSE || GetWindowLong(hCRT, 0) == FALSE || GetWindowLong(hCRB, 0) == FALSE) return 0;
	NowSplit = SP_NONE;
	ReleaseCapture();
	return 0;
}

INT OnDeviceChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case DBT_DEVNODES_CHANGED:			
		MySerial.ReflashConnectedPort();
		// ����� �͹̳� ��Ʈ �޺� �ڽ��� �α� ����Ʈ, �� ��������
		SendMessage(hCLT, WM_USER + 1, 1, 0);
	}
	return 0;
	//MessageBox(hWnd, TEXT("��ġ ��ȭ ����!!"), TEXT("�޽��� �ڽ�"), MB_OK);
}

INT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;	
	//TCHAR str[MAX_PATH];
	//LoadString(g_hInst, IDS_STRING_INTRO, str, sizeof(str));
	hdc = BeginPaint(hWnd, &ps);	
	DrawBitmap(hWnd, hdc, 10, 10, MyBitmap);
	//InvalidateRect(hWnd, NULL, TRUE);
	//TextOut(hdc, 1000, 10, str, lstrlen(str));
	EndPaint(hWnd, &ps);
	return 0;
}

INT OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//KillTimer(hWnd, 1);
	SetWindowLongPtr(hCmdEdit, GWLP_WNDPROC, (LONG_PTR)OldWndProc);
	PostQuitMessage(0);
	return 0;
}

BOOL CALLBACK DECMDDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	TCHAR str[DEAPI_DATAMAXLENGTH];
	tagDEAPIPacket packet;
	switch (iMessage) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			SetDlgItemText(hDlg, IDC_STATIC_PAPIID, "-");
			SetDlgItemText(hDlg, IDC_STATIC_PCMD, "-");
			SetDlgItemText(hDlg, IDC_STATIC_PSTATUS, "-");
			SetDlgItemText(hDlg, IDC_STATIC_PFRAMEID, "-");
			SetDlgItemText(hDlg, IDC_STATIC_POPTIONH, "-");
			SetDlgItemText(hDlg, IDC_STATIC_POPTIONL, "-");
			SetDlgItemText(hDlg, IDC_STATIC_PCKSUM, "-");
			SetDlgItemText(hDlg, IDC_STATIC_PDATA, "-");
			if (GetDlgItemText(hDlg, IDC_EDIT_PCMDINSERT, str, sizeof(str)) < 20) break;
			if (GetDEAPIPacket(&packet, str, sizeof(str)) == -1) break;
			GetMSGVerifiedDEAPIPacket(str, &packet, DEAPI_APIID, sizeof(str));
			SetDlgItemText(hDlg, IDC_STATIC_PAPIID, str);
			GetMSGVerifiedDEAPIPacket(str, &packet, DEAPI_CMD, sizeof(str));
			SetDlgItemText(hDlg, IDC_STATIC_PCMD, str);
			GetMSGVerifiedDEAPIPacket(str, &packet, DEAPI_STATUS, sizeof(str));
			SetDlgItemText(hDlg, IDC_STATIC_PSTATUS, str);
			GetMSGVerifiedDEAPIPacket(str, &packet, DEAPI_FRAMEID, sizeof(str));
			SetDlgItemText(hDlg, IDC_STATIC_PFRAMEID, str);
			GetMSGVerifiedDEAPIPacket(str, &packet, DEAPI_OPTIONH, sizeof(str));
			//SetDlgItemText(hDlg, IDC_STATIC_POPTIONH, str);
			//GetMSGVerifiedDEAPIPacket(str, &packet, DEAPI_OPTIONL, sizeof(str));
			SetDlgItemText(hDlg, IDC_STATIC_POPTIONL, str);			
			GetMSGVerifiedDEAPIPacket(str, &packet, DEAPI_CKSUM, sizeof(str));
			SetDlgItemText(hDlg, IDC_STATIC_PCKSUM, str);
			GetMSGVerifiedDEAPIPacket(str, &packet, DEAPI_DATA, sizeof(str));
			SetDlgItemText(hDlg, IDC_STATIC_PDATA, str);
			break;
		case IDCANCEL:
		case IDCLOSE:			
			DestroyWindow(hDECMD);
			hDECMD = NULL;
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK CRCDlgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{	
	TCHAR str[MAX_PATH];
	TCHAR hexArr[MAX_PATH * 2];
	UCHAR index;
	INT cnt;
	UCHAR crc8;
	USHORT crc16;
	LONG crc32;
	switch (iMessage) {
	case WM_INITDIALOG:
		CheckDlgButton(hWnd, IDC_RADIO_CRC8, BST_CHECKED);
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_CDMA2000");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_DARC");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_DVB_S2");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_EBU");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_I_CODE");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_ITU");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_MAXIM");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_ROHC");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_WCDMA");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_AUTOSAR");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_BLUETOOTH");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_LTE");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_COMBO_CRCTYPE:
			switch (HIWORD(wParam)) {
			case CBN_SETFOCUS:
				SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_RESETCONTENT, 0, 0);
				if (IsDlgButtonChecked(hWnd, IDC_RADIO_CRC8) == TRUE) {
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_CDMA2000");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_DARC");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_DVB_S2");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_EBU");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_I_CODE");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_ITU");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_MAXIM");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_ROHC");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_WCDMA");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_AUTOSAR");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_BLUETOOTH");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC8_LTE");
				}
				else if (IsDlgButtonChecked(hWnd, IDC_RADIO_CRC16) == TRUE) {
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_CCITT_FALSE");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_ARC");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_AUR_CCITT");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_BUYPASS");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_CDMA2000");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_DDS_110");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_DECT_R");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_DECT_X");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_DNP");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_EN_13757");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_GENIBUS");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_MAXIM");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_MCRF4XX");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_RIELLO");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_T10_DIF");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_TELEDISK");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_TMS37157");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_USB");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC_A");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_KERMIT");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_MODBUS");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_X_25");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_XMODEM");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC16_TEST");
				}
				else if (IsDlgButtonChecked(hWnd, IDC_RADIO_CRC32) == TRUE) {
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32_BZIP2");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32_C");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32_D");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32_MPEG2");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32_POSIX");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32_Q");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32_JAMCRC");
					SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_ADDSTRING, 0, (LPARAM)"CRC32_XFER");					
				}
				break;
			}
			return TRUE;
		case IDOK:
			index = (UCHAR)SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_GETCURSEL, 0, 0);
			SendMessage(GetDlgItem(hWnd, IDC_COMBO_CRCTYPE), CB_GETLBTEXT, index, (LPARAM)str);
			if (IsDlgButtonChecked(hWnd, IDC_RADIO_CRC8) == TRUE) {
				index = GetCRCTypeIndex(str, 8);
				GetDlgItemText(hWnd, IDC_EDIT_PACKET, str, sizeof(str));
				cnt = AsciiHextoHarr(hexArr, str, lstrlen(str));
				crc8 = get_CRC8((UCHAR*)hexArr, cnt, index);
				wsprintf(str, "%02X", crc8);
				SetDlgItemText(hWnd, IDC_EDIT_CRC, str);
			}
			else if (IsDlgButtonChecked(hWnd, IDC_RADIO_CRC16) == TRUE) {
				index = GetCRCTypeIndex(str, 16);
				GetDlgItemText(hWnd, IDC_EDIT_PACKET, str, sizeof(str));
				cnt = AsciiHextoHarr(hexArr, str, lstrlen(str));
				crc16 = get_CRC16((UCHAR*)hexArr, cnt, index);
				wsprintf(str, "%04X", crc16);
				SetDlgItemText(hWnd, IDC_EDIT_CRC, str);
			}
			else if (IsDlgButtonChecked(hWnd, IDC_RADIO_CRC32) == TRUE) {
				index = GetCRCTypeIndex(str, 32);
				GetDlgItemText(hWnd, IDC_EDIT_PACKET, str, sizeof(str));
				cnt = AsciiHextoHarr(hexArr, str, lstrlen(str));
				crc32 = get_CRC32((UCHAR*)hexArr, cnt, index);
				wsprintf(str, "%08X", crc32);
				SetDlgItemText(hWnd, IDC_EDIT_CRC, str);
			}		
			break;
		case IDCANCEL:
		case IDCLOSE:
			DestroyWindow(hCRC);
			hCRC = NULL;
			break;
		}
		return TRUE;
	}
	return FALSE;
}

//void CALLBACK Timer1Proc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime){}

/*case WM_CREATE:return OnCreate(hWnd, wParam, lParam);
case WM_DESTROY:OnDestroy(hWnd, wParam, lParam); return 0;
case WM_COMMAND:OnCommand(hWnd, wParam, lParam); break;
case WM_TIMER:OnTimer(hWnd, wParam, lParam); return 0;
case WM_CLOSE:if (OnClose(hWnd, wParam, lParam)) return 0; break;
case WM_QUERYENDSESSION:if (OnQueryEndSession(hWnd, wParam, lParam)) return 0; break;
case WM_INITMENU:OnInitMenu(hWnd, wParam, lParam); return 0;
case WM_DROPFILES:OnDropFiles(hWnd, wParam, lParam); return 0;
case WM_FONTCHANGE:OnFontChange(hWnd, wParam, lParam); return 0;
case WM_ACTIVATEAPP:OnActivateApp(hWnd, wParam, lParam); return 0;
case WM_USER + 1:OnUser1(hWnd, wParam, lParam); return 0;
case WM_USER + 2:OnUser2(hWnd, wParam, lParam); return 0;
case WM_SIZE:if (OnSize(hWnd, wParam, lParam)) return 0; break;
case WM_SETCURSOR:if (OnSetCursor(hWnd, wParam, lParam)) return 0; break;
case WM_LBUTTONDOWN:OnLButtonDown(hWnd, wParam, lParam); return 0;
case WM_MOUSEMOVE:OnMouseMove(hWnd, wParam, lParam); return 0;
case WM_LBUTTONUP:OnLButtonUp(hWnd, wParam, lParam); return 0;
case WM_NOTIFY:OnNotify(hWnd, wParam, lParam); return 0;
case WM_SYSCOLORCHANGE:OnSysColorChange(hWnd, wParam, lParam); return 0;
case WM_USER + 3:OnUser3(hWnd, wParam, lParam); return 0;
case WM_USER + 4:OnUser4(hWnd, wParam, lParam); return 0;*/