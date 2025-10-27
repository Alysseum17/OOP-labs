#pragma once
#include "line_shape.h" 
#include "rect_shape.h" 

class CubeShape : public LineShape, public RectShape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
    const wchar_t* GetName() const override { return L"Куб"; }
    Shape* Clone() const override;
};