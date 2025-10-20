#include "shape.h"
#include <cmath>

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
    Rectangle(hdc, x1, y1, x2, y2);
}

void EllipseShape::Show(HDC hdc) const {
    Ellipse(hdc, x1, y1, x2, y2);
}

void LineOOShape::Show(HDC hdc) const {
    LineShape::Show(hdc);

    const int radius = 5;
    EllipseShape firstCicle, secondCicle;
    firstCicle.Set(x1 - radius, y1 - radius, x1 + radius, y1 + radius);
    firstCicle.Show(hdc);
    secondCicle.Set(x2 - radius, y2 - radius, x2 + radius, y2 + radius);
    secondCicle.Show(hdc);
}

void CubeShape::Show(HDC hdc) const {
    LONG side = abs(x2 - x1);
    LONG dx = side / 2;
    LONG dy = side / 2;

    LONG fx1 = x1, fy1 = y1;
    LONG fx2 = x1 + side, fy2 = y1 + side;

    LONG bx1 = fx1 + dx, by1 = fy1 - dy;
    LONG bx2 = fx2 + dx, by2 = fy2 - dy;

    RectShape frontRect, backRect;
    LineShape connectors[4];

    backRect.Set(bx1, by1, bx2, by2);
    backRect.Show(hdc);

    frontRect.Set(fx1, fy1, fx2, fy2);
    frontRect.Show(hdc);

    connectors[0].Set(fx1, fy1, bx1, by1);
    connectors[1].Set(fx2, fy1, bx2, by1);
    connectors[2].Set(fx2, fy2, bx2, by2);
    connectors[3].Set(fx1, fy2, bx1, by2);

    for (int i = 0; i < 4; i++) {
        connectors[i].Show(hdc);
    }
}