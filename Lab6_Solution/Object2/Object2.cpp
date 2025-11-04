#include "framework.h"
#include "Object2.h"
#include <shellapi.h>
#include <vector>
#include <string>
#include <sstream>
#include <random>

#define MAX_LOADSTRING 100

// Повідомлення
#define WM_APP_OBJECT2_READY    (WM_USER + 1)
#define WM_APP_DATA_GENERATED   (WM_USER + 3)

// Структури
struct Point { int x, y; };
struct DataParams { int nPoint, xMin, xMax, yMin, yMax; };

// Глобальні змінні
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
std::vector<Point> g_data;
HWND g_hParent = NULL;

// Прототипи
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                PutTextToClipboard(HWND hWnd, const std::string& text);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc > 1) {
        g_hParent = (HWND)(UINT_PTR)_wtoi(argv[1]);
    }
    LocalFree(argv);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OBJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OBJECT2));
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJECT2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OBJECT2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 300, 400, nullptr, nullptr, hInstance, nullptr);
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
        if (g_hParent) {
            PostMessage(g_hParent, WM_APP_OBJECT2_READY, (WPARAM)hWnd, 0);
        }
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
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_COPYDATA:
    {
        COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
        if (pcds->dwData == 1) {
            DataParams* params = (DataParams*)pcds->lpData;
            g_hParent = (HWND)wParam;

            g_data.clear();
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distribX(params->xMin, params->xMax);
            std::uniform_int_distribution<> distribY(params->yMin, params->yMax);

            for (int i = 0; i < params->nPoint; ++i) {
                g_data.push_back({ distribX(gen), distribY(gen) });
            }

            std::stringstream ss;
            for (const auto& p : g_data) {
                ss << p.x << "\t" << p.y << "\n";
            }
            PutTextToClipboard(hWnd, ss.str());

            PostMessage(g_hParent, WM_APP_DATA_GENERATED, 0, 0);
            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // ================== НОВИЙ КОД ДЛЯ ШРИФТУ ==================
        HFONT hFont, hOldFont;
        LOGFONT lf = { 0 };

        // Встановлюємо безпечний шрифт, що 100% підтримує кирилицю
        lf.lfHeight = 16; // Висота шрифту

        // Оберіть один з цих шрифтів. "Tahoma" дуже надійний.
        //wcscpy_s(lf.lfFaceName, L"Tahoma");
         wcscpy_s(lf.lfFaceName, L"Segoe UI"); // Сучасний шрифт Windows

        hFont = CreateFontIndirect(&lf);
        hOldFont = (HFONT)SelectObject(hdc, hFont);
        // ==========================================================

        RECT rc;
        GetClientRect(hWnd, &rc);
        int yPos = 10;

        // Тепер TextOut буде використовувати правильний шрифт
        TextOut(hdc, 10, yPos, L"Згенеровані дані:", 17); // <--- Тут має бути кирилиця

        yPos += 20;
        for (const auto& p : g_data) {
            std::wstring text = L"x: " + std::to_wstring(p.x) + L", y: " + std::to_wstring(p.y);
            TextOut(hdc, 10, yPos, text.c_str(), (int)text.length());
            yPos += 20;
        }

        // ================ Очищення Шрифту =================
        SelectObject(hdc, hOldFont); // Повертаємо старий шрифт
        DeleteObject(hFont);         // Видаляємо створений
        // ====================================================

        EndPaint(hWnd, &ps);
    }
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

void PutTextToClipboard(HWND hWnd, const std::string& text) {
    if (!OpenClipboard(hWnd)) return;
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hg) {
        CloseClipboard();
        return;
    }
    char* pszText = static_cast<char*>(GlobalLock(hg));
    if (pszText) {
        memcpy(pszText, text.c_str(), text.size() + 1);
        GlobalUnlock(hg);
        SetClipboardData(CF_TEXT, hg);
    }
    CloseClipboard();
}