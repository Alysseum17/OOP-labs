#pragma once
#include <windows.h>
#include "shape.h"

class MyEditor {
public:
    MyEditor(HWND hWnd, HINSTANCE hInst);
    ~MyEditor();

    void CreateToolbar(); 
    void OnSize();       

    void Start(Shape* prototype); // Універсальний метод для початку редагування
    void OnLDown(HWND hWnd, int x, int y);
    void OnLUp(HWND hWnd, int x, int y);
    void OnMouseMove(HWND hWnd, int x, int y);
    void OnPaint(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hWnd;
    HINSTANCE m_hInst = NULL;
    HWND m_hwndToolBar = NULL;
    Shape** m_objects = nullptr;
    int m_count = 0;
    int m_max_objects = 0;

    Shape* m_prototype = nullptr; // Прототип для малювання "гумового" сліду
    bool m_isDrawing = false;
    LONG x0{}, y0{};
    LONG x_temp{}, y_temp{};

    Shape* createShapeBasedOnPrototype(LONG x1, LONG y1, LONG x2, LONG y2);
};