// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//

#ifndef METGET_LIBRARY_GEOMETRY_H_
#define METGET_LIBRARY_GEOMETRY_H_

#include "Point.h"
#include "boost/geometry.hpp"

class Geometry {
 public:
  template <size_t s>
  explicit Geometry(const std::array<Point, s> &points) {
    for (const auto &p : points) {
      boost::geometry::append(m_polygon, geom_point_t(p.x(), p.y()));
    }
    boost::geometry::append(m_polygon,
                            geom_point_t(points[0].x(), points[0].y()));
    boost::geometry::correct(m_polygon);
  }

  bool is_inside(const Point &p) const {
    return boost::geometry::within(geom_point_t(p.x(), p.y()), m_polygon);
  }

 private:
  typedef boost::geometry::model::point<double, 2,
                                        boost::geometry::cs::cartesian>
      geom_point_t;
  typedef boost::geometry::model::polygon<geom_point_t> geom_polygon_t;

  geom_polygon_t m_polygon;
};

#endif  // METGET_LIBRARY_GEOMETRY_H_
