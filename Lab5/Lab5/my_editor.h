#pragma once
#include <windows.h>
#include "shape.h"
#include <functional> // Потрібно для std::function

class MyEditor {
public:
    // Визначення типу для callback-функції, яка буде викликатись при додаванні фігури
    using ShapeAddedCallback = std::function<void(const wchar_t*, LONG, LONG, LONG, LONG)>;

    static MyEditor* getInstance(HWND hWnd);
    static void destroyInstance();
    ~MyEditor();

    void SetToolbar(HWND hwnd);
    // Новий метод для реєстрації callback-функції
    void SetOnShapeAddedCallback(ShapeAddedCallback callback);

    void Start(Shape* prototype);
    void OnLDown(HWND hWnd, int x, int y);
    void OnLUp(HWND hWnd, int x, int y);
    void OnMouseMove(HWND hWnd, int x, int y);
    void OnPaint(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
    MyEditor(HWND hWnd);
    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;
    static MyEditor* p_instance;

    HWND m_hWnd;
    HWND m_hwndToolBar = NULL;
    Shape** m_objects = nullptr;
    int m_count = 0;
    int m_max_objects = 0;

    Shape* m_prototype = nullptr;
    bool m_isDrawing = false;
    LONG x0{}, y0{};
    LONG x_temp{}, y_temp{};

    // Член класу для зберігання callback-функції
    ShapeAddedCallback onShapeAdded;

    Shape* createShapeBasedOnPrototype(LONG x1, LONG y1, LONG x2, LONG y2);
};