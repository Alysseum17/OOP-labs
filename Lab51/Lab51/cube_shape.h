#pragma once
#include "line_shape.h" // Включаємо першого батька
#include "rect_shape.h" // Включаємо другого батька

class CubeShape : public LineShape, public RectShape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
    const wchar_t* GetName() const override { return L"Куб"; }
    Shape* Clone() const override; // <--- ДОДАТИ
};