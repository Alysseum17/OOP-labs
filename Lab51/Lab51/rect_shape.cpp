#include "rect_shape.h"

void RectShape::Show(HDC hdc, HPEN hPen, HBRUSH hBrush) {
    HPEN hOldPen = nullptr; HBRUSH hOldBrush = nullptr;
    bool ownPenCreated = false; bool ownBrushCreated = false;
    if (hPen == nullptr) {
        hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        ownPenCreated = true;
    }
    if (hBrush == nullptr) {
        hBrush = CreateSolidBrush(RGB(255, 192, 203));
        ownBrushCreated = true;
    }
    hOldPen = (HPEN)SelectObject(hdc, hPen);
    hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Rectangle(hdc, x1, y1, x2, y2);
    SelectObject(hdc, hOldPen); SelectObject(hdc, hOldBrush);
    if (ownPenCreated) DeleteObject(hPen);
    if (ownBrushCreated) DeleteObject(hBrush);
}
Shape* RectShape::Clone() const {
    return new RectShape(*this);
}