#include "point_shape.h"

void PointShape::Show(HDC hdc, HPEN hPen, HBRUSH hBrush) {
    SetPixel(hdc, x1, y1, RGB(0, 0, 0));
}
Shape* PointShape::Clone() const {
    return new PointShape(*this); // Створює і повертає копію себе
}