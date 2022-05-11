//
// Created by Zach Cobell on 5/8/22.
//

#include "StormQuadrant.h"

#include "Constants.h"

std::tuple<int, double> Gahm::StormQuadrant::getBaseQuadrant(double angle) {
  angle = angle < 0.0 ? angle += Constants::twopi() : angle;
  if (angle > Constants::twopi()) angle -= Constants::twopi();
  const int quad =
      static_cast<int>(std::floor(std::fmod(angle / Constants::halfpi(), 4)));
  const double rem = angle - quad * Constants::halfpi();
  return {quad, rem};
}
