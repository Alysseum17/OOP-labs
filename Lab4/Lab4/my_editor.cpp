#include "my_editor.h"
#include "resource.h" // Використовуємо ID з вашого проєкту
#include <windowsx.h>
#include <string>
#include <commctrl.h>

MyEditor::MyEditor(HWND hWnd, HINSTANCE hInst) {
    m_hWnd = hWnd;
    m_hInst = hInst;
    m_max_objects = 140; // 2 * 20 + 100
    m_objects = new Shape * [m_max_objects];
    for (int i = 0; i < m_max_objects; ++i) {
        m_objects[i] = nullptr;
    }
}

MyEditor::~MyEditor() {
    for (int i = 0; i < m_count; ++i) {
        if (m_objects[i]) {
            delete m_objects[i];
        }
    }
    delete[] m_objects;
    if (m_prototype) {
        delete m_prototype;
    }
}

void MyEditor::Start(Shape* prototype) {
    if (m_prototype) {
        delete m_prototype;
    }
    m_prototype = prototype;

    // Оновлення тексту заголовка вікна
    std::wstring shapeName;
    if (dynamic_cast<PointShape*>(m_prototype)) shapeName = L"Режим: Крапка";
    else if (dynamic_cast<LineOOShape*>(m_prototype)) shapeName = L"Режим: Лінія з кружечками";
    else if (dynamic_cast<CubeShape*>(m_prototype)) shapeName = L"Режим: Куб";
    else if (dynamic_cast<LineShape*>(m_prototype)) shapeName = L"Режим: Лінія";
    else if (dynamic_cast<RectShape*>(m_prototype)) shapeName = L"Режим: Прямокутник";
    else if (dynamic_cast<EllipseShape*>(m_prototype)) shapeName = L"Режим: Еліпс";
    SetWindowText(m_hWnd, shapeName.c_str());

    // Оновлення стану кнопок на панелі інструментів
    if (m_hwndToolBar) {
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_POINT, dynamic_cast<PointShape*>(m_prototype) != nullptr);
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_LINE, dynamic_cast<LineShape*>(m_prototype) && !dynamic_cast<LineOOShape*>(m_prototype));
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_RECT, dynamic_cast<RectShape*>(m_prototype) && !dynamic_cast<CubeShape*>(m_prototype));
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_ELLIPSE, dynamic_cast<EllipseShape*>(m_prototype) && !dynamic_cast<LineOOShape*>(m_prototype));
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_LINEOO, dynamic_cast<LineOOShape*>(m_prototype) != nullptr);
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_CUBE, dynamic_cast<CubeShape*>(m_prototype) != nullptr);
    }
}

void MyEditor::OnLDown(HWND hWnd, int x, int y) {
    if (m_prototype) {
        m_isDrawing = true;
        x0 = x_temp = x;
        y0 = y_temp = y;
    }
}

void MyEditor::OnMouseMove(HWND hWnd, int x, int y) {
    if (m_isDrawing) {
        x_temp = x;
        y_temp = y;
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

void MyEditor::OnLUp(HWND hWnd, int x, int y) {
    if (m_isDrawing) {
        m_isDrawing = false;
        if (m_count < m_max_objects) {
            Shape* newShape = createShapeBasedOnPrototype(x0, y0, x, y);
            if (newShape) {
                m_objects[m_count++] = newShape;
            }
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

void MyEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // Малювання збережених об'єктів
    for (int i = 0; i < m_count; ++i) {
        if (m_objects[i]) {
            m_objects[i]->Show(hdc);
        }
    }

    // Малювання "гумового" сліду
    if (m_isDrawing && m_prototype) {
        HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0)); // Пунктирна лінія
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

        m_prototype->Set(x0, y0, x_temp, y_temp);
        m_prototype->Show(hdc);

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        SelectObject(hdc, hOldBrush);
    }
    EndPaint(hWnd, &ps);
}

void MyEditor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_NEEDTEXT) {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom) {
        case IDM_TOOL_POINT:   lstrcpy(lpttt->szText, L"Крапка"); break;
        case IDM_TOOL_LINE:    lstrcpy(lpttt->szText, L"Лінія"); break;
        case IDM_TOOL_RECT:    lstrcpy(lpttt->szText, L"Прямокутник"); break;
        case IDM_TOOL_ELLIPSE: lstrcpy(lpttt->szText, L"Еліпс"); break;
        case IDM_TOOL_LINEOO:  lstrcpy(lpttt->szText, L"Лінія з кружечками"); break;
        case IDM_TOOL_CUBE:    lstrcpy(lpttt->szText, L"Куб"); break;
        }
    }
}

Shape* MyEditor::createShapeBasedOnPrototype(LONG x1, LONG y1, LONG x2, LONG y2) {
    Shape* newShape = nullptr;
    if (dynamic_cast<PointShape*>(m_prototype)) newShape = new PointShape();
    else if (dynamic_cast<LineOOShape*>(m_prototype)) newShape = new LineOOShape();
    else if (dynamic_cast<CubeShape*>(m_prototype)) newShape = new CubeShape();
    else if (dynamic_cast<LineShape*>(m_prototype)) newShape = new LineShape();
    else if (dynamic_cast<RectShape*>(m_prototype)) newShape = new RectShape();
    else if (dynamic_cast<EllipseShape*>(m_prototype)) newShape = new EllipseShape();

    if (newShape) {
        newShape->Set(x1, y1, x2, y2);
    }
    return newShape;
}

void MyEditor::CreateToolbar()
{
    m_hwndToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        m_hWnd, (HMENU)1, m_hInst, NULL);
    if (!m_hwndToolBar) return;

    SendMessage(m_hwndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    // Потрібно додати 2 нові іконки у ваш bitmap IDB_BITMAP1
    TBADDBITMAP tbab;
    tbab.hInst = m_hInst;
    tbab.nID = IDB_BITMAP1; // Припускаємо, що тут тепер 6 іконок
    SendMessage(m_hwndToolBar, TB_ADDBITMAP, 6, (LPARAM)&tbab);

    TBBUTTON tbb[6];
    ZeroMemory(tbb, sizeof(tbb));

    tbb[0] = { 0, IDM_TOOL_POINT,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Крапка" };
    tbb[1] = { 1, IDM_TOOL_LINE,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Лінія" };
    tbb[2] = { 2, IDM_TOOL_RECT,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Прямокутник" };
    tbb[3] = { 3, IDM_TOOL_ELLIPSE, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Еліпс" };
    tbb[4] = { 4, IDM_TOOL_LINEOO,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Лінія з кружечками" };
    tbb[5] = { 5, IDM_TOOL_CUBE,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Куб" };

    SendMessage(m_hwndToolBar, TB_ADDBUTTONS, 6, (LPARAM)&tbb);
}

// НОВИЙ МЕТОД: Логіка перенесена з Lab31.cpp
void MyEditor::OnSize()
{
    if (m_hwndToolBar)
    {
        // Оновлюємо розмір тулбару, який є членом класу
        SendMessage(m_hwndToolBar, WM_SIZE, 0, 0);
    }
}