#pragma once
#include "shape.h"

class EllipseShape : public virtual Shape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
    const wchar_t* GetName() const override { return L"Еліпс"; }
    Shape* Clone() const override; 
};