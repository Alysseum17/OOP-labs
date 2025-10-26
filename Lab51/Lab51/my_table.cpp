#include "my_table.h"
#include "resource.h" 
#include <wchar.h>

static MyTable* p_instance = nullptr;

MyTable::MyTable() {
    p_instance = this;
}

void MyTable::Show(HINSTANCE hInst, HWND hWndParent) {
    if (!m_hDlg) {
        m_hDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TABLE_VIEW), hWndParent, DlgProc);
        ShowWindow(m_hDlg, SW_SHOW);
    }
    else {
        SetFocus(m_hDlg);
    }
}

void MyTable::AddEntry(const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2) {
    if (m_hDlg) {
        wchar_t buffer[256];
        swprintf_s(buffer, 256, L"%s\t%ld\t%ld\t%ld\t%ld", shapeName, x1, y1, x2, y2);
        SendDlgItemMessage(m_hDlg, IDC_SHAPE_LIST, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)buffer);
    }
}

void MyTable::RemoveEntry(int index) {
    if (m_hDlg) {
        SendDlgItemMessage(m_hDlg, IDC_SHAPE_LIST, LB_DELETESTRING, (WPARAM)index, 0);
    }
}


void MyTable::Clear() {
    if (m_hDlg) {
        SendDlgItemMessage(m_hDlg, IDC_SHAPE_LIST, LB_RESETCONTENT, 0, 0);
    }
}

void MyTable::RegisterCallback(TableCallback callback) {
    m_callback = callback;
}

INT_PTR CALLBACK MyTable::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
    {
        int tabs[] = { 100, 140, 180, 220 };
        SendDlgItemMessage(hDlg, IDC_SHAPE_LIST, LB_SETTABSTOPS, 4, (LPARAM)tabs);

        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_SHAPE_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE && p_instance->m_callback) {
                int selectedIndex = SendDlgItemMessage(hDlg, IDC_SHAPE_LIST, LB_GETCURSEL, 0, 0);
                p_instance->m_callback(TABLE_ACTION_SELECT, selectedIndex);
            }
            break;
        case IDC_DELETE_BUTTON:
            if (p_instance->m_callback) {
                int selectedIndex = SendDlgItemMessage(hDlg, IDC_SHAPE_LIST, LB_GETCURSEL, 0, 0);
                if (selectedIndex != LB_ERR) {
                    p_instance->m_callback(TABLE_ACTION_DELETE, selectedIndex);
                }
            }
            break;
        case IDOK:
        case IDCANCEL:
            DestroyWindow(hDlg);
            break;
        }
        return (INT_PTR)TRUE;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        return (INT_PTR)TRUE;

    case WM_DESTROY:
        p_instance->m_hDlg = NULL;
        if (p_instance->m_callback) {
            p_instance->m_callback(TABLE_ACTION_CLOSE, -1);
        }
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}