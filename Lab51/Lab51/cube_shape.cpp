#include "cube_shape.h"
#include <cmath> // Потрібно для round()

void CubeShape::Show(HDC hdc, HPEN hPen, HBRUSH hBrush) {
    LONG sideX = abs(x2 - x1); LONG sideY = abs(y2 - y1);
    float offsetX = static_cast<float>(sideX) / 2.0f;
    float offsetY = static_cast<float>(sideY) / 2.0f;
    LONG fx1 = x1; LONG fy1 = y1; LONG fx2 = x1 + sideX; LONG fy2 = y1 + sideY;
    LONG bx1 = fx1 + static_cast<LONG>(round(offsetX)); LONG by1 = fy1 - static_cast<LONG>(round(offsetY));
    LONG bx2 = fx2 + static_cast<LONG>(round(offsetX)); LONG by2 = fy2 - static_cast<LONG>(round(offsetY));

    LONG original_x1 = this->x1, original_y1 = this->y1;
    LONG original_x2 = this->x2, original_y2 = this->y2;

    HBRUSH hActualBrush = hBrush;
    HBRUSH hOldBrush = nullptr;

    if (hActualBrush == nullptr) {
        hActualBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    hOldBrush = (HBRUSH)SelectObject(hdc, hActualBrush);

    // Малюємо задню грань
    this->x1 = bx1; this->y1 = by1; this->x2 = bx2; this->y2 = by2;
    RectShape::Show(hdc, hPen, hActualBrush);
    // Малюємо передню грань
    this->x1 = fx1; this->y1 = fy1; this->x2 = fx2; this->y2 = fy2;
    RectShape::Show(hdc, hPen, hActualBrush);

    SelectObject(hdc, hOldBrush);

    // Малюємо 4 з'єднувальні лінії
    this->x1 = fx1; this->y1 = fy1; this->x2 = bx1; this->y2 = by1; LineShape::Show(hdc, hPen);
    this->x1 = fx2; this->y1 = fy1; this->x2 = bx2; this->y2 = by1; LineShape::Show(hdc, hPen);
    this->x1 = fx2; this->y1 = fy2; this->x2 = bx2; this->y2 = by2; LineShape::Show(hdc, hPen);
    this->x1 = fx1; this->y1 = fy2; this->x2 = bx1; this->y2 = by2; LineShape::Show(hdc, hPen);

    // Повертаємо координати
    this->x1 = original_x1; this->y1 = original_y1;
    this->x2 = original_x2; this->y2 = original_y2;
}
Shape* CubeShape::Clone() const {
    return new CubeShape(*this);
}