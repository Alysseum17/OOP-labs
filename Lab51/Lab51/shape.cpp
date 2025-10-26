// 
// ===== shape.cpp =====
//
#include "shape.h"

// Реалізація методів базового класу
void Shape::Set(LONG ax1, LONG ay1, LONG ax2, LONG ay2) {
    x1 = ax1; y1 = ay1; x2 = ax2; y2 = ay2;
}

void Shape::GetCoords(LONG& out_x1, LONG& out_y1, LONG& out_x2, LONG& out_y2) const {
    out_x1 = this->x1; out_y1 = this->y1; out_x2 = this->x2; out_y2 = this->y2;
}