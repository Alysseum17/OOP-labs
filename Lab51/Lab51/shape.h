#pragma once
#include <windows.h>
#include <string>

class Shape {
protected:
    LONG x1{}, y1{}, x2{}, y2{};
public:
    virtual ~Shape() = default;
    virtual void Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2);
    void GetCoords(LONG& out_x1, LONG& out_y1, LONG& out_x2, LONG& out_y2) const;

    virtual void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) = 0;
    virtual const wchar_t* GetName() const = 0;
    virtual Shape* Clone() const = 0;
};