#pragma once
#include <windows.h>

class Shape {
protected:
    LONG x1{}, y1{}, x2{}, y2{};
public:
    virtual ~Shape();
    void Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2);
    virtual void Show(HDC hdc) const = 0;
};


class PointShape : public Shape { public: void Show(HDC hdc) const override; };
class LineShape : public Shape { public: void Show(HDC hdc) const override; };
class RectShape : public Shape { public: void Show(HDC hdc) const override; };
class EllipseShape : public Shape { public: void Show(HDC hdc) const override; };