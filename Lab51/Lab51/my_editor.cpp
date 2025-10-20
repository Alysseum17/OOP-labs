#include "my_editor.h"
#include "resource.h" // Або ваш головний .h файл
#include <windowsx.h>
#include <string>
#include <commctrl.h>
#include <fstream>
#include <codecvt> // Потрібно додати на початку файлу
#include <locale>  // Потрібно додати на початку файлу
#include <cstdio>

MyEditor* MyEditor::p_instance = nullptr;

MyEditor* MyEditor::getInstance(HWND hWnd) {
    if (!p_instance) {
        p_instance = new MyEditor(hWnd);
    }
    return p_instance;
}

MyEditor::MyEditor(HWND hWnd) {
    m_hWnd = hWnd;
    m_max_objects = 120; // 20 + 100
    m_objects = new Shape * [m_max_objects];
    for (int i = 0; i < m_max_objects; ++i) {
        m_objects[i] = nullptr;
    }
  /*  std::ofstream ofs("shapes.txt", std::ofstream::out | std::ofstream::trunc);
    ofs.close();*/
}

MyEditor::~MyEditor() {
    for (int i = 0; i < m_count; ++i) {
        if (m_objects[i]) delete m_objects[i];
    }
    delete[] m_objects;
    if (m_prototype) delete m_prototype;
}

void MyEditor::RegisterCallback(EditorCallback callback) {
    m_callback = callback;
}

void MyEditor::SetToolbar(HWND hwnd) {
    m_hwndToolBar = hwnd;
}

void MyEditor::Start(Shape* prototype) {
    if (m_prototype) {
        delete m_prototype;
    }
    m_prototype = prototype;
}

void MyEditor::OnLDown(HWND hWnd, int x, int y) {
    if (m_prototype) {
        m_isDrawing = true;
        x0 = x_temp = x;
        y0 = y_temp = y;
    }
}

void MyEditor::OnLUp(HWND hWnd, int x, int y) {
    if (m_isDrawing) {
        m_isDrawing = false;
        if (m_count < m_max_objects) {
            Shape* newShape = createShapeBasedOnPrototype(x0, y0, x, y);
            if (newShape) {
                m_objects[m_count++] = newShape;
                saveShapeToFile(newShape);
                if (m_callback) {
                    LONG sx1, sy1, sx2, sy2;
                    newShape->GetCoords(sx1, sy1, sx2, sy2);
                    m_callback(EDITOR_ACTION_ADD, newShape->GetName(), sx1, sy1, sx2, sy2, m_count - 1);
                }
            }
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

void MyEditor::OnMouseMove(HWND hWnd, int x, int y) {
    if (m_isDrawing) {
        x_temp = x;
        y_temp = y;
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

void MyEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    for (int i = 0; i < m_count; ++i) {
        if (m_objects[i]) {
            if (i == m_selectedIndex) {
                HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
                m_objects[i]->Show(hdc);
                SelectObject(hdc, hOldPen);
                DeleteObject(hPen);
            }
            else {
                m_objects[i]->Show(hdc);
            }
        }
    }

    if (m_isDrawing && m_prototype) {
        HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        m_prototype->Set(x0, y0, x_temp, y_temp);
        m_prototype->Show(hdc);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        SelectObject(hdc, hOldBrush);
    }
    EndPaint(hWnd, &ps);
}

void MyEditor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_NEEDTEXT) {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom) {
        case IDM_TOOL_POINT:   lstrcpy(lpttt->szText, L"Крапка"); break;
        case IDM_TOOL_LINE:    lstrcpy(lpttt->szText, L"Лінія"); break;
        case IDM_TOOL_RECT:    lstrcpy(lpttt->szText, L"Прямокутник"); break;
        case IDM_TOOL_ELLIPSE: lstrcpy(lpttt->szText, L"Еліпс"); break;
        case IDM_TOOL_LINEOO:  lstrcpy(lpttt->szText, L"Лінія з кружечками"); break;
        case IDM_TOOL_CUBE:    lstrcpy(lpttt->szText, L"Куб"); break;
        }
    }
}

void MyEditor::SelectShape(int index) {
    if (index >= 0 && index < m_count) {
        m_selectedIndex = index;
    }
    else {
        m_selectedIndex = -1;
    }
    InvalidateRect(m_hWnd, nullptr, TRUE);
}

void MyEditor::DeleteShape(int index) {
    if (index >= 0 && index < m_count) {
        delete m_objects[index];
        for (int i = index; i < m_count - 1; ++i) {
            m_objects[i] = m_objects[i + 1];
        }
        m_count--;
        m_selectedIndex = -1;

        if (m_callback) {
            m_callback(EDITOR_ACTION_DELETE, nullptr, 0, 0, 0, 0, index);
        }
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
}

Shape* MyEditor::createShapeBasedOnPrototype(LONG x1, LONG y1, LONG x2, LONG y2) {
    Shape* newShape = nullptr;
    if (dynamic_cast<PointShape*>(m_prototype)) newShape = new PointShape();
    else if (dynamic_cast<LineOOShape*>(m_prototype)) newShape = new LineOOShape();
    else if (dynamic_cast<CubeShape*>(m_prototype)) newShape = new CubeShape();
    else if (dynamic_cast<LineShape*>(m_prototype)) newShape = new LineShape();
    else if (dynamic_cast<RectShape*>(m_prototype)) newShape = new RectShape();
    else if (dynamic_cast<EllipseShape*>(m_prototype)) newShape = new EllipseShape();

    if (newShape) {
        newShape->Set(x1, y1, x2, y2);
    }
    return newShape;
}

void MyEditor::saveShapeToFile(Shape* shape) {
    FILE* file;
    // Відкриваємо файл для дозапису в бінарному режимі ("ab" - append binary)
    // Це важливо, щоб уникнути проблем з кодуванням системної локалі
    errno_t err = fopen_s(&file, "shapes.txt", "ab");

    if (err != 0 || file == nullptr) {
        return; // Не вдалося відкрити файл
    }

    // Переміщуємо вказівник в кінець файлу, щоб дізнатися його розмір
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);

    // Якщо файл порожній (його розмір 0), значить ми пишемо в нього вперше.
    // Потрібно додати маркер BOM, щоб Блокнот зрозумів, що це UTF-8.
    if (fileSize == 0) {
        unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
        fwrite(bom, sizeof(bom), 1, file);
    }

    // Отримуємо назву фігури (це wchar_t*, тобто UTF-16 у Windows)
    const wchar_t* shapeNameW = shape->GetName();

    // Конвертуємо назву з UTF-16 в UTF-8 (це стандарт для текстових файлів)
    std::string shapeNameA;
    int len = WideCharToMultiByte(CP_UTF8, 0, shapeNameW, -1, NULL, 0, NULL, NULL);
    if (len > 0) {
        shapeNameA.resize(len - 1);
        WideCharToMultiByte(CP_UTF8, 0, shapeNameW, -1, &shapeNameA[0], len, NULL, NULL);
    }

    // Отримуємо координати
    LONG x1, y1, x2, y2;
    shape->GetCoords(x1, y1, x2, y2);

    // Записуємо дані у файл за допомогою fprintf
    // Використовуємо \r\n для переносу рядка, щоб він коректно відображався у Блокноті
    fprintf(file, "%s\t%ld\t%ld\t%ld\t%ld\r\n",
        shapeNameA.c_str(),
        x1, y1, x2, y2);

    // Обов'язково закриваємо файл
    fclose(file);
}
int MyEditor::GetCount() const {
    return m_count;
}

const Shape* MyEditor::GetShape(int index) const {
    if (index >= 0 && index < m_count) {
        return m_objects[index];
    }
    return nullptr;
}