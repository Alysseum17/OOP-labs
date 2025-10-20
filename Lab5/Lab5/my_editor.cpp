#include "my_editor.h"
#include "resource.h"
// #include "my_table.h" // <- ЗАЛЕЖНІСТЬ ВИДАЛЕНО
#include <windowsx.h>
#include <string>
#include <commctrl.h>
#include <cwchar>

// ... (getInstance, destroyInstance, конструктор, деструктор залишаються без змін) ...
MyEditor* MyEditor::p_instance = nullptr;

MyEditor* MyEditor::getInstance(HWND hWnd) {
    if (!p_instance) {
        p_instance = new MyEditor(hWnd);
    }
    return p_instance;
}

void MyEditor::destroyInstance() {
    if (p_instance) {
        delete p_instance;
        p_instance = nullptr;
    }
}

MyEditor::MyEditor(HWND hWnd) {
    m_hWnd = hWnd;
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


// Реалізація нового методу для встановлення callback
void MyEditor::SetOnShapeAddedCallback(ShapeAddedCallback callback) {
    onShapeAdded = callback;
}

// ОНОВЛЕНИЙ МЕТОД ONLUP
void MyEditor::OnLUp(HWND hWnd, int x, int y) {
    if (m_isDrawing) {
        m_isDrawing = false;
        if (m_count < m_max_objects) {
            Shape* newShape = createShapeBasedOnPrototype(x0, y0, x, y);
            if (newShape) {
                m_objects[m_count++] = newShape;

                std::wstring shapeName = L"Невідомо";
                if (dynamic_cast<PointShape*>(newShape)) shapeName = L"Крапка";
                else if (dynamic_cast<LineOOShape*>(newShape)) shapeName = L"Лінія з кружечками";
                else if (dynamic_cast<CubeShape*>(newShape)) shapeName = L"Куб";
                else if (dynamic_cast<LineShape*>(newShape)) shapeName = L"Лінія";
                else if (dynamic_cast<RectShape*>(newShape)) shapeName = L"Прямокутник";
                else if (dynamic_cast<EllipseShape*>(newShape)) shapeName = L"Еліпс";

                // 1. Викликаємо callback, якщо він зареєстрований,
                //    замість прямого виклику MyTable
                if (onShapeAdded) {
                    onShapeAdded(shapeName.c_str(), x0, y0, x, y);
                }

                // 2. Запис у файл залишається тут, оскільки це логіка редактора
                FILE* f_out;
                if (_wfopen_s(&f_out, L"shapes.txt", L"at, ccs=UTF-8") == 0 && f_out) {
                    fwprintf(f_out, L"%s\t%ld\t%ld\t%ld\t%ld\n", shapeName.c_str(), x0, y0, x, y);
                    fclose(f_out);
                }
            }
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

// ... (Інші методи: Start, OnLDown, OnMouseMove, OnPaint, OnNotify, SetToolbar, createShapeBasedOnPrototype залишаються без змін) ...
void MyEditor::Start(Shape* prototype) {
    if (m_prototype) {
        delete m_prototype;
    }
    m_prototype = prototype;

    std::wstring shapeName;
    if (dynamic_cast<PointShape*>(m_prototype)) shapeName = L"Режим: Крапка";
    else if (dynamic_cast<LineOOShape*>(m_prototype)) shapeName = L"Режим: Лінія з кружечками";
    else if (dynamic_cast<CubeShape*>(m_prototype)) shapeName = L"Режим: Куб";
    else if (dynamic_cast<LineShape*>(m_prototype)) shapeName = L"Режим: Лінія";
    else if (dynamic_cast<RectShape*>(m_prototype)) shapeName = L"Режим: Прямокутник";
    else if (dynamic_cast<EllipseShape*>(m_prototype)) shapeName = L"Режим: Еліпс";
    SetWindowText(m_hWnd, shapeName.c_str());

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

void MyEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    for (int i = 0; i < m_count; ++i) {
        if (m_objects[i]) {
            m_objects[i]->Show(hdc);
        }
    }

    if (m_isDrawing && m_prototype) {
        HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
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

void MyEditor::SetToolbar(HWND hwnd) {
    m_hwndToolBar = hwnd;
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