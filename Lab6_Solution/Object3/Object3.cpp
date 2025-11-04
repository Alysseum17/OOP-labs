#include "framework.h"
#include "Object3.h"
#include <shellapi.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#define MAX_LOADSTRING 100

// Повідомлення
#define WM_APP_OBJECT3_READY    (WM_USER + 2)
#define IDM_PROCESS_DATA        (WM_USER + 4) // Додано, щоб відповідати Lab6

struct Point { int x, y; };

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
std::string         GetTextFromClipboard(HWND hWnd);
void                ProcessData(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    //UNREFERENCED_PARAMETER(lpCmdLine); // lpCmdLine буде використовуватись

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc > 1) {
        // Отримуємо HWND батька з аргументів командного рядка
        g_hParent = (HWND)(UINT_PTR)_wtoi(argv[1]);
    }
    LocalFree(argv);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OBJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OBJECT3));
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJECT3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OBJECT3);
    wcex.lpszClassName = szWindowClass; // Використовуємо IDC_OBJECT3 як ім'я класу
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 500, 400, nullptr, nullptr, hInstance, nullptr);
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
            // Повідомляємо Lab6, що Object3 готовий
            PostMessage(g_hParent, WM_APP_OBJECT3_READY, (WPARAM)hWnd, 0);
        }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_PROCESS_DATA: // Це повідомлення надсилає Lab6
            ProcessData(hWnd);
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
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);

        // Малюємо осі
        int margin = 50;
        MoveToEx(hdc, margin, rc.bottom - margin, NULL); LineTo(hdc, rc.right - 20, rc.bottom - margin); // Ось X
        MoveToEx(hdc, margin, rc.bottom - margin, NULL); LineTo(hdc, margin, 20); // Ось Y
        TextOut(hdc, rc.right - 15, rc.bottom - margin - 10, L"X", 1);
        TextOut(hdc, margin - 10, 5, L"Y", 1);

        if (g_data.size() > 1) {

            // ==========================================================
            // ===             ПОЧАТОК ЗМІН У ЛОГІЦІ                ===
            // ==========================================================

            // --- Розрахунок діапазону даних ---
            // g_data вже відсортовано по X у ProcessData
            int maxX = g_data.back().x;
            int minY_data = g_data.front().y, maxY = g_data.front().y;

            for (const auto& p : g_data) {
                if (p.y < minY_data) minY_data = p.y;
                if (p.y > maxY) maxY = p.y;
            }

            // --- Встановлення діапазону осей ---
            // Примусово починаємо осі з 0
            int minX = 0;
            int minY = 0;

            // Якщо максимальні значення 0, трохи збільшуємо, щоб уникнути ділення на нуль
            if (maxX == 0) maxX = 1;
            if (maxY == 0) maxY = 1;

            // ==========================================================
            // ===              КІНЕЦЬ ЗМІН У ЛОГІЦІ                ===
            // ==========================================================


            double scaleX = (maxX > minX) ? (double)(rc.right - margin - 20 - margin) / (maxX - minX) : 1.0;
            double scaleY = (maxY > minY) ? (double)(rc.bottom - margin - 20 - margin) / (maxY - minY) : 1.0;

            // ==========================================================
            // ===           Малювання числових міток               ===
            // ==========================================================
            SetBkMode(hdc, TRANSPARENT); // Прозорий фон для тексту

            const int nIntervals = 4; // 4 інтервали дадуть 5 міток (0, 25, 50, 75, 100)

            // --- Мітки для осі X ---
            SetTextAlign(hdc, TA_CENTER | TA_TOP);
            int x_axis_y_pos = rc.bottom - margin + 5; // Позиція Y для міток осі X

            for (int i = 0; i <= nIntervals; ++i)
            {
                // Розраховуємо значення мітки (від 0 до maxX)
                int currentX = minX + (i * (maxX - minX) / nIntervals);
                // Розраховуємо позицію мітки в пікселях
                int pixelX = margin + (int)((currentX - minX) * scaleX);

                std::wstring strX = std::to_wstring(currentX);
                TextOut(hdc, pixelX, x_axis_y_pos, strX.c_str(), (int)strX.length());
            }

            // --- Мітки для осі Y ---
            SetTextAlign(hdc, TA_RIGHT | TA_BASELINE);
            int y_axis_x_pos = margin - 10; // Позиція X для міток осі Y

            for (int i = 0; i <= nIntervals; ++i)
            {
                // Розраховуємо значення мітки (від 0 до maxY)
                int currentY = minY + (i * (maxY - minY) / nIntervals);
                // Розраховуємо позицію мітки в пікселях
                int pixelY = rc.bottom - margin - (int)((currentY - minY) * scaleY);
                if (pixelY < 20) pixelY = 20; // Обмеження зверху

                std::wstring strY = std::to_wstring(currentY);
                TextOut(hdc, y_axis_x_pos, pixelY, strY.c_str(), (int)strY.length());
            }

            // Повертаємо вирівнювання за замовчуванням
            SetTextAlign(hdc, TA_LEFT | TA_TOP);
            // ==========================================================
            // ===             Кінець малювання міток               ===
            // ==========================================================


            // --- Крок 1: Малюємо з'єднувальну лінію (синім кольором) ---
            HPEN hLinePen = CreatePen(PS_SOLID, 1, RGB(100, 100, 255)); // Світло-синій колір
            HPEN hOldPen = (HPEN)SelectObject(hdc, hLinePen);

            // ВАЖЛИВО: Використовуємо нові minX (0) та minY (0) для розрахунку позицій
            int startX = margin + (int)((g_data[0].x - minX) * scaleX);
            int startY = rc.bottom - margin - (int)((g_data[0].y - minY) * scaleY);
            MoveToEx(hdc, startX, startY, NULL);

            for (size_t i = 1; i < g_data.size(); ++i) {
                int nextX = margin + (int)((g_data[i].x - minX) * scaleX);
                int nextY = rc.bottom - margin - (int)((g_data[i].y - minY) * scaleY);
                LineTo(hdc, nextX, nextY);
            }
            SelectObject(hdc, hOldPen);
            DeleteObject(hLinePen);

            // --- Крок 2: Малюємо самі точки (червоним кольором) ---
            HBRUSH hPointBrush = CreateSolidBrush(RGB(255, 0, 0)); // Червоний пензель для заливки
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hPointBrush);
            hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

            const int pointRadius = 3; // Радіус точки в пікселях

            for (const auto& point : g_data) {
                // ВАЖЛИВО: Використовуємо нові minX (0) та minY (0) для розрахунку позицій
                int px = margin + (int)((point.x - minX) * scaleX);
                int py = rc.bottom - margin - (int)((point.y - minY) * scaleY);
                Ellipse(hdc, px - pointRadius, py - pointRadius, px + pointRadius, py + pointRadius);
            }

            SelectObject(hdc, hOldBrush);
            DeleteObject(hPointBrush);
            SelectObject(hdc, hOldPen);
        }
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

void ProcessData(HWND hWnd) {
    g_data.clear();
    std::string clipboardText = GetTextFromClipboard(hWnd);
    if (clipboardText.empty()) return;

    std::stringstream ss(clipboardText);
    int x, y;
    while (ss >> x >> y) { // Читаємо пари x та y
        g_data.push_back({ x, y });
    }

    if (g_data.size() > 1) {
        // Сортуємо дані по X для коректного малювання графіка
        std::sort(g_data.begin(), g_data.end(), [](const Point& a, const Point& b) {
            return a.x < b.x;
            });
    }
    // Оновлюємо вікно, щоб викликати WM_PAINT
    InvalidateRect(hWnd, NULL, TRUE);
}

std::string GetTextFromClipboard(HWND hWnd) {
    if (!OpenClipboard(hWnd)) return "";
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        CloseClipboard();
        return "";
    }
    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (!pszText) {
        CloseClipboard();
        return "";
    }
    std::string text(pszText);
    GlobalUnlock(hData);
    CloseClipboard();
    return text;
}