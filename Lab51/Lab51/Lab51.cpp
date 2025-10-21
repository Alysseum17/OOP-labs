#include "framework.h"
#include "resource.h"
#include "my_editor.h"
#include "my_table.h"
#include <windowsx.h>
#include <commctrl.h> // Потрібно для TOOLBARCLASSNAME (перенесено в my_editor.cpp)
#include <locale>
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

MyEditor* g_editor = nullptr; // Singleton
MyTable g_table;
// HWND g_hwndToolBar = NULL; // Більше не потрібен
bool isTableVisible = false;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
// Глобальні OnCreate / OnSize видалено
void                TableEventsCallback(TableAction action, int index);
void                EditorEventsCallback(EditorAction action, const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2, int index);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    std::locale::global(std::locale(""));
    UNREFERENCED_PARAMETER(hPrevInstance); UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB51, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB51));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg); DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX); wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc; wcex.cbClsExtra = 0; wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance; wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB51));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB51); wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance; // Зберігаємо глобальний hInst
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 960, 640, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow); UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        g_editor = MyEditor::getInstance(hWnd);
        g_table.RegisterCallback(TableEventsCallback);
        g_editor->RegisterCallback(EditorEventsCallback);
        g_editor->CreateToolbar(hInst); // Викликаємо метод редактора
        break;
    case WM_SIZE:
        g_editor->OnSize(); // Викликаємо метод редактора
        break;
    case WM_NOTIFY:
        g_editor->OnNotify(hWnd, wParam, lParam);
        break;
    case WM_INITMENUPOPUP: {
        HMENU hMenu = GetMenu(hWnd);
        if (hMenu) {
            HMENU hSubMenu = GetSubMenu(hMenu, 1);
            if ((HMENU)wParam == hSubMenu) g_editor->OnInitMenuPopup((HMENU)wParam);
        }
        break;
    }
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_ABOUT: DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About); break;
        case IDM_EXIT: DestroyWindow(hWnd); break;
        case IDM_VIEW_TABLE:
            if (!isTableVisible) {
                g_table.Show(hInst, hWnd); isTableVisible = true; g_table.Clear();
                int count = g_editor->GetCount();
                for (int i = 0; i < count; ++i) {
                    const Shape* shape = g_editor->GetShape(i);
                    if (shape) {
                        LONG x1, y1, x2, y2; shape->GetCoords(x1, y1, x2, y2);
                        g_table.AddEntry(shape->GetName(), x1, y1, x2, y2);
                    }
                }
            }
            break;
        case IDM_OBJ_POINT: case IDM_TOOL_POINT: g_editor->Start(new PointShape()); break;
        case IDM_OBJ_LINE: case IDM_TOOL_LINE: g_editor->Start(new LineShape()); break;
        case IDM_OBJ_RECT: case IDM_TOOL_RECT: g_editor->Start(new RectShape()); break;
        case IDM_OBJ_ELLIPSE: case IDM_TOOL_ELLIPSE: g_editor->Start(new EllipseShape()); break;
        case IDM_OBJ_LINEOO: case IDM_TOOL_LINEOO: g_editor->Start(new LineOOShape()); break;
        case IDM_OBJ_CUBE: case IDM_TOOL_CUBE: g_editor->Start(new CubeShape()); break;
        default: return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break; // Додано break
    }
    case WM_LBUTTONDOWN: g_editor->OnLDown(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
    case WM_LBUTTONUP: g_editor->OnLUp(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
    case WM_MOUSEMOVE: if (wParam & MK_LBUTTON) g_editor->OnMouseMove(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
    case WM_PAINT: g_editor->OnPaint(hWnd); break; // Припускаємо, що g_editor вже створено
    case WM_DESTROY: PostQuitMessage(0); break;
    default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG: return (INT_PTR)TRUE;
    case WM_COMMAND: if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) { EndDialog(hDlg, LOWORD(wParam)); return (INT_PTR)TRUE; } break;
    }
    return (INT_PTR)FALSE;
}

// Глобальні OnCreate та OnSize видалено

void TableEventsCallback(TableAction action, int index) {
    switch (action) {
    case TABLE_ACTION_SELECT: g_editor->SelectShape(index); break;
    case TABLE_ACTION_DELETE: g_editor->DeleteShape(index); break;
    case TABLE_ACTION_CLOSE: isTableVisible = false; break;
    }
}
void EditorEventsCallback(EditorAction action, const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2, int index) {
    switch (action) {
    case EDITOR_ACTION_ADD: g_table.AddEntry(shapeName, x1, y1, x2, y2); break;
    case EDITOR_ACTION_DELETE: g_table.RemoveEntry(index); break;
    }
}