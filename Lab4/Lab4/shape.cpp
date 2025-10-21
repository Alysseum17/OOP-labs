#include "shape.h"
#include <cmath> 
Shape::~Shape() = default;

void Shape::Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2) {
    x1 = ax1; y1 = ay1; x2 = ax2; y2 = ay2;
}



void PointShape::Show(HDC hdc) {
    SetPixel(hdc, x1, y1, RGB(0, 0, 0));
}

void LineShape::Show(HDC hdc) {
    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);
}

void RectShape::Show(HDC hdc)  {
    Rectangle(hdc, x1, y1, x2, y2);
}

void EllipseShape::Show(HDC hdc)  {
    Ellipse(hdc, x1, y1, x2, y2);
}


void LineOOShape::Show(HDC hdc) {

    LineShape::Show(hdc);

    LONG original_x1 = this->x1, original_y1 = this->y1;
    LONG original_x2 = this->x2, original_y2 = this->y2;
    const int radius = 5;

    this->x1 = original_x1 - radius; 
    this->y1 = original_y1 - radius;
    this->x2 = original_x1 + radius;
    this->y2 = original_y1 + radius;
    EllipseShape::Show(hdc); 

    this->x1 = original_x2 - radius; 
    this->y1 = original_y2 - radius;
    this->x2 = original_x2 + radius;
    this->y2 = original_y2 + radius;
    EllipseShape::Show(hdc); 

    this->x1 = original_x1;
    this->y1 = original_y1;
    this->x2 = original_x2;
    this->y2 = original_y2;
}

void CubeShape::Show(HDC hdc) {
    LONG sideX = abs(x2 - x1);
    LONG sideY = abs(y2 - y1);
    float offsetX = static_cast<float>(sideX) / 2.0f; 
    float offsetY = static_cast<float>(sideY) / 2.0f;

    LONG fx1 = x1;
    LONG fy1 = y1;
    LONG fx2 = x1 + sideX;
    LONG fy2 = y1 + sideY;

    LONG bx1 = fx1 + static_cast<LONG>(round(offsetX));
    LONG by1 = fy1 - static_cast<LONG>(round(offsetY));
    LONG bx2 = fx2 + static_cast<LONG>(round(offsetX));
    LONG by2 = fy2 - static_cast<LONG>(round(offsetY));

   
    LONG original_x1 = this->x1, original_y1 = this->y1;
    LONG original_x2 = this->x2, original_y2 = this->y2;

    this->x1 = bx1; this->y1 = by1; this->x2 = bx2; this->y2 = by2; 
    RectShape::Show(hdc); 


    this->x1 = fx1; this->y1 = fy1; this->x2 = fx2; this->y2 = fy2; 
    RectShape::Show(hdc); 

    this->x1 = fx1; this->y1 = fy1; this->x2 = bx1; this->y2 = by1;
    LineShape::Show(hdc); 
    this->x1 = fx2; this->y1 = fy1; this->x2 = bx2; this->y2 = by1;
    LineShape::Show(hdc);
    this->x1 = fx2; this->y1 = fy2; this->x2 = bx2; this->y2 = by2;
    LineShape::Show(hdc);
    this->x1 = fx1; this->y1 = fy2; this->x2 = bx1; this->y2 = by2;
    LineShape::Show(hdc);

    this->x1 = original_x1;
    this->y1 = original_y1;
    this->x2 = original_x2;
    this->y2 = original_y2;
}