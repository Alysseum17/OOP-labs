#include "lineoo_shape.h"

void LineOOShape::Show(HDC hdc, HPEN hPen, HBRUSH hBrush) {
    LineShape::Show(hdc, hPen);
    LONG original_x1 = this->x1, original_y1 = this->y1;
    LONG original_x2 = this->x2, original_y2 = this->y2;
    const int radius = 5;

    this->x1 = original_x1 - radius;
    this->y1 = original_y1 - radius;
    this->x2 = original_x1 + radius;
    this->y2 = original_y1 + radius;

    EllipseShape::Show(hdc, hPen, hBrush);

    this->x1 = original_x2 - radius;
    this->y1 = original_y2 - radius;
    this->x2 = original_x2 + radius;
    this->y2 = original_y2 + radius;

    EllipseShape::Show(hdc, hPen, hBrush);
    this->x1 = original_x1;
    this->y1 = original_y1;
    this->x2 = original_x2;
    this->y2 = original_y2;
}
Shape* LineOOShape::Clone() const {
    return new LineOOShape(*this);
}
