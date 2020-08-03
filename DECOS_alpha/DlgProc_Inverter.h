#pragma once
#include <Windows.h>
#include "resource.h"

BOOL CALLBACK InverterDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

INT GetInvPortNum();

BOOL GetFlagIDScan();

INT SetInvSerialBuff(const TCHAR *src, UINT cbSrc);