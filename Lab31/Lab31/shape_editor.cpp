#include "shape_editor.h"
#include "Lab31.h"
#include <windowsx.h>
#include <algorithm>
#include <string>
#include <commctrl.h>

ShapeObjectsEditor::ShapeObjectsEditor(HWND hWnd) {
    m_hWnd = hWnd;
    m_max_objects = 121; 
    m_objects = new Shape * [m_max_objects];
    for (int i = 0; i < m_max_objects; ++i) {
        m_objects[i] = nullptr;
    }
}

ShapeObjectsEditor::~ShapeObjectsEditor() {
    for (int i = 0; i < m_count; ++i) {
        if (m_objects[i]) {
            delete m_objects[i];
        }
    }
    delete[] m_objects;
    if (m_currentEditor) {
        delete m_currentEditor;
    }
}

void ShapeObjectsEditor::StartPointEditor() {
    if (m_count < m_max_objects) {
        if (m_currentEditor) delete m_currentEditor;
        m_currentEditor = new PointEditor();
        m_shapeFactory = []() { return new PointShape(); }; 

        SetWindowText(m_hWnd, L"Режим: Крапка");
        InvalidateRect(m_hWnd, nullptr, TRUE);
        if (m_hwndToolBar) {
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_POINT, TRUE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_LINE, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_RECT, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_ELLIPSE, FALSE);
        }
    }
}

void ShapeObjectsEditor::StartLineEditor() {
    if (m_count < m_max_objects) {
        if (m_currentEditor) delete m_currentEditor;
        m_currentEditor = new LineEditor();
        m_shapeFactory = []() { return new LineShape(); };
        SetWindowText(m_hWnd, L"Режим: Лінія"); 
        InvalidateRect(m_hWnd, nullptr, TRUE);
        if (m_hwndToolBar) {
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_POINT, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_LINE, TRUE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_RECT, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_ELLIPSE, FALSE);
        }
    }
}

void ShapeObjectsEditor::StartRectEditor() {
    if (m_count < m_max_objects) {
        if (m_currentEditor) delete m_currentEditor;
        m_currentEditor = new RectEditor();
        m_shapeFactory = []() { return new RectShape(); };
        SetWindowText(m_hWnd, L"Режим: Прямокутник");
        InvalidateRect(m_hWnd, nullptr, TRUE);
        if (m_hwndToolBar) {
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_POINT, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_LINE, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_RECT, TRUE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_ELLIPSE, FALSE);
        }
    }
}

void ShapeObjectsEditor::StartEllipseEditor() {
    if (m_count < m_max_objects) {
        if (m_currentEditor) delete m_currentEditor;
        m_currentEditor = new EllipseEditor();
        m_shapeFactory = []() { return new EllipseShape(); };
        SetWindowText(m_hWnd, L"Режим: Еліпс"); 
        InvalidateRect(m_hWnd, nullptr, TRUE);
        if (m_hwndToolBar) {
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_POINT, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_LINE, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_RECT, FALSE);
            SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_ELLIPSE, TRUE);
        }
    }
}

void ShapeObjectsEditor::OnLDown(HWND hWnd, int x, int y) {
    if (m_currentEditor) {
        x0 = x;
        y0 = y;
        m_currentEditor->OnLButtonDown(hWnd, x, y);
    }
}

void ShapeObjectsEditor::OnMouseMove(HWND hWnd, int x, int y) {
    if (m_currentEditor) {
        m_currentEditor->OnMouseMove(hWnd, x, y);
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

void ShapeObjectsEditor::OnLUp(HWND hWnd, int x, int y) {
    if (m_currentEditor && m_shapeFactory) { 
        if (m_count >= m_max_objects) return;

       
        Shape* newShape = m_shapeFactory();

   
        if (dynamic_cast<RectEditor*>(m_currentEditor)) {
            LONG dx = abs(x - x0);
            LONG dy = abs(y - y0);
            newShape->Set(x0 - dx, y0 - dy, x0 + dx, y0 + dy);
        }
        else {
            newShape->Set(x0, y0, x, y);
        }

        m_objects[m_count++] = newShape;
        m_currentEditor->OnLButtonUp(hWnd, x, y);
        m_currentEditor->Reset();
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

void ShapeObjectsEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    for (int i = 0; i < m_count; ++i) {
        if (m_objects[i]) {
            m_objects[i]->Show(hdc);
        }
    }
    if (m_currentEditor) {
        m_currentEditor->OnPaint(hWnd, hdc);
    }
    EndPaint(hWnd, &ps);
}

void ShapeObjectsEditor::OnInitMenuPopup(HWND, WPARAM wParam) {
    if (m_currentEditor) {
        m_currentEditor->OnInitMenuPopup((HMENU)wParam);
    }
}

void ShapeObjectsEditor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_NEEDTEXT)
    {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom)
        {
        case IDM_TOOL_POINT:
            lstrcpy(lpttt->szText, L"Крапка");
            break;
        case IDM_TOOL_LINE:
            lstrcpy(lpttt->szText, L"Лінія");
            break;
        case IDM_TOOL_RECT:
            lstrcpy(lpttt->szText, L"Прямокутник");
            break;
        case IDM_TOOL_ELLIPSE:
            lstrcpy(lpttt->szText, L"Еліпс");
            break;
        }
    }
}
void ShapeObjectsEditor::SetToolbar(HWND hwnd)
{
    m_hwndToolBar = hwnd;
}