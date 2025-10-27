#pragma once
#include "line_shape.h"    
#include "ellipse_shape.h"

class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
    const wchar_t* GetName() const override { return L"Лінія з кружечками"; }
    Shape* Clone() const override;
};