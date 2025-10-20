#pragma once
#include <windows.h>
#include "editor.h"
#include "shape.h"
#include <functional> 


class ShapeObjectsEditor {
public:
    ShapeObjectsEditor(HWND hWnd, HINSTANCE hInst);
    ~ShapeObjectsEditor();

    void CreateToolbar(); 
    void OnSize();        

    void StartPointEditor();
    void StartLineEditor();
    void StartRectEditor();
    void StartEllipseEditor();

    void OnLDown(HWND hWnd, int x, int y);
    void OnLUp(HWND hWnd, int x, int y);
    void OnMouseMove(HWND hWnd, int x, int y);
    void OnPaint(HWND hWnd);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwndToolBar = NULL; 
    HINSTANCE m_hInst = NULL;
    Shape** m_objects = nullptr;
    int m_count = 0;
    int m_max_objects = 0;

    std::function<Shape* ()> m_shapeFactory; 

    LONG x0{}, y0{};
    ShapeEditor* m_currentEditor = nullptr;
    HWND m_hWnd = nullptr;
};