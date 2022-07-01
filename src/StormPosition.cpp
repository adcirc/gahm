//
// Created by Zach Cobell on 5/8/22.
//

#include "StormPosition.h"

#include <cmath>

using namespace Gahm;

StormPosition::StormPosition()
    : m_longitude(0.0),
      m_latitude(0.0),
      m_forward_speed(0.0),
      m_transit_speed(0.0),
      m_transit_speed_u(0.0),
      m_transit_speed_v(0.0),
      m_transit_direction(0.0) {}

StormPosition::StormPosition(double longitude, double latitude,
                             double forwardSpeed, double transitDirection)
    : m_longitude(longitude),
      m_latitude(latitude),
      m_forward_speed(forwardSpeed),
      m_transit_speed(0.0),
      m_transit_speed_u(0.0),
      m_transit_speed_v(0.0),
      m_transit_direction(transitDirection) {
  this->computeStormTransitSpeeds();
}

StormPosition::StormPosition(const StormPosition& s) {
  m_transit_direction = s.m_transit_direction;
  m_transit_speed = s.m_transit_speed;
  m_transit_speed_u = s.m_transit_speed_u;
  m_transit_speed_v = s.m_transit_speed_v;
  m_forward_speed = s.m_forward_speed;
  m_longitude = s.m_longitude;
  m_latitude = s.m_latitude;
}

StormPosition& StormPosition::operator=(const StormPosition& s) = default;

/**
 * Computes the storm transit speeds consistent with NOAA technical report
 * NWS 23 - Meteorological Criteria for Standard Project Hurricane and
 * Probable Maximum Hurricane Windfields, Gulf and East Coasts of the United
 * States. Washington, D.C. - September 1979
 *
 * Storm transit speed is equal to:
 * t = 1.5 * T^0.63 * T0^0.37
 *
 * where:
 *    T  = forward speed of the storm
 *    T0 = 1 when the units are knots
 *         0.514791 when then units are m/s
 *         1.853248 when the units are mi/hr
 *
 */
void StormPosition::computeStormTransitSpeeds() {
  static const double coefficient_metersPerSecond = std::pow(1.853248, 0.37);
  m_transit_speed =
      1.5 * std::pow(m_forward_speed, 0.63) * coefficient_metersPerSecond;
  m_transit_speed_u = std::cos(m_transit_direction) * m_transit_speed;
  m_transit_speed_v = std::sin(m_transit_direction) * m_transit_speed;
}

double StormPosition::longitude() const { return m_longitude; }

double StormPosition::latitude() const { return m_latitude; }

double StormPosition::transitSpeed() const { return m_transit_speed; }

double StormPosition::transitDirection() const { return m_transit_direction; }

std::tuple<double, double> StormPosition::transitSpeedComponents() const {
  return {m_transit_speed_u, m_transit_speed_v};
}
void StormPosition::setLongitude(double longitude) { m_longitude = longitude; }
void StormPosition::setLatitude(double latitude) { m_latitude = latitude; }

void StormPosition::setForwardSpeed(double forwardSpeed) {
  m_forward_speed = forwardSpeed;
  this->computeStormTransitSpeeds();
}

void StormPosition::setTransitDirection(double transitDirection) {
  m_transit_direction = transitDirection;
  this->computeStormTransitSpeeds();
}
void StormPosition::setSpeedAndDirection(double forwardSpeed,
                                         double direction) {
  m_forward_speed = forwardSpeed;
  m_transit_direction = direction;
  this->computeStormTransitSpeeds();
}

double StormPosition::forwardSpeed() const { return m_forward_speed; }
