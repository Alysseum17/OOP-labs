#include "editor.h"
#include <algorithm>
#include "resource.h"


static void NormRect(LONG& x1, LONG& y1, LONG& x2, LONG& y2) {
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
}


void PointEditor::OnLButtonDown(HWND hWnd, LONG x, LONG y) {
    this->x = x;
    this->y = y;
}

void PointEditor::OnMouseMove(HWND hWnd, LONG x, LONG y) {

}

void PointEditor::OnLButtonUp(HWND hWnd, LONG x, LONG y) {
    this->x = x;
    this->y = y;
}

void PointEditor::OnPaint(HWND hWnd, HDC hdc) {
    SetPixel(hdc, x, y, RGB(0, 0, 0));  
}

void PointEditor::OnInitMenuPopup(HMENU hMenu) {
    CheckMenuItem(hMenu, IDM_OBJ_POINT, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_LINE, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_RECT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_ELLIPSE, MF_BYCOMMAND | MF_UNCHECKED);
}
void LineEditor::OnLButtonDown(HWND hWnd, LONG x, LONG y) {
    this->x1 = x;
    this->y1 = y;
}

void LineEditor::OnMouseMove(HWND hWnd, LONG x, LONG y) {
    this->x2 = x;
    this->y2 = y;
}

void LineEditor::OnLButtonUp(HWND hWnd, LONG x, LONG y) {
    this->x2 = x;
    this->y2 = y;
}

void LineEditor::OnPaint(HWND hWnd, HDC hdc) {
    if (x1 != x2 || y1 != y2) {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); 
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        MoveToEx(hdc, x1, y1, nullptr);  
        LineTo(hdc, x2, y2);             
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen); 
        
    }
}

void LineEditor::OnInitMenuPopup(HMENU hMenu) {
    CheckMenuItem(hMenu, IDM_OBJ_POINT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_LINE, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_RECT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_ELLIPSE, MF_BYCOMMAND | MF_UNCHECKED);
}
void RectEditor::OnLButtonDown(HWND hWnd, LONG x, LONG y) {
    this->x1 = x;
    this->y1 = y;
}

void RectEditor::OnMouseMove(HWND hWnd, LONG x, LONG y) {
    this->x2 = x;
    this->y2 = y;
}

void RectEditor::OnLButtonUp(HWND hWnd, LONG x, LONG y) {
    this->x2 = x;
    this->y2 = y;
}

void RectEditor::OnPaint(HWND hWnd, HDC hdc) {
    if (x1 != x2 || y1 != y2) {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); 
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hBrush = (HBRUSH)SelectObject(hdc, GetStockObject(WHITE_BRUSH)); 
        Rectangle(hdc, x1, y1, x2, y2); 
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen); 
        SelectObject(hdc, hBrush);
    }
}

void RectEditor::OnInitMenuPopup(HMENU hMenu) {
    CheckMenuItem(hMenu, IDM_OBJ_POINT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_LINE, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_RECT, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_ELLIPSE, MF_BYCOMMAND | MF_UNCHECKED);
}
void EllipseEditor::OnLButtonDown(HWND hWnd, LONG x, LONG y) {
    this->x1 = x;
    this->y1 = y;
}

void EllipseEditor::OnMouseMove(HWND hWnd, LONG x, LONG y) {
    this->x2 = x;
    this->y2 = y;
}

void EllipseEditor::OnLButtonUp(HWND hWnd, LONG x, LONG y) {
    this->x2 = x;
    this->y2 = y;
}

void EllipseEditor::OnPaint(HWND hWnd, HDC hdc) {
    if (x1 != x2 || y1 != y2) {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); 
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Ellipse(hdc, x1, y1, x2, y2); 
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen); 
        SelectObject(hdc, hBrush);
    }
}

void EllipseEditor::OnInitMenuPopup(HMENU hMenu) {
    CheckMenuItem(hMenu, IDM_OBJ_POINT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_LINE, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_RECT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_ELLIPSE, MF_BYCOMMAND | MF_CHECKED);
}