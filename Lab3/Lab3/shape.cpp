#include "shape.h"

Shape::~Shape() = default;

void Shape::Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2) {
    x1 = ax1; y1 = ay1; x2 = ax2; y2 = ay2;
}

void PointShape::Show(HDC hdc) const {
    SetPixel(hdc, x1, y1, RGB(0, 0, 0));
}

void LineShape::Show(HDC hdc) const {
    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);
}

void RectShape::Show(HDC hdc) const {
    // ВАРІАНТ: Рожеве заповнення
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 192, 203));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Rectangle(hdc, x1, y1, x2, y2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

void EllipseShape::Show(HDC hdc) const {
    // ВАРІАНТ: Біле заповнення
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Ellipse(hdc, x1, y1, x2, y2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    SelectObject(hdc, hOldBrush);
}