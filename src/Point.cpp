//
// Created by Zach Cobell on 1/22/21.
//

#include "Point.h"

Point::Point() : m_x(0.0), m_y(0.0) {}
Point::Point(double x, double y) : m_x(x), m_y(y) {}

double Point::x() const { return m_x; }

double Point::y() const { return m_y; }

void Point::setX(double x) { m_x = x; }

void Point::setY(double y) { m_y = y; }