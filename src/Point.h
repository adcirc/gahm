//
// Created by Zach Cobell on 1/22/21.
//

#ifndef METGET_LIBRARY_POINT_H_
#define METGET_LIBRARY_POINT_H_

class Point {
 public:
  Point();
  Point(double x, double y);

  double x() const;

  double y() const;

  void setX(double x);

  void setY(double y);

 private:
  double m_x;
  double m_y;
};

#endif  // METGET_LIBRARY_POINT_H_
