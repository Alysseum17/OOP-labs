#pragma once
#include "shape.h" // Успадковуємо від Shape

class PointShape : public virtual Shape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
    const wchar_t* GetName() const override { return L"Крапка"; }
    Shape* Clone() const override; // <--- ДОДАТИ
};