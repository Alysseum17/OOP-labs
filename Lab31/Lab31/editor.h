#pragma once
#include <windows.h>

class Editor {
public:
    virtual ~Editor() = default;
    virtual void OnLButtonDown(HWND hWnd, LONG x, LONG y) = 0;
    virtual void OnMouseMove(HWND hWnd, LONG x, LONG y) = 0;
    virtual void OnLButtonUp(HWND hWnd, LONG x, LONG y) = 0;
    virtual void OnPaint(HWND hWnd, HDC hdc) = 0;
};
class ShapeEditor : public Editor {
public:
    ShapeEditor() = default;
    virtual void OnLButtonDown(HWND hWnd, LONG x, LONG y) override = 0;
    virtual void OnMouseMove(HWND hWnd, LONG x, LONG y) override = 0;
    virtual void OnLButtonUp(HWND hWnd, LONG x, LONG y) override = 0;
    virtual void OnPaint(HWND hWnd, HDC hdc) override = 0;
    virtual void OnInitMenuPopup(HMENU hMenu) = 0;
    virtual void Reset() = 0;
};
class PointEditor : public ShapeEditor {
private:
    LONG x = 0, y = 0;
public:
    void OnLButtonDown(HWND hWnd, LONG x, LONG y) override;
    void OnMouseMove(HWND hWnd, LONG x, LONG y) override;
    void OnLButtonUp(HWND hWnd, LONG x, LONG y) override;
    void OnPaint(HWND hWnd, HDC hdc) override;
    void OnInitMenuPopup(HMENU hMenu) override;
    void Reset() override; 
};
class LineEditor : public ShapeEditor {
private:
    LONG x1 = 0, y1 = 0, x2 = 0, y2 = 0;
public:
    void OnLButtonDown(HWND hWnd, LONG x, LONG y) override;
    void OnMouseMove(HWND hWnd, LONG x, LONG y) override;
    void OnLButtonUp(HWND hWnd, LONG x, LONG y) override;
    void OnPaint(HWND hWnd, HDC hdc) override;
    void OnInitMenuPopup(HMENU hMenu) override;
    void Reset() override; 
};
class RectEditor : public ShapeEditor {
private:
    LONG x1 = 0, y1 = 0, x2 = 0, y2 = 0;
public:
    void OnLButtonDown(HWND hWnd, LONG x, LONG y) override;
    void OnMouseMove(HWND hWnd, LONG x, LONG y) override;
    void OnLButtonUp(HWND hWnd, LONG x, LONG y) override;
    void OnPaint(HWND hWnd, HDC hdc) override;
    void OnInitMenuPopup(HMENU hMenu) override;
    void Reset() override; 
};
class EllipseEditor : public ShapeEditor {
private:
    LONG x1 = 0, y1 = 0, x2 = 0, y2 = 0;
public:
    void OnLButtonDown(HWND hWnd, LONG x, LONG y) override;
    void OnMouseMove(HWND hWnd, LONG x, LONG y) override;
    void OnLButtonUp(HWND hWnd, LONG x, LONG y) override;
    void OnPaint(HWND hWnd, HDC hdc) override;
    void OnInitMenuPopup(HMENU hMenu) override;
    void Reset() override; 
};