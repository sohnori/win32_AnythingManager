#include <Windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass=TEXT("SHEditor");

// 함수 원형
void ChangeCaption(TCHAR *Path);
int ConfirmSave();
BOOL Save();
BOOL SaveAs();
BOOL SaveToFile(TCHAR *Path);
void Open();

static BOOL flagZOrder = 0;

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
	  ,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)(COLOR_WINDOW);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,"SHEditor-제목없음.txt",WS_OVERLAPPEDWINDOW,
		0,0,500,400,
		NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);


	HACCEL hAccel=LoadAccelerators(hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));
	while (GetMessage(&Message,NULL,0,0)) {
		if (!TranslateAccelerator(hWnd,hAccel,&Message)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
	return (int)Message.wParam;
}

#define IDC_EDIT 100
#define MAXBUFFER 1048576
HWND hEdit;
TCHAR NowFile[MAX_PATH]="제목없음.txt";
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	DWORD Start,End;
	RECT rt;
	switch (iMessage) {
	case WM_CREATE:
		hWndMain=hWnd;
		GetClientRect(hWnd, &rt);
		hEdit=CreateWindowEx(NULL,"edit",NULL,WS_CHILD | WS_VISIBLE 
			| ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_HSCROLL | WS_VSCROLL,
			5,5,rt.right-10,rt.bottom-10,hWnd,(HMENU)IDC_EDIT,g_hInst,NULL);
		SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)MAXBUFFER,0);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_EDIT_UNDO:
			SendMessage(hEdit, EM_UNDO,0,0);
			return 0;
		case IDM_EDIT_CUT:
			SendMessage(hEdit, WM_CUT,0,0);
			break;
		case IDM_EDIT_COPY:
			SendMessage(hEdit, WM_COPY,0,0);
			break;
		case IDM_EDIT_PASTE:
			SendMessage(hEdit, WM_PASTE,0,0);
			break;
		case IDM_EDIT_DEL:
			SendMessage(hEdit, WM_CLEAR,0,0);
			break;
		case IDM_EDIT_SELALL:
			SendMessage(hEdit, EM_SETSEL,0,-1);
			break;
		case IDM_FILE_NEW:
			if (ConfirmSave() == IDCANCEL) {
				break;
			}
			SetWindowText(hEdit,"");
			ChangeCaption("제목없음");
			break;
		case IDM_FILE_LOAD:
			Open();
			break;
		case IDM_FILE_SAVE:
			Save();
			break;
		case IDM_FILE_SAVEAS:
			SaveAs();
			break;
		case IDM_FILE_EXIT:
			SendMessage(hWnd,WM_CLOSE,0,0);
			break;
		case IDM_VIEW_ZORDER:
			if (flagZOrder == 0) {
				SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				flagZOrder = TRUE;
			}
			else {
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				flagZOrder = FALSE;
			}
			
			break;
		case IDM_HELP_ABOUT:
			MessageBox(hWnd,"간단한 메모장입니다","소개",MB_OK);
			break;
		case IDC_EDIT:
			switch (HIWORD(wParam)) {
			case EN_MAXTEXT:
				MessageBox(hWnd, "입력 문자수 초과","에러",MB_OK);
				break;
			}
		}
		return 0;
	case WM_INITMENU:
		// 취소가 가능한가에 따라 Undo 메뉴 항목을 사용금지/허가한다.
		if (SendMessage(hEdit, EM_CANUNDO,0,0)==TRUE)
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_UNDO,MF_ENABLED | MF_BYCOMMAND);
		else 
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_UNDO,MF_GRAYED | MF_BYCOMMAND);

		// 선택 영역이 없으면 Cut, Copy, Clear 메뉴 항목은 사용할 수 없다.
		SendMessage(hEdit,EM_GETSEL, (WPARAM)&Start, (LPARAM)&End);
		if (Start != End) {
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_CUT,MF_ENABLED | MF_BYCOMMAND);
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_COPY,MF_ENABLED | MF_BYCOMMAND);
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_DEL,MF_ENABLED | MF_BYCOMMAND);
		}
		else {
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_CUT,MF_GRAYED | MF_BYCOMMAND);
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_COPY,MF_GRAYED | MF_BYCOMMAND);
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_DEL,MF_GRAYED | MF_BYCOMMAND);
		}

		// 클립보드에 텍스트 자료가 없으면 Paste 항목은 사용할 수 없다.
		if (IsClipboardFormatAvailable(CF_TEXT)) 
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_PASTE,MF_ENABLED | MF_BYCOMMAND);
		else 
			EnableMenuItem(GetMenu(hWnd),IDM_EDIT_PASTE,MF_GRAYED | MF_BYCOMMAND);
		return 0;
	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if (ConfirmSave() == IDCANCEL) {
			return TRUE;
		} else {
			break;
		}
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam),TRUE);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

void ChangeCaption(TCHAR *Path)
{
	TCHAR Cap[MAX_PATH+64];

	lstrcpy(NowFile,Path);
	wsprintf(Cap,"SHEditor - %s",Path);
	SetWindowText(hWndMain,Cap);
}

int ConfirmSave()
{
	int result=IDNO;
	TCHAR Mes[MAX_PATH+64];

	if (SendMessage(hEdit,EM_GETMODIFY,0,0)==TRUE) {
		wsprintf(Mes,"%s 파일이 변경되었습니다. 저장하시겠습니까?",NowFile);
		result=MessageBox(hWndMain,Mes,"알림",MB_YESNOCANCEL);
		if (result == IDCANCEL) {
			return IDCANCEL;
		}
		if (result == IDYES) {
			if (Save() == FALSE)
				return IDCANCEL;
			else 
				return IDYES;
		}
	}
	return result;
}

BOOL Save()
{
	if (lstrcmp(NowFile,"제목없음.txt")==0) {
		return SaveAs();
	} else {
		return SaveToFile(NowFile);
	}
}

BOOL SaveAs()
{
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH]="";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner=hWndMain;
	OFN.lpstrFilter="Text File\0*.txt;*.doc\0Every File(*.*)\0*.*\0";
	OFN.lpstrFile=lpstrFile;
	OFN.nMaxFile=MAX_PATH;
	OFN.lpstrDefExt="txt";
	if (GetSaveFileName(&OFN)==FALSE) {
		return FALSE;
	}
	ChangeCaption(OFN.lpstrFile);
	return SaveToFile(NowFile);
}

BOOL SaveToFile(TCHAR *Path)
{
	HANDLE hFile;
	DWORD dwWritten;
	int length;
	TCHAR *buf;

	hFile=CreateFile(Path,GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	length=GetWindowTextLength(hEdit);
	buf=(TCHAR *)malloc(length+1);
	GetWindowText(hEdit,buf,length+1);
	WriteFile(hFile,buf,length,&dwWritten,NULL);
	CloseHandle(hFile);
	free(buf);
	SendMessage(hEdit,EM_SETMODIFY,(WPARAM)FALSE,0);

	return TRUE;
}

void Open()
{
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH]="";
	HANDLE hFile;
	DWORD dwRead;
	DWORD size;
	TCHAR *buf;

	if (ConfirmSave() == IDCANCEL) {
		return;
	}
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner=hWndMain;
	OFN.lpstrFilter="Text File\0*.txt;*.doc\0Every File(*.*)\0*.*\0";
	OFN.lpstrFile=lpstrFile;
	OFN.nMaxFile=MAX_PATH;
	OFN.lpstrDefExt="txt";
	if (GetOpenFileName(&OFN)==FALSE) {
		return;
	}

	hFile=CreateFile(OFN.lpstrFile,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		MessageBox(hWndMain,"파일을 열 수 없습니다.","에러",MB_OK);
		return;
	}

	size=GetFileSize(hFile,NULL);
	buf=(TCHAR *)calloc(size+1,1);
	ReadFile(hFile,buf,size,&dwRead,NULL);
	CloseHandle(hFile);
	SetWindowText(hEdit,buf);
	free(buf);

	SendMessage(hEdit,EM_SETMODIFY,(WPARAM)FALSE,0);
	ChangeCaption(OFN.lpstrFile);
}
