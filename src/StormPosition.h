//
// Created by Zach Cobell on 5/8/22.
//

#ifndef GAHM2_SRC_STORMPOSITION_H_
#define GAHM2_SRC_STORMPOSITION_H_

#include <tuple>

namespace Gahm {
class StormPosition {
 public:
  StormPosition() = default;

  StormPosition(double longitude, double latitude, double forwardSpeed,
                double transitDirection);

  [[nodiscard]] double longitude() const;
  void setLongitude(double longitude);

  [[nodiscard]] double latitude() const;
  void setLatitude(double latitude);

  [[nodiscard]] double transitSpeed() const;

  double forwardSpeed() const;
  void setForwardSpeed(double forwardSpeed);

  [[nodiscard]] double transitDirection() const;
  void setTransitDirection(double transitDirection);

  void setSpeedAndDirection(double forwardSpeed, double direction);

  [[nodiscard]] std::tuple<double, double> transitSpeedComponents() const;

  void computeStormTransitSpeeds();

 private:
  double m_longitude;
  double m_latitude;
  double m_forward_speed;
  double m_transit_direction;
  double m_transit_speed;
  double m_transit_speed_u;
  double m_transit_speed_v;
};
}  // namespace Gahm
#endif  // GAHM2_SRC_STORMPOSITION_H_
