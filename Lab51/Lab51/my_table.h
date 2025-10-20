#pragma once
#include <windows.h>

enum TableAction {
    TABLE_ACTION_SELECT,
    TABLE_ACTION_DELETE,
    TABLE_ACTION_CLOSE
};

typedef void (*TableCallback)(TableAction action, int index);

class MyTable {
public:
    MyTable();
    void Show(HINSTANCE hInst, HWND hWndParent);
    void AddEntry(const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2);
    void RemoveEntry(int index);
    void Clear();
    void RegisterCallback(TableCallback callback);

private:
    HWND m_hDlg = NULL;
    TableCallback m_callback = nullptr;
    static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};