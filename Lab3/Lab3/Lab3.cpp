#include "framework.h"
#include "Lab3.h"
#include "shape_editor.h"
#include <windowsx.h>
#include <commctrl.h> // Для Toolbar
#pragma comment(lib, "comctl32.lib") // Підключаємо бібліотеку

#define MAX_LOADSTRING 100
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ShapeObjectsEditor* g_editor = nullptr; // Вказівник на редактор
HWND g_hwndToolBar = NULL;              // Глобальний хендл для Toolbar

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// Функції для створення та обробки Toolbar
void OnCreate(HWND);
void OnSize(HWND);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Ініціалізація Common Controls для Toolbar
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB3));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Очищення пам'яті
    if (g_editor)
    {
        delete g_editor;
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 960, 640, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        g_editor = new ShapeObjectsEditor(hWnd);
        OnCreate(hWnd);
        break;
    case WM_SIZE:
        OnSize(hWnd);
        break;
    case WM_NOTIFY:
        g_editor->OnNotify(hWnd, wParam, lParam);
        break;
    case WM_INITMENUPOPUP:
        g_editor->OnInitMenuPopup(hWnd, wParam);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
            // Кожен case тепер має унікальне значення
        case IDM_OBJ_POINT:     // 300
        case IDM_TOOL_POINT:    // 310
            g_editor->StartPointEditor();
            break;
        case IDM_OBJ_LINE:      // 301
        case IDM_TOOL_LINE:     // 311
            g_editor->StartLineEditor();
            break;
        case IDM_OBJ_RECT:      // 302
        case IDM_TOOL_RECT:     // 312
            g_editor->StartRectEditor();
            break;
        case IDM_OBJ_ELLIPSE:   // 303
        case IDM_TOOL_ELLIPSE:  // 313
            g_editor->StartEllipseEditor();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_LBUTTONDOWN:
        g_editor->OnLDown(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONUP:
        g_editor->OnLUp(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON)
            g_editor->OnMouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_PAINT:
        g_editor->OnPaint(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void OnCreate(HWND hWnd)
{
    // 1. Створюємо пусту панель інструментів
    g_hwndToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hWnd, (HMENU)1, hInst, NULL);

    if (!g_hwndToolBar) return;

    // Обов'язкове повідомлення для ініціалізації
    SendMessage(g_hwndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    // 2. Готуємо і додаємо список зображень (ВАШУ ВЛАСНУ бітову карту)
    TBADDBITMAP tbab;
    tbab.hInst = hInst; // <<< ВАЖЛИВО: Використовуємо hInst (наш додаток)
    tbab.nID = IDB_BITMAP1; // <<< ВАЖЛИВО: Вказуємо ID ВАШОЇ бітової карти
    SendMessage(g_hwndToolBar, TB_ADDBITMAP, 4, (LPARAM)&tbab);

    // 3. Готуємо і додаємо самі кнопки
    TBBUTTON tbb[4];
    ZeroMemory(tbb, sizeof(tbb));

    tbb[0] = { 0, IDM_TOOL_POINT,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Крапка" };
    tbb[1] = { 1, IDM_TOOL_LINE,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Лінія" };
    tbb[2] = { 2, IDM_TOOL_RECT,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Прямокутник" };
    tbb[3] = { 3, IDM_TOOL_ELLIPSE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Еліпс" };

    SendMessage(g_hwndToolBar, TB_ADDBUTTONS, 4, (LPARAM)&tbb);

    // Передаємо хендл панелі в редактор
    if (g_editor)
    {
        g_editor->SetToolbar(g_hwndToolBar);
    }
}

void OnSize(HWND hWnd)
{
    if (g_hwndToolBar)
    {
        SendMessage(g_hwndToolBar, WM_SIZE, 0, 0);
    }
}