#include "shape.h"
#include <cmath>

void Shape::Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2) {
    x1 = ax1; y1 = ay1; x2 = ax2; y2 = ay2;
}

void Shape::GetCoords(LONG& out_x1, LONG& out_y1, LONG& out_x2, LONG& out_y2) const {
    out_x1 = this->x1; out_y1 = this->y1; out_x2 = this->x2; out_y2 = this->y2;
}

void PointShape::Show(HDC hdc, HPEN hPen, HBRUSH hBrush) {
    SetPixel(hdc, x1, y1, RGB(0, 0, 0));
}

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

void LineOOShape::Show(HDC hdc, HPEN hPen, HBRUSH hBrush) {
    LineShape::Show(hdc, hPen);
    const int radius = 5;
    HPEN hLocalOldPen = nullptr; HBRUSH hLocalOldBrush = nullptr;
    bool penSelected = false; bool brushSelected = false;
    HPEN hPenToUse = hPen; HBRUSH hBrushToUse = hBrush;
    bool ownPenCreated = false;

    if (hPenToUse == nullptr) {
        hPenToUse = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        ownPenCreated = true;
    }
    if (hBrushToUse == nullptr) {
        hBrushToUse = (HBRUSH)GetStockObject(WHITE_BRUSH);
    }

    hLocalOldPen = (HPEN)SelectObject(hdc, hPenToUse);
    hLocalOldBrush = (HBRUSH)SelectObject(hdc, hBrushToUse);
    penSelected = true; brushSelected = true;

    Ellipse(hdc, x1 - radius, y1 - radius, x1 + radius, y1 + radius);
    Ellipse(hdc, x2 - radius, y2 - radius, x2 + radius, y2 + radius);

    if (penSelected) SelectObject(hdc, hLocalOldPen);
    if (brushSelected) SelectObject(hdc, hLocalOldBrush);

    if (ownPenCreated) DeleteObject(hPenToUse);
}

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

    this->x1 = bx1; this->y1 = by1; this->x2 = bx2; this->y2 = by2;
    RectShape::Show(hdc, hPen, hActualBrush);
    this->x1 = fx1; this->y1 = fy1; this->x2 = fx2; this->y2 = fy2;
    RectShape::Show(hdc, hPen, hActualBrush);

    SelectObject(hdc, hOldBrush);

    this->x1 = fx1; this->y1 = fy1; this->x2 = bx1; this->y2 = by1; LineShape::Show(hdc, hPen);
    this->x1 = fx2; this->y1 = fy1; this->x2 = bx2; this->y2 = by1; LineShape::Show(hdc, hPen);
    this->x1 = fx2; this->y1 = fy2; this->x2 = bx2; this->y2 = by2; LineShape::Show(hdc, hPen);
    this->x1 = fx1; this->y1 = fy2; this->x2 = bx1; this->y2 = by2; LineShape::Show(hdc, hPen);

    this->x1 = original_x1; this->y1 = original_y1;
    this->x2 = original_x2; this->y2 = original_y2;
}