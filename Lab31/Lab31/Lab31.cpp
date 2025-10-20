#include "framework.h"
#include "Lab31.h"
#include "shape_editor.h"
#include <windowsx.h>
#include <commctrl.h> 
#pragma comment(lib, "comctl32.lib") 

#define MAX_LOADSTRING 100
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ShapeObjectsEditor* g_editor = nullptr; 
HWND g_hwndToolBar = NULL;              

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);


void OnCreate(HWND);
void OnSize(HWND);

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
        case IDM_OBJ_POINT:   
        case IDM_TOOL_POINT:    
            g_editor->StartPointEditor();
            break;
        case IDM_OBJ_LINE:      
        case IDM_TOOL_LINE:    
            g_editor->StartLineEditor();
            break;
        case IDM_OBJ_RECT:      
        case IDM_TOOL_RECT:     
            g_editor->StartRectEditor();
            break;
        case IDM_OBJ_ELLIPSE:   
        case IDM_TOOL_ELLIPSE:  
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
    g_hwndToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hWnd, (HMENU)1, hInst, NULL);

    if (!g_hwndToolBar) return;


    SendMessage(g_hwndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);


    TBADDBITMAP tbab;
    tbab.hInst = hInst; 
    tbab.nID = IDB_BITMAP1; 
    SendMessage(g_hwndToolBar, TB_ADDBITMAP, 4, (LPARAM)&tbab);

 
    TBBUTTON tbb[4];
    ZeroMemory(tbb, sizeof(tbb));

    tbb[0] = { 0, IDM_TOOL_POINT,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Крапка" };
    tbb[1] = { 1, IDM_TOOL_LINE,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Лінія" };
    tbb[2] = { 2, IDM_TOOL_RECT,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Прямокутник" };
    tbb[3] = { 3, IDM_TOOL_ELLIPSE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Еліпс" };

    SendMessage(g_hwndToolBar, TB_ADDBUTTONS, 4, (LPARAM)&tbb);

 
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