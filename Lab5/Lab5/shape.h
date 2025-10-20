#pragma once
#include <windows.h>

// Базовий абстрактний клас для всіх фігур
class Shape {
protected:
    LONG x1{}, y1{}, x2{}, y2{};
public:
    virtual ~Shape(); // Віртуальний деструктор для коректного видалення
    virtual void Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2);
    // Чисто віртуальний метод Show, який має бути реалізований у всіх похідних класах
    virtual void Show(HDC hdc) const = 0;
};

// Проміжні класи з віртуальним успадкуванням для уникнення "diamond problem"
class PointShape : public virtual Shape { public: void Show(HDC hdc) const override; };
class LineShape : public virtual Shape { public: void Show(HDC hdc) const override; };
class RectShape : public virtual Shape { public: void Show(HDC hdc) const override; };
class EllipseShape : public virtual Shape { public: void Show(HDC hdc) const override; };

// Класи, що використовують множинне успадкування
class LineOOShape : public LineShape, public EllipseShape {
public:
    void Show(HDC hdc) const override;
};

class CubeShape : public LineShape, public RectShape {
public:
    void Show(HDC hdc) const override;
};