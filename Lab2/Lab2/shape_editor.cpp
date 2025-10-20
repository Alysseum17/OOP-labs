#include "shape_editor.h"
#include "Lab2.h"     
#include <windowsx.h>   
#include <algorithm>


void ShapeObjectsEditor::Attach(HWND hWnd) {
    m_hWnd = hWnd;
}

void ShapeObjectsEditor::Detach() {
    if (m_currentEditor) {
        delete m_currentEditor;  
        m_currentEditor = nullptr;
    }
    m_hWnd = nullptr;
}


void ShapeObjectsEditor::StartPointEditor() {
    if (m_count < MAX_OBJECTS) {
        if (m_currentEditor) delete m_currentEditor;
        type = 1;
        m_currentEditor = new PointEditor();
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
}

void ShapeObjectsEditor::StartLineEditor() {
    if (m_count < MAX_OBJECTS) {
        if (m_currentEditor) delete m_currentEditor;
        type = 2;
        m_currentEditor = new LineEditor();
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
}

void ShapeObjectsEditor::StartRectEditor() {
    if (m_count < MAX_OBJECTS) {
        if (m_currentEditor) delete m_currentEditor;
        type = 3;
        m_currentEditor = new RectEditor();
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
}

void ShapeObjectsEditor::StartEllipseEditor() {
    if (m_count < MAX_OBJECTS) {
        if (m_currentEditor) delete m_currentEditor;
        type = 4;
        m_currentEditor = new EllipseEditor();
        InvalidateRect(m_hWnd, nullptr, TRUE);
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
    if (m_currentEditor) {
        Shape* newShape = nullptr;

        switch (type) {
            case 1:newShape = new PointShape(); break;
            case 2:newShape = new LineShape(); break;
            case 3:newShape = new RectShape(); break;
            case 4:newShape = new EllipseShape(); break;
            default: return;
        }

        newShape->Set(x0, y0, x, y); 

 
        if (m_count < MAX_OBJECTS) {
            m_objects[m_count++] = newShape;
        }
        else {
    
            delete newShape;  
            return;
        }

        m_currentEditor->OnLButtonUp(hWnd, x, y);
    }

 
    InvalidateRect(hWnd, nullptr, FALSE);
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
