#include <windows.h>

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass=TEXT("EnumProcess");

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
	  ,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT, 500,800,
		NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);

	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

// PSAPI 사용
#include "psapi.h"
HWND hListProc;
void MyEnumProcess()
{
	DWORD arProc[1024],cb;
	int nProc;
	int i;
	TCHAR str[256];
	HMODULE hModule;
	HANDLE hProcess;
	TCHAR FullPath[MAX_PATH]={0,};
	BOOL Result;

	// 프로세스의 목록을 배열에 구하고 개수를 계산한다.
	EnumProcesses(arProc,sizeof(arProc),&cb);
	nProc=cb/sizeof(DWORD);
	SendMessage(hListProc,LB_RESETCONTENT,0,0);
	for (i=0;i<nProc;i++) {
		hProcess=OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,FALSE,arProc[i]);
		if (hProcess) {
			// 첫번째 모듈(=프로세스 그 자체)의 이름을 구해 출력한다.
			Result=EnumProcessModules(hProcess,&hModule,sizeof(hModule),&cb);
			if (Result==0) {
				wsprintf(str,"EnumProcessModules - Error %d",GetLastError());
			} else {
				GetModuleBaseName(hProcess,hModule,FullPath,sizeof(FullPath));
				wsprintf(str,"%5d - %s",arProc[i],FullPath);
			}
			CloseHandle(hProcess);
		} else {
			wsprintf(str,"OpenProcess - Error %d",GetLastError());
		}
		SendMessage(hListProc,LB_ADDSTRING,0,(LPARAM)str);
	}
}

// 툴헬프 라이브러리 사용
#include "tlhelp32.h"
void MyEnumProcess98()
{
	HANDLE hSnap;
	PROCESSENTRY32 pe;
	TCHAR str[256];

	hSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hSnap == (HANDLE)-1)
		return;

	pe.dwSize=sizeof(PROCESSENTRY32);
	SendMessage(hListProc,LB_RESETCONTENT,0,0);
	if (Process32First(hSnap,&pe)) {
		do {
			wsprintf(str,"%5d - %s",pe.th32ProcessID, pe.szExeFile);
			SendMessage(hListProc,LB_ADDSTRING,0,(LPARAM)str);
		} while (Process32Next(hSnap,&pe));
	}
	CloseHandle(hSnap);
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR *Mes="왼쪽 버튼 : 프로세스 열거, 오른쪽 버튼 : 스냅샷 방법";
	TCHAR str[MAX_PATH];

	switch (iMessage) {
	case WM_CREATE:
		hListProc=CreateWindow("listbox",NULL,WS_CHILD | WS_VISIBLE | 
			WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
			10,100,300,600,hWnd,(HMENU)0,g_hInst,NULL);
		hWndMain=hWnd;
		return 0;
	case WM_LBUTTONDOWN:
		MyEnumProcess();
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_RBUTTONDOWN:
		MyEnumProcess98();
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_MBUTTONDOWN:
		SendMessage(hListProc, LB_RESETCONTENT, 0, 0);
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		wsprintf(str, "열거된 프로세스 갯수 : %d", (int)SendMessage(hListProc, LB_GETCOUNT, 0, 0));
		TextOut(hdc,10,10,Mes,lstrlen(Mes));
		TextOut(hdc, 10, 30, str, lstrlen(str));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}
