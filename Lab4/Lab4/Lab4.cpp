#include "framework.h"
#include "resource.h" 
#include "my_editor.h"
#include <windowsx.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

MyEditor* g_editor = nullptr; 

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB4));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB4));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB4);
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
        g_editor = new MyEditor(hWnd,hInst);
        g_editor->CreateToolbar();
        break;
    case WM_SIZE:
        if (g_editor) 
        {
            g_editor->OnSize();
        }
        break;
    case WM_NOTIFY:
        g_editor->OnNotify(hWnd, wParam, lParam);
        break;
    case WM_INITMENUPOPUP:
    {
        HMENU hMenu = GetMenu(hWnd);
        if (hMenu && g_editor) {
            HMENU hSubMenu = GetSubMenu(hMenu, 1);
            if ((HMENU)wParam == hSubMenu) {
                g_editor->OnInitMenuPopup((HMENU)wParam);
            }
        }
        break;
    }
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
        case IDM_OBJ_POINT:
        case IDM_TOOL_POINT:
            g_editor->Start(new PointShape());
            break;
        case IDM_OBJ_LINE:
        case IDM_TOOL_LINE:
            g_editor->Start(new LineShape());
            break;
        case IDM_OBJ_RECT:
        case IDM_TOOL_RECT:
            g_editor->Start(new RectShape());
            break;
        case IDM_OBJ_ELLIPSE:
        case IDM_TOOL_ELLIPSE:
            g_editor->Start(new EllipseShape());
            break;
        case IDM_OBJ_LINEOO:
        case IDM_TOOL_LINEOO:
            g_editor->Start(new LineOOShape());
            break;
        case IDM_OBJ_CUBE:
        case IDM_TOOL_CUBE:
            g_editor->Start(new CubeShape());
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

