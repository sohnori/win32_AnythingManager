#pragma once
#include <Windows.h>

// ��ġ �𵨸��� ��ȭ���� �޽��� ���ν���
BOOL CALLBACK CoordiDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
// �߰�� ���� ��� ��ȭ���� �޽��� ���ν���
BOOL CALLBACK InsertFileDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

VOID SetInvPhaseCombobox(HWND hDlg, INT indexType, INT indexPhase);