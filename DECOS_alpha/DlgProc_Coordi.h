#pragma once
#include <Windows.h>

// 장치 모델리스 대화상자 메시지 프로시저
BOOL CALLBACK CoordiDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
// 중계기 하위 모달 대화상자 메시지 프로시저
BOOL CALLBACK InsertFileDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

VOID SetInvPhaseCombobox(HWND hDlg, INT indexType, INT indexPhase);