#include "shape.h"
#include <cmath>

void Shape::Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2) {
    x1 = ax1; y1 = ay1; x2 = ax2; y2 = ay2;
}

void Shape::GetCoords(LONG& out_x1, LONG& out_y1, LONG& out_x2, LONG& out_y2) const {
    out_x1 = this->x1;
    out_y1 = this->y1;
    out_x2 = this->x2;
    out_y2 = this->y2;
}

void PointShape::Show(HDC hdc) const {
    SetPixel(hdc, x1, y1, RGB(0, 0, 0));
}

void LineShape::Show(HDC hdc) const {
    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);
}

void RectShape::Show(HDC hdc) const {
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
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Ellipse(hdc, x1, y1, x2, y2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    SelectObject(hdc, hOldBrush);
}

void LineOOShape::Show(HDC hdc) const {
    LineShape::Show(hdc);
    const int radius = 5;
    Ellipse(hdc, x1 - radius, y1 - radius, x1 + radius, y1 + radius);
    Ellipse(hdc, x2 - radius, y2 - radius, x2 + radius, y2 + radius);
}

void CubeShape::Show(HDC hdc) const {
    LONG side = abs(x2 - x1);
    LONG dx = side / 2;
    LONG dy = side / 2;

    LONG fx1 = x1, fy1 = y1;
    LONG fx2 = x1 + side, fy2 = y1 + side;

    LONG bx1 = fx1 + dx, by1 = fy1 - dy;
    LONG bx2 = fx2 + dx, by2 = fy2 - dy;

    RectShape backRect, frontRect;
    backRect.Set(bx1, by1, bx2, by2);
    frontRect.Set(fx1, fy1, fx2, fy2);

    backRect.RectShape::Show(hdc);
    frontRect.RectShape::Show(hdc);

    LineShape connectors[4];
    connectors[0].Set(fx1, fy1, bx1, by1);
    connectors[1].Set(fx2, fy1, bx2, by1);
    connectors[2].Set(fx2, fy2, bx2, by2);
    connectors[3].Set(fx1, fy2, bx1, by2);

    for (int i = 0; i < 4; i++) {
        connectors[i].LineShape::Show(hdc);
    }
}