#include <Windows.h>
#include "AnythingManager.h"
#include "resource.h"

// ������ Ʈ���� ó�� case �� ����
#define TRAY_NOTIFY	(WM_APP+100)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
// ���α׷� ��ư ������ �ڵ�
HWND hBtn1, hBtn2, hBtn3, hBtn4, hBtn5, hBtn6;
HWND hBtn7, hBtn8, hBtn9, hBtn10;
// ���α׷� ���� ǥ�� ����Ʈ â ������ �ڵ�
HWND hEdit1, hEdit2, hEdit3, hEdit4, hEdit5, hEdit6;
HWND hEdit7, hEdit8, hEdit9, hEdit10;
// �� Ʈ���� ������ ����ü
NOTIFYICONDATA nid;
//HWND hProgram;
LPCTSTR lpszClass = TEXT("�ƹ��ų� �Ŵ��� V_1_0_0");

static BOOL flagZOrder = 0; // ž ��Ʈ â �÷���
// ���α׷��� ���� ���� ����ü
static struct tagProgInfo
{
	HANDLE hProcess;
	HANDLE hThread;
	BOOL flagValid;
}processInfo[MAX_PATH];

INT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
INT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);

INT GetAvailableProcessHandleNum(struct tagProgInfo processInfo[])
{
	INT cnt;
	for (cnt = 0; cnt < MAX_PATH; cnt++) {
		if (processInfo[cnt].flagValid != TRUE) return cnt;
	}
	return -1;
}

INT FlushProcessInfo()
{
	INT cnt;
	INT cnt1 = 0;
	DWORD exitCode;
	for (cnt = 0; cnt < MAX_PATH; cnt++) {
		if (processInfo[cnt].flagValid == TRUE) {
			GetExitCodeProcess(processInfo[cnt].hProcess, &exitCode);
			if (exitCode != STILL_ACTIVE) processInfo[cnt].flagValid = FALSE;
			cnt1++;
		}
	}
	return cnt1;
}

// ���α׷� ��ư�� Ŭ���Ǿ����� �ش� ��Ʈ�� ó���Ѵ�.
INT ProcessBtnClick(HWND hWnd, HWND hEdit, TCHAR* path)
{
	INT index;
	STARTUPINFO si = { sizeof(STARTUPINFO), };
	PROCESS_INFORMATION pi;
	TCHAR str[MAX_PATH];
	index = GetAvailableProcessHandleNum(processInfo);
	if (index == -1) { MessageBox(hWnd, TEXT("���̻� ���μ��� ������ �Ұ��մϴ�."), TEXT("�˸�"), MB_OK); return -1; }
	if (CreateProcess(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) != TRUE) {
		MessageBox(hWnd, TEXT("���μ��� ���� ����\r\n���α׷� ���� ������ Ȯ���ϼ���."), TEXT("�˸�"), MB_OK);
		return -1;
	}
	WaitForInputIdle(pi.hProcess, 3000); // ���μ��� �����ð�(�ִ뼳���ð���)���� ���
	processInfo[index].hProcess = pi.hProcess;
	processInfo[index].hThread = pi.hThread;
	processInfo[index].flagValid = TRUE;
	wsprintf(str, "���μ����ڵ�:%d", pi.hProcess);
	SetWindowText(hEdit, str);
	return index;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	HACCEL hAccel;	
	HANDLE hFproc;
	hFproc = FindWindow("�ƹ��ų� �Ŵ��� V_1_0_0", NULL);
	if (hFproc != NULL) {
		MessageBox(NULL, TEXT("�̹� ���α׷��� �������Դϴ�."), "Notice", MB_OK);
		return 0;
	}
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	//WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hbrBackground = (HBRUSH)(COLOR_BTNSHADOW);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	//WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// ���ҽ� ������ ������ ������ ���� �⺻ ������ ���
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);	
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);		
	ShowWindow(hWnd, nCmdShow);

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage(&Message, NULL, 0, 0)) {
		if (!TranslateAccelerator(hWnd, hAccel, &Message)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}		
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{	
	DWORD exitCode;
	INT cnt;	
	HMENU hMenu, hPopupMenu;
	POINT pt;
	switch (iMessage) {
	case WM_CREATE: return OnCreate(hWnd, wParam, lParam);
	case WM_COMMAND: OnCommand(hWnd, wParam, lParam); break;
	case WM_PAINT: OnPaint(hWnd, wParam, lParam); return 0;
	case WM_CLOSE:	// ������ Ŭ�ο��� ��ư�� ������ �����찡 �������� Ʈ���̷� ����ȴ�.
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 0;
		nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		nid.uCallbackMessage = TRAY_NOTIFY;
		nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
		lstrcpy(nid.szTip, "�ƹ��ų� �Ŵ���");
		Shell_NotifyIcon(NIM_ADD, &nid);
		ShowWindow(hWnd, SW_HIDE);
		return 0;
	case TRAY_NOTIFY: // ������ Ʈ���� Ȱ��ȭ�� ���콺 ��ư Ŭ���� ���� ó��
		switch (lParam) {
		case WM_LBUTTONDOWN:
			ShowWindow(hWnd, SW_SHOW);
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = hWnd;
			nid.uID = 0;
			Shell_NotifyIcon(NIM_DELETE, &nid);
			break;
		case WM_RBUTTONDOWN:
			hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU1));
			hPopupMenu = GetSubMenu(hMenu, 0);
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON |
				TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
			SetForegroundWindow(hWnd);
			DestroyMenu(hPopupMenu);
			DestroyMenu(hMenu);			
			break;
		}
		return 0;
	case WM_DESTROY:
		// ���α׷��� �ݴ´�.
		for (cnt = 0; cnt < MAX_PATH; cnt++) {
			if (processInfo[cnt].flagValid == TRUE) {
				GetExitCodeProcess(processInfo[cnt].hProcess, &exitCode);
				if (exitCode == STILL_ACTIVE) TerminateProcess(processInfo[cnt].hProcess, 0);
				CloseHandle(processInfo[cnt].hProcess);
				CloseHandle(processInfo[cnt].hThread);
			}
		}
		// Ʈ���̿� ����� �����츦 �����Ѵ�.
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 0;
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{	
	hWndMain = hWnd;
	hBtn1 = CreateWindow(TEXT("button"), TEXT("�ܼ� command"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		BTN1_INIT_X, BTN1_INIT_Y, BTN1_INIT_W, BTN1_INIT_H, hWnd, (HMENU)1, g_hInst, NULL);
	hBtn2 = CreateWindow(TEXT("button"), TEXT("SHEditor(�޸���)"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN2_INIT_X, BTN2_INIT_Y, BTN2_INIT_W, BTN2_INIT_H, hWnd, (HMENU)2, g_hInst, NULL);
	hBtn3 = CreateWindow(TEXT("button"), TEXT("EnumProcess"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN3_INIT_X, BTN3_INIT_Y, BTN3_INIT_W, BTN3_INIT_H, hWnd, (HMENU)3, g_hInst, NULL);
	hBtn4 = CreateWindow(TEXT("button"), TEXT("�ι���������α׷�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN4_INIT_X, BTN4_INIT_Y, BTN4_INIT_W, BTN4_INIT_H, hWnd, (HMENU)4, g_hInst, NULL);
	hBtn5 = CreateWindow(TEXT("button"), TEXT("MySerialComm"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN5_INIT_X, BTN5_INIT_Y, BTN5_INIT_W, BTN5_INIT_H, hWnd, (HMENU)5, g_hInst, NULL);
	hBtn6 = CreateWindow(TEXT("button"), TEXT("DECOS_alpha_v1_0"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN6_INIT_X, BTN6_INIT_Y, BTN6_INIT_W, BTN6_INIT_H, hWnd, (HMENU)6, g_hInst, NULL);
	hBtn7 = CreateWindow(TEXT("button"), TEXT("���α׷� ����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN7_INIT_X, BTN7_INIT_Y, BTN7_INIT_W, BTN7_INIT_H, hWnd, (HMENU)7, g_hInst, NULL);
	hBtn8 = CreateWindow(TEXT("button"), TEXT("���α׷� ����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN8_INIT_X, BTN8_INIT_Y, BTN8_INIT_W, BTN8_INIT_H, hWnd, (HMENU)8, g_hInst, NULL);
	hBtn9 = CreateWindow(TEXT("button"), TEXT("���α׷� ����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN9_INIT_X, BTN9_INIT_Y, BTN9_INIT_W, BTN9_INIT_H, hWnd, (HMENU)9, g_hInst, NULL);
	hBtn10 = CreateWindow(TEXT("button"), TEXT("���α׷� ����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		BTN10_INIT_X, BTN10_INIT_Y, BTN10_INIT_W, BTN10_INIT_H, hWnd, (HMENU)10, g_hInst, NULL);
	hEdit1 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |  ES_LEFT | ES_READONLY,
		EDIT1_INIT_X, EDIT1_INIT_Y, EDIT1_INIT_W, EDIT1_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit2 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT2_INIT_X, EDIT2_INIT_Y, EDIT2_INIT_W, EDIT2_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit3 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT3_INIT_X, EDIT3_INIT_Y, EDIT3_INIT_W, EDIT3_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit4 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT4_INIT_X, EDIT4_INIT_Y, EDIT4_INIT_W, EDIT4_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit5 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT5_INIT_X, EDIT5_INIT_Y, EDIT5_INIT_W, EDIT5_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit6 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT6_INIT_X, EDIT6_INIT_Y, EDIT6_INIT_W, EDIT6_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit7 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT7_INIT_X, EDIT7_INIT_Y, EDIT7_INIT_W, EDIT7_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit8 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT8_INIT_X, EDIT8_INIT_Y, EDIT8_INIT_W, EDIT8_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit9 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT9_INIT_X, EDIT9_INIT_Y, EDIT9_INIT_W, EDIT9_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	hEdit10 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
		EDIT10_INIT_X, EDIT10_INIT_Y, EDIT10_INIT_W, EDIT10_INIT_H, hWnd, (HMENU)0, g_hInst, NULL);
	MoveWindow(hWnd, 0, 0, WND_INITSIZE_W, WND_INITSIZE_H, TRUE);	
	return 0;
}

INT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT point;	
	TCHAR path[MAX_PATH] = "C:\\Users\\sohno\\source\\repos\\MyAPIProgram\\Release";
	TCHAR str[MAX_PATH];
	switch (LOWORD(wParam))
	{
	case 1: // button1
		strcpy_s(path, "cmd.exe"); // ������ command �ܼ� ���α׷�
		ProcessBtnClick(hWnd, hEdit1, path);		
		break;
	case 2: // button2
		//GetCurrentDirectory(sizeof(path), path);
		lstrcat(path, TEXT("\\SHEditor.exe")); // �޸��� ž��Ʈ ����
		ProcessBtnClick(hWnd, hEdit2, path);		
		break;
	case 3: // button3
		//GetCurrentDirectory(sizeof(path), path);
		lstrcat(path, TEXT("\\EnumProcess.exe")); // ���μ��� ���� ���α׷�
		ProcessBtnClick(hWnd, hEdit3, path);		
		break;
	case 4: // button4
		//GetCurrentDirectory(sizeof(path), path);
		lstrcat(path, TEXT("\\InverterComm.exe")); // �ι��� ��� ���α׷�
		ProcessBtnClick(hWnd, hEdit4, path);
		break;
	case 5: //
			//GetCurrentDirectory(sizeof(path), path);
		lstrcat(path, TEXT("\\MySerialComm.exe")); // �ø��� ��� ���α׷�
		ProcessBtnClick(hWnd, hEdit5, path);
		break;
	case 6: //
		lstrcat(path, TEXT("\\DECOS_alpha.exe")); // �ø��� ��� ���α׷�
		ProcessBtnClick(hWnd, hEdit6, path);
		break;
	case 7: //
		break;
	case 8: //
		break;
	case 9: //
		break;
	case 10: //
		break;
	case ID_FILE_MENU1:
		MessageBox(hWnd, TEXT("�޴�1�� ����"), TEXT("�˸�"), MB_OK);
		break;
	case ID_FILE_MENU2:
		MessageBox(hWnd, TEXT("�޴�2�� ����"), TEXT("�˸�"), MB_OK);
		break;
	case ID_FILE_EXIT: // ������ ����
		SendMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case ID_VIEW_MOVETOORIGIN: // �����츦 0,0 �������� �̵�
		MoveWindow(hWnd, 0, 0, WND_INITSIZE_W, WND_INITSIZE_H, TRUE);
		break;
	case ID_VIEW_MOVETOCURSOR: // �����츦 ���� Ŀ�� ��ġ�� �̵�
		if(!GetCursorPos(&point)) break;
		MoveWindow(hWnd, point.x, point.y, WND_INITSIZE_W, WND_INITSIZE_H, TRUE);
		break;
	case ID_VIEW_TOPMOST: // ������ ��ġ�� �׻� ž���� ����
		if (flagZOrder == 0) {
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);			
			strcpy_s(str, lpszClass);
			strcat_s(str, TEXT("(TopMost)"));
			SetWindowText(hWndMain, str);
			flagZOrder = TRUE;
		}
		else {
			SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowText(hWndMain, lpszClass);
			flagZOrder = FALSE;
		}

		break;
	case ID_HELP_INFO:
		MessageBox(hWnd, TEXT("�޴����� ����"), TEXT("�˸�"), MB_OK);
		break;	
	default:
		break;
	}
	return 0;
}

INT OnPaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
	return 0;
}

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
