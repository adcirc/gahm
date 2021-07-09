//
// Created by Zach Cobell on 1/23/21.
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
