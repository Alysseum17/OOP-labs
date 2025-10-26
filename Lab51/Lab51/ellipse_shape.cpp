#include "ellipse_shape.h"

void EllipseShape::Show(HDC hdc, HPEN hPen, HBRUSH hBrush) {
    HPEN hOldPen = nullptr; HBRUSH hOldBrush = nullptr;
    bool ownPenCreated = false;
    if (hPen == nullptr) {
        hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        ownPenCreated = true;
    }
    if (hBrush == nullptr) {
        hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    }
    hOldPen = (HPEN)SelectObject(hdc, hPen);
    hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Ellipse(hdc, x1, y1, x2, y2);
    SelectObject(hdc, hOldPen); SelectObject(hdc, hOldBrush);
    if (ownPenCreated) DeleteObject(hPen);
}
Shape* EllipseShape::Clone() const {
    return new EllipseShape(*this);
}