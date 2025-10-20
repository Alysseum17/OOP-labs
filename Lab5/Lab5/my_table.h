#pragma once
#include <windows.h>

class MyTable {
public:
    // Метод для отримання єдиного екземпляра класу
    static MyTable* getInstance();
    // Метод для знищення екземпляра
    static void destroyInstance();

    // Показати немодальне вікно таблиці
    void Show(HINSTANCE hInst, HWND hWndParent);
    // Додати новий рядок з даними про фігуру
    void Add(const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2);

    // Отримати дескриптор вікна (потрібно для циклу повідомлень)
    static HWND GetDialogHandle();

private:
    MyTable() = default;
    ~MyTable() = default;
    // Заборона копіювання
    MyTable(const MyTable&) = delete;
    MyTable& operator=(const MyTable&) = delete;

    static MyTable* p_instance;
    static HWND hDialog; // Дескриптор немодального вікна

    // Процедура обробки повідомлень для вікна таблиці
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};