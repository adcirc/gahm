//
// Created by Zach Cobell on 3/8/23.
//

#ifndef GAHM_SRC_ATCF_STORMPOSITION_H_
#define GAHM_SRC_ATCF_STORMPOSITION_H_

namespace Gahm::Atcf {

class StormPosition {
 public:
  StormPosition() = default;

  StormPosition(double x, double y) : m_x(x), m_y(y) {}

  [[nodiscard]] double x() const { return m_x; }
  void setX(double x) { m_x = x; }

  [[nodiscard]] double y() const { return m_y; }
  void setY(double y) { m_y = y; }

 private:
  double m_x;
  double m_y;
};

}  // namespace Gahm::Atcf

#endif  // GAHM_SRC_ATCF_STORMPOSITION_H_
