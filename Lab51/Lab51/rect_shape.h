#pragma once
#include "shape.h"

class RectShape : public virtual Shape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
    const wchar_t* GetName() const override { return L"Прямокутник"; }
    Shape* Clone() const override; // <--- ДОДАТИ
};