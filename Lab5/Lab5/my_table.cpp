#include "my_table.h"
#include "resource.h" // Потрібно додати нові ID ресурсів
#include <commctrl.h> // Для ListView

// Ініціалізація статичних членів
MyTable* MyTable::p_instance = nullptr;
HWND MyTable::hDialog = NULL;

MyTable* MyTable::getInstance() {
    if (!p_instance) {
        p_instance = new MyTable();
    }
    return p_instance;
}

void MyTable::destroyInstance() {
    if (p_instance) {
        delete p_instance;
        p_instance = nullptr;
    }
}

HWND MyTable::GetDialogHandle() {
    return hDialog;
}

void MyTable::Show(HINSTANCE hInst, HWND hWndParent) {
    if (!hDialog) {
        // Створюємо немодальне вікно, якщо воно ще не існує 
        hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TABLE_VIEW), hWndParent, DialogProc);
        ShowWindow(hDialog, SW_SHOW);
    }
    else {
        // Якщо вікно вже є, просто виводимо його на передній план
        SetForegroundWindow(hDialog);
    }
}

void MyTable::Add(const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2) {
    if (!hDialog) return;

    HWND hListView = GetDlgItem(hDialog, IDC_SHAPE_LIST);
    if (!hListView) return;

    // Отримуємо поточну кількість рядків, щоб додати новий в кінець
    int itemCount = ListView_GetItemCount(hListView);

    LVITEM lvi = { 0 };
    lvi.mask = LVIF_TEXT;
    lvi.iItem = itemCount;
    lvi.iSubItem = 0;
    lvi.pszText = (LPWSTR)shapeName;
    ListView_InsertItem(hListView, &lvi);

    // Заповнюємо стовпці з координатами
    wchar_t buffer[20];
    wsprintf(buffer, L"%ld", x1);
    ListView_SetItemText(hListView, itemCount, 1, buffer);
    wsprintf(buffer, L"%ld", y1);
    ListView_SetItemText(hListView, itemCount, 2, buffer);
    wsprintf(buffer, L"%ld", x2);
    ListView_SetItemText(hListView, itemCount, 3, buffer);
    wsprintf(buffer, L"%ld", y2);
    ListView_SetItemText(hListView, itemCount, 4, buffer);
}

INT_PTR CALLBACK MyTable::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG: {
        HWND hListView = GetDlgItem(hDlg, IDC_SHAPE_LIST);
        // Встановлюємо розширені стилі для таблиці
        ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        // Створюємо стовпці таблиці 
        LVCOLUMN lvc;
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.fmt = LVCFMT_LEFT;

        const wchar_t* headers[] = { L"Назва", L"x1", L"y1", L"x2", L"y2" };
        int widths[] = { 120, 70, 70, 70, 70 };
        for (int i = 0; i < 5; ++i) {
            lvc.iSubItem = i;
            lvc.cx = widths[i];
            lvc.pszText = (LPWSTR)headers[i];
            ListView_InsertColumn(hListView, i, &lvc);
        }
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
        // Обробка кнопки "Закрити"
        if (LOWORD(wParam) == IDCANCEL) {
            DestroyWindow(hDlg);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hDlg);
        break;
    case WM_DESTROY:
        // При знищенні вікна обнуляємо глобальний дескриптор
        hDialog = NULL;
        break;
    }
    return (INT_PTR)FALSE;
}