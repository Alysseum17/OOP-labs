#include "line_shape.h"

void LineShape::Show(HDC hdc, HPEN hPen, HBRUSH hBrush) {
    HPEN hOldPen = nullptr;
    bool ownPenCreated = false;
    if (hPen == nullptr) {
        hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        ownPenCreated = true;
    }
    hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, hOldPen);
    if (ownPenCreated) DeleteObject(hPen);
}
Shape* LineShape::Clone() const {
    return new LineShape(*this);
}