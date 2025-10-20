#pragma once
#include <windows.h>
#include "editor.h"   
#include "shape.h" 

class ShapeObjectsEditor {
public:
    ShapeObjectsEditor() = default;
    ~ShapeObjectsEditor() = default;


    void Attach(HWND hWnd);
    void Detach();

 
    void StartPointEditor();
    void StartLineEditor();
    void StartRectEditor();
    void StartEllipseEditor();



    void OnLDown(HWND hWnd, int x, int y);
    void OnLUp(HWND hWnd, int x, int y);
    void OnMouseMove(HWND hWnd, int x, int y);
    void OnPaint(HWND hWnd);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam);



private:
    static const int MAX_OBJECTS = 120; 
    int m_count = 0; 
    Shape* m_objects[MAX_OBJECTS] = {};  

    int type;
    LONG x0{}, y0{};
    ShapeEditor* m_currentEditor = nullptr; 
    HWND   m_hWnd = nullptr;
};