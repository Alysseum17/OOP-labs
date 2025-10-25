#include "my_editor.h"
#include "resource.h"
#include <windowsx.h>
#include <string>
#include <commctrl.h>
#include <fstream>
#include <codecvt>
#include <locale>
#include <cstdio>
#include <cmath>
#include <sstream> // <--- ДОДАЙТЕ ЦЕЙ РЯДОК

MyEditor* MyEditor::p_instance = nullptr;

MyEditor* MyEditor::getInstance(HWND hWnd) {
    if (!p_instance) p_instance = new MyEditor(hWnd);
    return p_instance;
}

MyEditor::MyEditor(HWND hWnd) : m_hWnd(hWnd) {
    m_max_objects = 120; // Або ваш розрахунок
    m_objects = new Shape * [m_max_objects];
    for (int i = 0; i < m_max_objects; ++i) m_objects[i] = nullptr;
    loadShapesFromFile();
}

MyEditor::~MyEditor() {
    for (int i = 0; i < m_count; ++i) if (m_objects[i]) delete m_objects[i];
    delete[] m_objects;
    if (m_prototype) delete m_prototype;
}

void MyEditor::RegisterCallback(EditorCallback callback) { m_callback = callback; }
void MyEditor::SetToolbar(HWND hwnd) { m_hwndToolBar = hwnd; }

void MyEditor::CreateToolbar(HINSTANCE hInst) {
    INITCOMMONCONTROLSEX icex; // Ініціалізація потрібна тут
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    m_hwndToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0, m_hWnd, (HMENU)1, hInst, NULL);
    if (!m_hwndToolBar) return;
    SendMessage(m_hwndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    TBADDBITMAP tbab;
    tbab.hInst = hInst;
    tbab.nID = IDB_BITMAP1;
    SendMessage(m_hwndToolBar, TB_ADDBITMAP, 6, (LPARAM)&tbab);
    TBBUTTON tbb[6];
    ZeroMemory(tbb, sizeof(tbb));
    tbb[0] = { 0, IDM_TOOL_POINT,   TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Крапка" };
    tbb[1] = { 1, IDM_TOOL_LINE,    TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Лінія" };
    tbb[2] = { 2, IDM_TOOL_RECT,    TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Прямокутник" };
    tbb[3] = { 3, IDM_TOOL_ELLIPSE, TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Еліпс" };
    tbb[4] = { 4, IDM_TOOL_LINEOO,  TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"ЛініяОО" };
    tbb[5] = { 5, IDM_TOOL_CUBE,    TBSTATE_ENABLED, TBSTYLE_BUTTON | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Куб" };
    SendMessage(m_hwndToolBar, TB_ADDBUTTONS, 6, (LPARAM)&tbb);
    Start(new PointShape()); // Інструмент за замовчуванням
}

void MyEditor::OnSize() {
    if (m_hwndToolBar) SendMessage(m_hwndToolBar, WM_SIZE, 0, 0);
}

void MyEditor::Start(Shape* prototype) {
    if (m_isDrawing) { m_isDrawing = false; InvalidateRect(m_hWnd, nullptr, TRUE); }
    if (m_prototype) delete m_prototype;
    m_prototype = prototype;
    m_selectedIndex = -1;
    InvalidateRect(m_hWnd, nullptr, TRUE);
    std::wstring shapeName = L"Режим: ";
    if (!m_prototype) shapeName += L"Не вибрано";
    else shapeName += m_prototype->GetName();
    SetWindowText(m_hWnd, shapeName.c_str());
    if (m_hwndToolBar && m_prototype) {
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_POINT, (dynamic_cast<PointShape*>(m_prototype) != nullptr));
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_LINE, (dynamic_cast<LineShape*>(m_prototype) && !dynamic_cast<LineOOShape*>(m_prototype) && !dynamic_cast<CubeShape*>(m_prototype)));
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_RECT, (dynamic_cast<RectShape*>(m_prototype) && !dynamic_cast<CubeShape*>(m_prototype)));
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_ELLIPSE, (dynamic_cast<EllipseShape*>(m_prototype) && !dynamic_cast<LineOOShape*>(m_prototype)));
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_LINEOO, (dynamic_cast<LineOOShape*>(m_prototype) != nullptr));
        SendMessage(m_hwndToolBar, TB_CHECKBUTTON, IDM_TOOL_CUBE, (dynamic_cast<CubeShape*>(m_prototype) != nullptr));
    }
}

void MyEditor::OnLDown(HWND hWnd, int x, int y) {
    if (m_prototype) {
        m_isDrawing = true; x0 = x_temp = x; y0 = y_temp = y;
        m_selectedIndex = -1; InvalidateRect(hWnd, nullptr, TRUE);
    }
}

void MyEditor::OnLUp(HWND hWnd, int x, int y) {
    if (m_isDrawing) {
        m_isDrawing = false;
        if (m_count < m_max_objects) {
            LONG fx1 = x0, fy1 = y0, fx2 = x, fy2 = y;
            bool isJustRect = dynamic_cast<RectShape*>(m_prototype) && !dynamic_cast<CubeShape*>(m_prototype);
            if (isJustRect) { LONG dx = abs(x - x0), dy = abs(y - y0); fx1 = x0 - dx; fy1 = y0 - dy; fx2 = x0 + dx; fy2 = y0 + dy; }
            Shape* newShape = createShapeBasedOnPrototype(fx1, fy1, fx2, fy2);
            if (newShape) {
                m_objects[m_count++] = newShape; saveShapeToFile(newShape);
                if (m_callback) {
                    LONG sx1, sy1, sx2, sy2; newShape->GetCoords(sx1, sy1, sx2, sy2);
                    m_callback(EDITOR_ACTION_ADD, newShape->GetName(), sx1, sy1, sx2, sy2, m_count - 1);
                }
            }
        }
        InvalidateRect(hWnd, nullptr, TRUE);
    }
}

void MyEditor::OnMouseMove(HWND hWnd, int x, int y) {
    if (m_isDrawing) { x_temp = x; y_temp = y; InvalidateRect(hWnd, nullptr, TRUE); }
}

void MyEditor::OnPaint(HWND hWnd) {
    PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
    for (int i = 0; i < m_count; ++i) {
        if (m_objects[i]) {
            if (i == m_selectedIndex) {
                HPEN hSelectPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
                m_objects[i]->Show(hdc, hSelectPen, nullptr);
                DeleteObject(hSelectPen);
            }
            else {
                m_objects[i]->Show(hdc);
            }
        }
    }
    if (m_isDrawing && m_prototype) {
        HPEN hDotPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
        HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hDotPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hNullBrush);
        LONG xs = x0, ys = y0, xe = x_temp, ye = y_temp;
        bool isJustRect = dynamic_cast<RectShape*>(m_prototype) && !dynamic_cast<CubeShape*>(m_prototype);
        if (isJustRect) { LONG dx = abs(xe - xs), dy = abs(ye - ys); xs = x0 - dx; ys = y0 - dy; xe = x0 + dx; ye = y0 + dy; }
        m_prototype->Set(xs, ys, xe, ye);
        m_prototype->Show(hdc, hDotPen, hNullBrush); // Передаємо стиль
        SelectObject(hdc, hOldPen); SelectObject(hdc, hOldBrush);
        DeleteObject(hDotPen);
    }
    EndPaint(hWnd, &ps);
}

void MyEditor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (lParam == NULL || m_hwndToolBar == NULL) return;
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->hwndFrom == m_hwndToolBar && pnmh->code == TTN_NEEDTEXT) {
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
Shape* MyEditor::createShapeByName(const std::wstring& name) {
    // Назви взяті з GetName() у shape.h
    if (name == L"Крапка") return new PointShape();
    if (name == L"Лінія з кружечками") return new LineOOShape();
    if (name == L"Куб") return new CubeShape();
    if (name == L"Лінія") return new LineShape();
    if (name == L"Прямокутник") return new RectShape();
    if (name == L"Еліпс") return new EllipseShape();
    return nullptr;
}

// Ця функція читає файл "shapes.txt" і заповнює масив m_objects
void MyEditor::loadShapesFromFile() {
    // Використовуємо std::wifstream для читання широких символів (wchar_t)
    std::wifstream wif("shapes.txt");
    if (!wif.is_open()) {
        return; // Файлу ще не існує, це нормально
    }

    // Налаштовуємо потік на правильне читання UTF-8 (який ви використовуєте при збереженні)
    // std::consume_header автоматично пропустить BOM (Byte Order Mark), який ви записуєте
    wif.imbue(std::locale(std::locale(),
        new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));

    std::wstring line;
    // Читаємо файл рядок за рядком
    while (std::getline(wif, line)) {
        if (m_count >= m_max_objects) {
            break; // Досягли ліміту об'єктів
        }

        std::wstringstream wss(line);
        std::wstring shapeName;
        LONG x1, y1, x2, y2;
        //wchar_t tab; // Для зчитування символів табуляції

        // Парсимо рядок. Формат: "Назва\tX1\tY1\tX2\tY2"
        if (std::getline(wss, shapeName, L'\t') &&
            (wss >> x1 >> y1 >> x2 >> y2))
        {
            Shape* newShape = createShapeByName(shapeName);

            if (newShape) {
                newShape->Set(x1, y1, x2, y2);
                m_objects[m_count++] = newShape; // Додаємо в масив
            }
        }
    }
    wif.close();
}

void MyEditor::SelectShape(int index) {
    m_selectedIndex = (index >= 0 && index < m_count) ? index : -1;
    InvalidateRect(m_hWnd, nullptr, TRUE);
}

void MyEditor::updateFileAfterDeletion() {
    FILE* file; errno_t err = fopen_s(&file, "shapes.txt", "wb");
    if (err == 0 && file != nullptr) {
        unsigned char bom[] = { 0xEF, 0xBB, 0xBF }; fwrite(bom, sizeof(bom), 1, file);
        for (int i = 0; i < m_count; ++i) {
            if (m_objects[i]) {
                const wchar_t* snW = m_objects[i]->GetName(); std::string snA;
                int len = WideCharToMultiByte(CP_UTF8, 0, snW, -1, NULL, 0, NULL, NULL);
                if (len > 0) { snA.resize(len - 1); WideCharToMultiByte(CP_UTF8, 0, snW, -1, &snA[0], len, NULL, NULL); }
                LONG x1, y1, x2, y2; m_objects[i]->GetCoords(x1, y1, x2, y2);
                fprintf(file, "%s\t%ld\t%ld\t%ld\t%ld\r\n", snA.c_str(), x1, y1, x2, y2);
            }
        }
        fclose(file);
    }
}

void MyEditor::DeleteShape(int index) {
    if (index >= 0 && index < m_count) {
        delete m_objects[index];
        for (int i = index; i < m_count - 1; ++i) m_objects[i] = m_objects[i + 1];
        m_objects[m_count - 1] = nullptr; m_count--; m_selectedIndex = -1;
        if (m_callback) m_callback(EDITOR_ACTION_DELETE, nullptr, 0, 0, 0, 0, index);
        updateFileAfterDeletion();
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
}

Shape* MyEditor::createShapeBasedOnPrototype(LONG x1, LONG y1, LONG x2, LONG y2) {
    Shape* newShape = nullptr; if (!m_prototype) return nullptr;
    if (dynamic_cast<PointShape*>(m_prototype)) newShape = new PointShape();
    else if (dynamic_cast<LineOOShape*>(m_prototype)) newShape = new LineOOShape();
    else if (dynamic_cast<CubeShape*>(m_prototype)) newShape = new CubeShape();
    else if (dynamic_cast<LineShape*>(m_prototype)) newShape = new LineShape();
    else if (dynamic_cast<RectShape*>(m_prototype)) newShape = new RectShape();
    else if (dynamic_cast<EllipseShape*>(m_prototype)) newShape = new EllipseShape();
    if (newShape) newShape->Set(x1, y1, x2, y2);
    return newShape;
}

void MyEditor::saveShapeToFile(Shape* shape) {
    if (!shape) return; FILE* file; errno_t err = fopen_s(&file, "shapes.txt", "ab");
    if (err != 0 || file == nullptr) return;
    fseek(file, 0, SEEK_END); if (ftell(file) == 0) { unsigned char bom[] = { 0xEF, 0xBB, 0xBF }; fwrite(bom, sizeof(bom), 1, file); }
    const wchar_t* snW = shape->GetName(); std::string snA;
    int len = WideCharToMultiByte(CP_UTF8, 0, snW, -1, NULL, 0, NULL, NULL);
    if (len > 0) { snA.resize(len - 1); WideCharToMultiByte(CP_UTF8, 0, snW, -1, &snA[0], len, NULL, NULL); }
    LONG x1, y1, x2, y2; shape->GetCoords(x1, y1, x2, y2);
    fprintf(file, "%s\t%ld\t%ld\t%ld\t%ld\r\n", snA.c_str(), x1, y1, x2, y2);
    fclose(file);
}

int MyEditor::GetCount() const { return m_count; }
const Shape* MyEditor::GetShape(int index) const { return (index >= 0 && index < m_count) ? m_objects[index] : nullptr; }

void MyEditor::OnInitMenuPopup(HMENU hMenu) {
    bool isPoint = 0, isLine = 0, isRect = 0, isEllipse = 0, isLineOO = 0, isCube = 0;
    if (m_prototype) {
        isPoint = dynamic_cast<PointShape*>(m_prototype) != nullptr;
        isLineOO = dynamic_cast<LineOOShape*>(m_prototype) != nullptr;
        isCube = dynamic_cast<CubeShape*>(m_prototype) != nullptr;
        isLine = dynamic_cast<LineShape*>(m_prototype) && !isLineOO && !isCube;
        isRect = dynamic_cast<RectShape*>(m_prototype) && !isCube;
        isEllipse = dynamic_cast<EllipseShape*>(m_prototype) && !isLineOO;
    }
    CheckMenuItem(hMenu, IDM_OBJ_POINT, MF_BYCOMMAND | (isPoint ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hMenu, IDM_OBJ_LINE, MF_BYCOMMAND | (isLine ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hMenu, IDM_OBJ_RECT, MF_BYCOMMAND | (isRect ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hMenu, IDM_OBJ_ELLIPSE, MF_BYCOMMAND | (isEllipse ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hMenu, IDM_OBJ_LINEOO, MF_BYCOMMAND | (isLineOO ? MF_CHECKED : MF_UNCHECKED));
    CheckMenuItem(hMenu, IDM_OBJ_CUBE, MF_BYCOMMAND | (isCube ? MF_CHECKED : MF_UNCHECKED));
}
