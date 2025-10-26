#pragma once
#include "shape.h"

class LineShape : public virtual Shape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
    const wchar_t* GetName() const override { return L"Лінія"; }
    Shape* Clone() const override; // <--- ДОДАТИ
};