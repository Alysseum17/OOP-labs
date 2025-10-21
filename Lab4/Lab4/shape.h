#pragma once
#include <windows.h>

class Shape {
protected:
    LONG x1{}, y1{}, x2{}, y2{};
public:
    virtual ~Shape();
    virtual void Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2);
    virtual void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) = 0;
};

class PointShape : public virtual Shape { public: void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override; };
class LineShape : public virtual Shape { public: void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override; };
class RectShape : public virtual Shape { public: void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override; };
class EllipseShape : public virtual Shape { public: void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override; };

class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
};

class CubeShape : public LineShape, public RectShape {
public:
    void Show(HDC hdc, HPEN hPen = nullptr, HBRUSH hBrush = nullptr) override;
};