#include "framework.h"
#include "resource.h"
#include "my_editor.h"
#include "my_table.h"
#include <windowsx.h>
#include <commctrl.h>
#include <locale>
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

MyEditor* g_editor = nullptr;
MyTable g_table;
HWND g_hwndToolBar = NULL;
bool isTableVisible = false;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                OnCreate(HWND);
void                OnSize(HWND);
void                TableEventsCallback(TableAction action, int index);
void                EditorEventsCallback(EditorAction action, const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2, int index);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    std::locale::global(std::locale(""));
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB51, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB51));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    /*delete g_editor;*/

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB51));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB51);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 960, 640, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        g_editor = MyEditor::getInstance(hWnd);
        g_table.RegisterCallback(TableEventsCallback);
        g_editor->RegisterCallback(EditorEventsCallback);
        OnCreate(hWnd);
        break;
    case WM_SIZE:
        OnSize(hWnd);
        break;
    case WM_NOTIFY:
        g_editor->OnNotify(hWnd, wParam, lParam);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_VIEW_TABLE:
            if (!isTableVisible) {
                // 1. Спочатку створюємо та показуємо порожнє вікно.
        // Цей виклик створить діалог і встановить m_hDlg всередині g_table.
                g_table.Show(hInst, hWnd);
                isTableVisible = true;
                // 1. Очищуємо таблицю від старих даних (на випадок, якщо вона була закрита і відкривається знову)
                g_table.Clear();

                // 2. Отримуємо всі існуючі об'єкти з редактора і заповнюємо таблицю
                int count = g_editor->GetCount();
                for (int i = 0; i < count; ++i) {
                    const Shape* shape = g_editor->GetShape(i);
                    if (shape) {
                        LONG x1, y1, x2, y2;
                        shape->GetCoords(x1, y1, x2, y2);
                        g_table.AddEntry(shape->GetName(), x1, y1, x2, y2);
                    }
                }
            }
            break;
        case IDM_OBJ_POINT: case IDM_TOOL_POINT:
            g_editor->Start(new PointShape()); break;
        case IDM_OBJ_LINE: case IDM_TOOL_LINE:
            g_editor->Start(new LineShape()); break;
        case IDM_OBJ_RECT: case IDM_TOOL_RECT:
            g_editor->Start(new RectShape()); break;
        case IDM_OBJ_ELLIPSE: case IDM_TOOL_ELLIPSE:
            g_editor->Start(new EllipseShape()); break;
        case IDM_OBJ_LINEOO: case IDM_TOOL_LINEOO:
            g_editor->Start(new LineOOShape()); break;
        case IDM_OBJ_CUBE: case IDM_TOOL_CUBE:
            g_editor->Start(new CubeShape()); break;
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

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG: return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void OnCreate(HWND hWnd) {
    g_hwndToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hWnd, (HMENU)1, hInst, NULL);
    if (!g_hwndToolBar) return;

    SendMessage(g_hwndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    // --- ПОЧАТОК ЗМІН ---

    // 1. Вказуємо, що бітмап потрібно брати з ресурсів НАШОЇ програми
    TBADDBITMAP tbab;
    tbab.hInst = hInst; // <-- ЗМІНА: Було HINST_COMMCTRL, стало hInst (ручка вашого додатка)
    tbab.nID = IDB_BITMAP1; // <-- ЗМІНА: Вказуємо ID вашого бітмапу з resource.h

    // Повідомляємо Toolbar, що у нашій стрічці 6 іконок
    SendMessage(g_hwndToolBar, TB_ADDBITMAP, 6, (LPARAM)&tbab);

    TBBUTTON tbb[6];
    ZeroMemory(tbb, sizeof(tbb));

    // 2. Вказуємо індекси іконок з вашого бітмапу (нумерація з нуля)
    // iBitmap тепер - це порядковий номер іконки у вашій "стрічці"
    tbb[0] = { 0, IDM_TOOL_POINT,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Крапка" };
    tbb[1] = { 1, IDM_TOOL_LINE,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Лінія" };
    tbb[2] = { 2, IDM_TOOL_RECT,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Прямокутник" };
    tbb[3] = { 3, IDM_TOOL_ELLIPSE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Еліпс" };
    tbb[4] = { 4, IDM_TOOL_LINEOO,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Лінія з кружечками" };
    tbb[5] = { 5, IDM_TOOL_CUBE,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Куб" };

    // --- КІНЕЦЬ ЗМІН ---

    SendMessage(g_hwndToolBar, TB_ADDBUTTONS, 6, (LPARAM)&tbb);

    if (g_editor) {
        g_editor->SetToolbar(g_hwndToolBar);
        g_editor->Start(new PointShape());
    }
}

void OnSize(HWND hWnd) {
    if (g_hwndToolBar) {
        SendMessage(g_hwndToolBar, WM_SIZE, 0, 0);
    }
}

void TableEventsCallback(TableAction action, int index) {
    if (!g_editor) return;
    switch (action) {
    case TABLE_ACTION_SELECT:
        g_editor->SelectShape(index);
        break;
    case TABLE_ACTION_DELETE:
        g_editor->DeleteShape(index);
        break;
    case TABLE_ACTION_CLOSE:
        isTableVisible = false;
        break;
    }
}

void EditorEventsCallback(EditorAction action, const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2, int index) {
    switch (action) {
    case EDITOR_ACTION_ADD:
        g_table.AddEntry(shapeName, x1, y1, x2, y2);
        break;
    case EDITOR_ACTION_DELETE:
        g_table.RemoveEntry(index);
        break;
    }
}