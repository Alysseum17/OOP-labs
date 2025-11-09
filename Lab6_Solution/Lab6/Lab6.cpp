#include "framework.h"
#include "Lab6.h"
#include <windowsx.h>
#include <string>
#include <vector>
#include <sstream>

#define MAX_LOADSTRING 100

#define WM_APP_OBJECT2_READY    (WM_USER + 1)
#define WM_APP_OBJECT3_READY    (WM_USER + 2)
#define WM_APP_DATA_GENERATED   (WM_USER + 3)
#define IDM_PROCESS_DATA        (WM_USER + 4)

struct DataParams {
    int nPoint, xMin, xMax, yMin, yMax;
};

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND g_hWndObject2 = NULL;
HWND g_hWndObject3 = NULL;
DataParams g_params;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ParamsDlgProc(HWND, UINT, WPARAM, LPARAM);
void                StartProcess(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB6, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB6));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB6));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB6);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 400, 300, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_START:
            StartProcess(hWnd);
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_APP_OBJECT2_READY:
    {
        g_hWndObject2 = (HWND)wParam;
        COPYDATASTRUCT cds;
        cds.dwData = 1;
        cds.cbData = sizeof(g_params);
        cds.lpData = &g_params;
        SendMessage(g_hWndObject2, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
        break;
    }
    case WM_APP_OBJECT3_READY:
    {
        g_hWndObject3 = (HWND)wParam;
        PostMessage(g_hWndObject3, WM_COMMAND, IDM_PROCESS_DATA, 0);
        break;
    }
    case WM_APP_DATA_GENERATED:
        MessageBox(hWnd, L"Object2 згенерував дані та помістив їх у буфер обміну.", L"Крок 1 завершено", MB_OK | MB_ICONINFORMATION);

        g_hWndObject3 = FindWindow(L"OBJECT3_CLASS", NULL);
        if (!g_hWndObject3) {
            std::wstring command = L"Object3.exe " + std::to_wstring((UINT_PTR)hWnd);
            WinExec(std::string(command.begin(), command.end()).c_str(), SW_SHOW);
        }
        else {
            PostMessage(g_hWndObject3, WM_COMMAND, IDM_PROCESS_DATA, 0);
        }
        break;
    case WM_DESTROY:
        if (g_hWndObject2) PostMessage(g_hWndObject2, WM_CLOSE, 0, 0);
        if (g_hWndObject3) PostMessage(g_hWndObject3, WM_CLOSE, 0, 0);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void StartProcess(HWND hWnd) {
    if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_PARAMS), hWnd, ParamsDlgProc, (LPARAM)hWnd) == IDOK) {
        g_hWndObject2 = FindWindow(L"OBJECT2_CLASS", NULL);
        if (!g_hWndObject2) {
            std::wstring command = L"Object2.exe " + std::to_wstring((UINT_PTR)hWnd);
            WinExec(std::string(command.begin(), command.end()).c_str(), SW_SHOW);
        }
        else {
            COPYDATASTRUCT cds;
            cds.dwData = 1;
            cds.cbData = sizeof(g_params);
            cds.lpData = &g_params;
            SendMessage(g_hWndObject2, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
        }
    }
}

INT_PTR CALLBACK ParamsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hParent;
    switch (message)
    {
    case WM_INITDIALOG:
        hParent = (HWND)lParam;
        SetDlgItemInt(hDlg, IDC_EDIT_NPOINT, 10, FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_XMIN, 0, TRUE);
        SetDlgItemInt(hDlg, IDC_EDIT_XMAX, 100, TRUE);
        SetDlgItemInt(hDlg, IDC_EDIT_YMIN, 0, TRUE);
        SetDlgItemInt(hDlg, IDC_EDIT_YMAX, 100, TRUE);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            g_params.nPoint = GetDlgItemInt(hDlg, IDC_EDIT_NPOINT, NULL, FALSE);
            g_params.xMin = GetDlgItemInt(hDlg, IDC_EDIT_XMIN, NULL, TRUE);
            g_params.xMax = GetDlgItemInt(hDlg, IDC_EDIT_XMAX, NULL, TRUE);
            g_params.yMin = GetDlgItemInt(hDlg, IDC_EDIT_YMIN, NULL, TRUE);
            g_params.yMax = GetDlgItemInt(hDlg, IDC_EDIT_YMAX, NULL, TRUE);

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
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