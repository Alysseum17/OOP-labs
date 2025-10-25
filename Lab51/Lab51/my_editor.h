#pragma once
#include "shape.h"

enum EditorAction { EDITOR_ACTION_ADD, EDITOR_ACTION_DELETE };
typedef void (*EditorCallback)(EditorAction action, const wchar_t* shapeName, LONG x1, LONG y1, LONG x2, LONG y2, int index);

class MyEditor {
public:
    static MyEditor* getInstance(HWND hWnd);

    void RegisterCallback(EditorCallback callback);
    void SelectShape(int index);
    void DeleteShape(int index);
    int GetCount() const;
    const Shape* GetShape(int index) const;

    void SetToolbar(HWND hwnd);
    void CreateToolbar(HINSTANCE hInst);
    void OnSize();
    void Start(Shape* prototype);
    void OnLDown(HWND hWnd, int x, int y);
    void OnLUp(HWND hWnd, int x, int y);
    void OnMouseMove(HWND hWnd, int x, int y);
    void OnPaint(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void OnInitMenuPopup(HMENU hMenu);

private:
    MyEditor(HWND hWnd);
    ~MyEditor();
    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;
    static MyEditor* p_instance;

    HWND m_hWnd;
    EditorCallback m_callback = nullptr;
    int m_selectedIndex = -1;

    HWND m_hwndToolBar = NULL;
    Shape** m_objects = nullptr;
    int m_count = 0;
    int m_max_objects = 0;

    Shape* m_prototype = nullptr;
    bool m_isDrawing = false;
    LONG x0{}, y0{};
    LONG x_temp{}, y_temp{};

    Shape* createShapeBasedOnPrototype(LONG x1, LONG y1, LONG x2, LONG y2);
    void saveShapeToFile(Shape* shape);
    void updateFileAfterDeletion();
    void loadShapesFromFile();
    Shape* createShapeByName(const std::wstring& name);
};