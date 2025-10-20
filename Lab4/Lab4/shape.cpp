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

// Реалізація нових класів
void LineOOShape::Show(HDC hdc) const {
    // Використовуємо реалізацію з базового класу для малювання лінії
    LineShape::Show(hdc);

    // Малюємо кружечки на кінцях
    const int radius = 5;
    Ellipse(hdc, x1 - radius, y1 - radius, x1 + radius, y1 + radius);
    Ellipse(hdc, x2 - radius, y2 - radius, x2 + radius, y2 + radius);
}

void CubeShape::Show(HDC hdc) const {
    // Визначаємо геометрію куба
    LONG side = abs(x2 - x1);
    LONG dx = side / 2;
    LONG dy = side / 2;

    // Координати переднього прямокутника
    LONG fx1 = x1, fy1 = y1;
    LONG fx2 = x1 + side, fy2 = y1 + side;

    // Координати заднього прямокутника
    LONG bx1 = fx1 + dx, by1 = fy1 - dy;
    LONG bx2 = fx2 + dx, by2 = fy2 - dy;

    // Створюємо тимчасові об'єкти для малювання
    RectShape frontRect, backRect;
    LineShape connectors[4];

    // Малюємо задній прямокутник
    backRect.Set(bx1, by1, bx2, by2);
    backRect.RectShape::Show(hdc);

    // Малюємо передній прямокутник
    frontRect.Set(fx1, fy1, fx2, fy2);
    frontRect.RectShape::Show(hdc);

    // Малюємо з'єднувальні лінії
    connectors[0].Set(fx1, fy1, bx1, by1);
    connectors[1].Set(fx2, fy1, bx2, by1);
    connectors[2].Set(fx2, fy2, bx2, by2);
    connectors[3].Set(fx1, fy2, bx1, by2);

    for (int i = 0; i < 4; i++) {
        connectors[i].LineShape::Show(hdc);
    }
}