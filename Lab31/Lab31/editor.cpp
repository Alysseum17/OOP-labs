#include "editor.h"
#include <algorithm>
#include "resource.h"

void PointEditor::OnLButtonDown(HWND hWnd, LONG x, LONG y) { this->x = x; this->y = y; }
void PointEditor::OnMouseMove(HWND hWnd, LONG x, LONG y) {}
void PointEditor::OnLButtonUp(HWND hWnd, LONG x, LONG y) { this->x = x; this->y = y; }
void PointEditor::OnPaint(HWND hWnd, HDC hdc) { SetPixel(hdc, x, y, RGB(0, 0, 0)); }
void PointEditor::OnInitMenuPopup(HMENU hMenu) {
    CheckMenuItem(hMenu, IDM_OBJ_POINT, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_LINE, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_RECT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_ELLIPSE, MF_BYCOMMAND | MF_UNCHECKED);
}

void LineEditor::OnLButtonDown(HWND hWnd, LONG x, LONG y) { this->x1 = x; this->y1 = y; }
void LineEditor::OnMouseMove(HWND hWnd, LONG x, LONG y) { this->x2 = x; this->y2 = y; }
void LineEditor::OnLButtonUp(HWND hWnd, LONG x, LONG y) { this->x2 = x; this->y2 = y; }
void LineEditor::OnPaint(HWND hWnd, HDC hdc) {
    if (x1 != x2 || y1 != y2) {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); 
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

void RectEditor::OnLButtonDown(HWND hWnd, LONG x, LONG y) { this->x1 = x; this->y1 = y; }
void RectEditor::OnMouseMove(HWND hWnd, LONG x, LONG y) { this->x2 = x; this->y2 = y; }
void RectEditor::OnLButtonUp(HWND hWnd, LONG x, LONG y) { this->x2 = x; this->y2 = y; }
void RectEditor::OnPaint(HWND hWnd, HDC hdc) {
    if (x1 != x2 || y1 != y2) {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH)); 

        LONG dx = abs(x2 - x1);
        LONG dy = abs(y2 - y1);
        Rectangle(hdc, x1 - dx, y1 - dy, x1 + dx, y1 + dy); 
      

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        SelectObject(hdc, hOldBrush);
    }
}
void RectEditor::OnInitMenuPopup(HMENU hMenu) {
    CheckMenuItem(hMenu, IDM_OBJ_POINT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_LINE, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_RECT, MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_ELLIPSE, MF_BYCOMMAND | MF_UNCHECKED);
}

void EllipseEditor::OnLButtonDown(HWND hWnd, LONG x, LONG y) { this->x1 = x; this->y1 = y; }
void EllipseEditor::OnMouseMove(HWND hWnd, LONG x, LONG y) { this->x2 = x; this->y2 = y; }
void EllipseEditor::OnLButtonUp(HWND hWnd, LONG x, LONG y) { this->x2 = x; this->y2 = y; }
void EllipseEditor::OnPaint(HWND hWnd, HDC hdc) {
    if (x1 != x2 || y1 != y2) {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); 
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH)); 
        Ellipse(hdc, x1, y1, x2, y2);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        SelectObject(hdc, hOldBrush);
    }
}
void EllipseEditor::OnInitMenuPopup(HMENU hMenu) {
    CheckMenuItem(hMenu, IDM_OBJ_POINT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_LINE, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_RECT, MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_OBJ_ELLIPSE, MF_BYCOMMAND | MF_CHECKED);
}

void PointEditor::Reset() { x = 0; y = 0; }
void LineEditor::Reset() { x1 = 0; y1 = 0; x2 = 0; y2 = 0; }
void RectEditor::Reset() { x1 = 0; y1 = 0; x2 = 0; y2 = 0; }
void EllipseEditor::Reset() { x1 = 0; y1 = 0; x2 = 0; y2 = 0; }