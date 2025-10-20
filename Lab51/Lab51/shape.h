#pragma once
#include <windows.h>
#include <string>

class Shape {
protected:
    LONG x1{}, y1{}, x2{}, y2{};
public:
    virtual ~Shape() = default;
    void Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2);
    void GetCoords(LONG& out_x1, LONG& out_y1, LONG& out_x2, LONG& out_y2) const;
    virtual void Show(HDC hdc) const = 0;
    virtual const wchar_t* GetName() const = 0;
};

class PointShape : public virtual Shape {
public:
    void Show(HDC hdc) const override;
    const wchar_t* GetName() const override { return L"Крапка"; }
};

class LineShape : public virtual Shape {
public:
    void Show(HDC hdc) const override;
    const wchar_t* GetName() const override { return L"Лінія"; }
};

class RectShape : public virtual Shape {
public:
    void Show(HDC hdc) const override;
    const wchar_t* GetName() const override { return L"Прямокутник"; }
};

class EllipseShape : public virtual Shape {
public:
    void Show(HDC hdc) const override;
    const wchar_t* GetName() const override { return L"Еліпс"; }
};

class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(HDC hdc) const override;
    const wchar_t* GetName() const override { return L"Лінія з кружечками"; }
};

class CubeShape : public LineShape, public RectShape {
public:
    void Show(HDC hdc) const override;
    const wchar_t* GetName() const override { return L"Куб"; }
};