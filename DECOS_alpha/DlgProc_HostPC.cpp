#include "DlgProc_HostPC.h"
#include <VersionHelpers.h>
#include "resource.h"

#define ENV_VAR_STRING_COUNT  (sizeof(envVarStrings)/sizeof(TCHAR*))
#define INFO_BUFFER_SIZE 32767

const TCHAR* envVarStrings[] =
{
	TEXT("OS         = %OS%"),
	TEXT("PATH       = %PATH%"),
	TEXT("HOMEPATH   = %HOMEPATH%"),
	TEXT("TEMP       = %TEMP%")
};

extern HWND hCRT;

static INT arFunc[] = {0, 10, 12, 10, 12, 10, 24, 7, 8, 9, 10, 11, 12};

INT OnDlgInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);
INT OnDlgPaint(HWND hDlg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

BOOL CALLBACK HostPCDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	INT index;
	switch (iMessage) {
	case WM_INITDIALOG:	return OnDlgInitDialog(hDlg, wParam, lParam);
	case WM_MOUSEWHEEL:
		index = (INT)wParam;
		if (index > 0) {
			if (LOWORD(wParam) && MK_SHIFT) SendMessage(hCRT, WM_HSCROLL, (WPARAM)LOWORD(SB_LINELEFT), 0);
			else SendMessage(hCRT, WM_VSCROLL, (WPARAM)LOWORD(SB_LINEUP), 0);
		}
		else if (index < 0) {
			if (LOWORD(wParam) && MK_SHIFT) SendMessage(hCRT, WM_HSCROLL, (WPARAM)LOWORD(SB_LINERIGHT), 0);
			else SendMessage(hCRT, WM_VSCROLL, (WPARAM)LOWORD(SB_LINEDOWN), 0);
		}
		return TRUE;
	case WM_COMMAND:		
		return TRUE;
	//case WM_PAINT: return OnDlgPaint(hDlg, wParam, lParam);
	}
	return FALSE;
}

INT OnDlgInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	SYSTEM_INFO si;	
	TCHAR strInfo[50];
	INT strLength;
	HWND hEditSpec;
	RECT rcVirt;

	TCHAR  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;

	GetSystemInfo(&si);
	wsprintf(infoBuf, "프로세서 갯수 : %d 개 \r\n", si.dwNumberOfProcessors);
	hEditSpec = GetDlgItem(hDlg, IDC_EDIT_PCSPEC);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	switch (si.wProcessorArchitecture) {  // 프로세서 아키텍처 조사
	case PROCESSOR_ARCHITECTURE_ALPHA: lstrcpy(strInfo, "Alpha"); break;
	case PROCESSOR_ARCHITECTURE_ALPHA64: lstrcpy(strInfo, "Alpha64"); break;
	case PROCESSOR_ARCHITECTURE_AMD64: lstrcpy(strInfo, "AMD64"); break;
	case PROCESSOR_ARCHITECTURE_ARM: lstrcpy(strInfo, "ARM"); break;
	case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64: lstrcpy(strInfo, "ARM32_ON_WIN64"); break;
	case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64: lstrcpy(strInfo, "IA32_ON_ARM64"); break;
	case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64: lstrcpy(strInfo, "IA32_ON_WIN64"); break;
	case PROCESSOR_ARCHITECTURE_IA64: lstrcpy(strInfo, "IA64"); break;
	case PROCESSOR_ARCHITECTURE_INTEL: lstrcpy(strInfo, "INTEL"); break;
	case PROCESSOR_ARCHITECTURE_MIPS: lstrcpy(strInfo, "MIPS"); break;
	case PROCESSOR_ARCHITECTURE_MSIL: lstrcpy(strInfo, "MSIL"); break;
	case PROCESSOR_ARCHITECTURE_NEUTRAL: lstrcpy(strInfo, "NEUTRAL"); break;
	case PROCESSOR_ARCHITECTURE_PPC: lstrcpy(strInfo, "PPC"); break;
	case PROCESSOR_ARCHITECTURE_SHX: lstrcpy(strInfo, "SHX"); break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN: lstrcpy(strInfo, "UNKNOWN"); break;
	}
	wsprintf(infoBuf, "프로세서 아키텍처 : %s \r\n", strInfo);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	switch (si.dwProcessorType) { // 프로세서 타입 조사
	case PROCESSOR_ALPHA_21064: lstrcpy(strInfo, "Alpha_21064"); break;
	case PROCESSOR_AMD_X8664: lstrcpy(strInfo, "AMD_X8664"); break;
	case PROCESSOR_ARM720: lstrcpy(strInfo, "ARM720"); break;
	case PROCESSOR_ARM820: lstrcpy(strInfo, "ARM820"); break;
	case PROCESSOR_ARM920: lstrcpy(strInfo, "ARM920"); break;
	case PROCESSOR_ARM_7TDMI: lstrcpy(strInfo, "ARM_7TDMI"); break;
	case PROCESSOR_HITACHI_SH3: lstrcpy(strInfo, "HITACHI_SH3"); break;
	case PROCESSOR_HITACHI_SH3E: lstrcpy(strInfo, "HITACHI_SH3E"); break;
	case PROCESSOR_HITACHI_SH4: lstrcpy(strInfo, "HITACHI_SH4"); break;
	case PROCESSOR_INTEL_386: lstrcpy(strInfo, "INTEL_386"); break;
	case PROCESSOR_INTEL_486: lstrcpy(strInfo, "INTEL_486"); break;
	case PROCESSOR_INTEL_IA64: lstrcpy(strInfo, "INTEL_IA64"); break;
	case PROCESSOR_INTEL_PENTIUM: lstrcpy(strInfo, "INTEL_PENTIUM"); break;
	case PROCESSOR_MIPS_R4000: lstrcpy(strInfo, "MIPS_R4000"); break;
	case PROCESSOR_MOTOROLA_821: lstrcpy(strInfo, "MOTOROLA_821"); break;
	case PROCESSOR_OPTIL: lstrcpy(strInfo, "OPTIL"); break;
	case PROCESSOR_PPC_601: lstrcpy(strInfo, "PPC_601"); break;
	case PROCESSOR_PPC_603: lstrcpy(strInfo, "PPC_603"); break;
	case PROCESSOR_PPC_604: lstrcpy(strInfo, "PPC_604"); break;
	case PROCESSOR_PPC_620: lstrcpy(strInfo, "PPC_620"); break;
	case PROCESSOR_SHx_SH3: lstrcpy(strInfo, "SHx_SH3"); break;
	case PROCESSOR_SHx_SH4: lstrcpy(strInfo, "SHx_SH3"); break;
	case PROCESSOR_STRONGARM: lstrcpy(strInfo, "STRONGARM"); break;
	}
	wsprintf(infoBuf, "프로세서 종류 : %s \r\n", strInfo);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	switch (si.wProcessorLevel) { // 프로세서 레벨 조사
	case 3: lstrcpy(strInfo, "80386"); break;
	case 4: lstrcpy(strInfo, "80486"); break;
	case 5: lstrcpy(strInfo, "Pentium"); break;
	case 6: lstrcpy(strInfo, "Pentium Pro or Pentium 2"); break;
	default: lstrcpy(strInfo, "기타"); break;
	}
	wsprintf(infoBuf, "프로세서 레벨 : %s \r\n", strInfo);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	wsprintf(infoBuf, "프로세서 리비전 : %x \r\n", si.wProcessorRevision);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	switch (GetKeyboardType(0)) { // 키보드 타입 조사
	case 1: lstrcpy(strInfo, "IBM PC/XT or compatible(83 key)"); break;
	case 2: lstrcpy(strInfo, "Olivetti ICO(102 key)"); break;
	case 3: lstrcpy(strInfo, "IBM PC/AT (84 key)"); break;
	case 4: lstrcpy(strInfo, "IBM enhanced (101 or 102 key)"); break;
	case 5: lstrcpy(strInfo, "Nokia 1050"); break;
	case 6: lstrcpy(strInfo, "Nokia 9140"); break;
	case 7: lstrcpy(strInfo, "일본 키보드"); break;
	default: lstrcpy(strInfo, "기타 키보드"); break;
	}
	wsprintf(infoBuf, "키보드 타입 : %s, 서브타입_%d, 펑션키_%d 개 \r\n", strInfo, GetKeyboardType(1), arFunc[GetKeyboardType(2)]);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	// 컴퓨터 이름 조사
	strLength = sizeof(strInfo);
	GetComputerName(strInfo, (LPDWORD)&strLength);
	wsprintf(infoBuf, "컴퓨터 이름 : %s \r\n", strInfo);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	// 사용자 이름 조사
	strLength = sizeof(strInfo);
	GetUserName(strInfo, (LPDWORD)&strLength);
	wsprintf(infoBuf, "사용자 이름 : %s \r\n", strInfo);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	// 모니터 정보
	wsprintf(infoBuf, "모니터 갯수 : %d, \t색상 포맷 : %s \r\n",
		GetSystemMetrics(SM_CMONITORS), GetSystemMetrics(SM_SAMEDISPLAYFORMAT) ? TEXT("모두동일") : TEXT("다름"));
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	rcVirt.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	rcVirt.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
	rcVirt.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	rcVirt.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	wsprintf(infoBuf, "가상화면 : (%d, %d) ~ (%d, %d) \r\n", rcVirt.left, rcVirt.top, rcVirt.right, rcVirt.bottom);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)hEditSpec);
	// OS 종류
	bufCharCount = ExpandEnvironmentStrings(envVarStrings[0], infoBuf,
		INFO_BUFFER_SIZE);
	if (IsWindowsXPOrGreater()) strcpy_s(strInfo, "\tWindows XP 이상 버전\r\n");
	if (IsWindowsXPSP1OrGreater()) strcpy_s(strInfo, "\tWindows XP SP1 이상 버전\r\n");
	if (IsWindowsXPSP2OrGreater()) strcpy_s(strInfo, "\tWindows XP SP2 이상 버전\r\n");
	if (IsWindowsXPSP3OrGreater()) strcpy_s(strInfo, "\tWindows XP SP3 이상 버전\r\n");
	if (IsWindowsVistaOrGreater()) strcpy_s(strInfo, "\tWindows Vista 이상 버전\r\n");
	if (IsWindowsVistaSP1OrGreater()) strcpy_s(strInfo, "\tWindows Vista SP1 이상 버전\r\n");
	if (IsWindowsVistaSP2OrGreater()) strcpy_s(strInfo, "\tWindows Vista SP2 이상 버전\r\n");
	if (IsWindows7OrGreater()) strcpy_s(strInfo, "\tWindows 7 이상 버전\r\n");
	if (IsWindows7SP1OrGreater()) strcpy_s(strInfo, "\tWindows 7 SP1 이상 버전\r\n");
	if (IsWindows8OrGreater()) strcpy_s(strInfo, "\tWindows 8 이상 버전\r\n");
	if (IsWindows8Point1OrGreater()) strcpy_s(strInfo, "\tWindows 8.1 이상 버전\r\n");
	if (IsWindows10OrGreater()) strcpy_s(strInfo, "\tWindows 10 이상 버전\r\n");
	if (IsWindowsServer()) strcpy_s(strInfo, "\tWindows Server \r\n");	
	strcat_s(infoBuf, strInfo);
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	// 환경변수 경로
	bufCharCount = ExpandEnvironmentStrings(envVarStrings[1], infoBuf,
		INFO_BUFFER_SIZE);
	strcat_s(infoBuf, "\r\n");
	strLength = GetWindowTextLength(hEditSpec);	
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	// 사용자  홈 경로
	bufCharCount = ExpandEnvironmentStrings(envVarStrings[2], infoBuf,
		INFO_BUFFER_SIZE);
	strcat_s(infoBuf, "\r\n");
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	// 사용자 임시 폴더 경로
	bufCharCount = ExpandEnvironmentStrings(envVarStrings[3], infoBuf,
		INFO_BUFFER_SIZE);
	strcat_s(infoBuf, "\r\n");
	strLength = GetWindowTextLength(hEditSpec);
	SendMessage(hEditSpec, EM_SETSEL, strLength, strLength);
	SendMessage(hEditSpec, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)infoBuf);
	return TRUE;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	MONITORINFOEX mi;
	TCHAR sInfo[256];
	INT strLength;
	static INT cntMon;

	mi.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &mi);
	wsprintf(sInfo, TEXT("모니터 %d번 핸들 : %x, 좌표 : (%d, %d) ~ (%d, %d) %s \r\n"),
		cntMon, hMonitor, lprcMonitor->left, lprcMonitor->top, lprcMonitor->right, lprcMonitor->bottom, 
		(mi.dwFlags&MONITORINFOF_PRIMARY)==0?"":"주 모니터");
	strLength = GetWindowTextLength((HWND)dwData);
	SendMessage((HWND)dwData, EM_SETSEL, strLength, strLength);
	SendMessage((HWND)dwData, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)sInfo);	
	cntMon++;
	return TRUE;
}

INT OnDlgPaint(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	//HDC hdc;
	PAINTSTRUCT ps;
	EndPaint(hDlg, &ps);
	return TRUE;
}
