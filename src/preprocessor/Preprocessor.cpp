// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
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
#include "Preprocessor.h"

#include "gahm/GahmSolver.h"
#include "physical/Earth.h"

namespace Gahm {

/**
 * Constructor for the preprocessor class
 * @param atcf Pointer to the AtcfFile object
 */
Preprocessor::Preprocessor(Gahm::Atcf::AtcfFile *atcf) : m_atcf(atcf) {
  if (!m_atcf->isRead()) {
    m_atcf->read();
  }
}

/*
 * Prepares the ATCF data for the solver
 */
void Preprocessor::prepareAtcfData() {
  this->fillMissingAtcfData();
  this->computeStormTranslationVelocities();
  this->computeBoundaryLayerWindspeed();
}

/**
 * Fills in missing quadrant data in the Isotach objects
 */
void Preprocessor::fillMissingAtcfData() {
  for (auto &snap : *m_atcf) {
    for (auto &isotach : snap.getIsotachs()) {
      const auto n_missing = [&]() {
        return std::count_if(
            isotach.getQuadrants().begin(), isotach.getQuadrants().end(),
            [](auto &q) { return q.getIsotachRadius() == 0.0; });
      }();
      if (n_missing == 1) {
        // We find the missing one and take the average of its neighbors
        auto missing = std::find_if(
            isotach.getQuadrants().begin(), isotach.getQuadrants().end(),
            [](auto &q) { return q.getIsotachRadius() == 0.0; });
        auto missing_index = static_cast<int>(
            std::distance(isotach.getQuadrants().begin(), missing));
        auto left_index = missing_index + 3;
        auto right_index = missing_index + 1;
        auto mean = (isotach.getQuadrants()[left_index].getIsotachRadius() +
                     isotach.getQuadrants()[right_index].getIsotachRadius()) /
                    2.0;
        missing->setIsotachRadius(mean);
      } else if (n_missing == 2) {
        // We need to find the two missing quadrants and take the average of the
        // other two
        auto mean_not_missing = [&]() {
          auto sum = 0.0;
          for (auto &q : isotach.getQuadrants()) {
            if (q.getIsotachRadius() != 0.0) {
              sum += q.getIsotachRadius();
            }
          }
          return sum / 2.0;
        }();
        for (auto &q : isotach.getQuadrants()) {
          if (q.getIsotachRadius() == 0.0) {
            q.setIsotachRadius(mean_not_missing);
          }
        }
      } else if (n_missing == 3) {
        // We need to find the quadrant that is set and copy its value to the
        // missing three
        auto not_missing = std::find_if(
            isotach.getQuadrants().begin(), isotach.getQuadrants().end(),
            [](auto &q) { return q.getIsotachRadius() != 0.0; });
        for (auto &q : isotach.getQuadrants()) {
          if (q.getIsotachRadius() == 0.0) {
            q.setIsotachRadius(not_missing->getIsotachRadius());
          }
        }
      } else if (n_missing == 4) {
        // We set all four quadrants to the radius of the maximum wind
        for (auto &q : isotach.getQuadrants()) {
          q.setIsotachRadius(snap.radiusToMaxWinds());
        }
      }
    }
  }
}

/*
 * Calculates the radius to maximum wind speed and GAHM B for each quadrant
 */
void Preprocessor::solve() {
  for (auto &snap : *m_atcf) {
    for (auto &isotach : snap.getIsotachs()) {
      for (auto &quadrant : isotach.getQuadrants()) {
        double isotach_radius = quadrant.getIsotachRadius();
        double isotach_speed = isotach.getWindSpeed();
        double vmax = snap.vmax();
        double p_min = snap.centralPressure();
        double p_back = snap.backgroundPressure();
        double latitude = snap.position().y();
        Gahm::Solver::GahmSolver solver(isotach_radius, isotach_speed, vmax,
                                        p_min, p_back, latitude);
        solver.solve();

        double solution_gahm_rmax = solver.rmax();
        double solution_gahm_b = solver.bg();

        quadrant.setRadiusToMaxWindSpeed(solution_gahm_rmax);
        quadrant.setGahmHollandB(solution_gahm_b);
      }
    }
  }
}

/*
 * Computes the storm translation velocities
 */
void Preprocessor::computeStormTranslationVelocities() {
  for (auto it = m_atcf->begin(); it != m_atcf->end(); ++it) {
    if (it == m_atcf->begin()) {
      auto next = std::next(it);
      if (next == m_atcf->end()) {
        it->setTranslation({0.0, 0.0});
      } else {
        auto translation = Preprocessor::getTranslation(*it, *next);
        it->setTranslation(translation);
      }
    } else {
      auto prev = std::prev(it);
      auto translation = Preprocessor::getTranslation(*prev, *it);
      it->setTranslation(translation);
    }
  }
}

/*
 * Returns the translation object for two snaps
 * @param now Current snap
 * @param next Next snap
 * @return Translation object
 */
Gahm::Atcf::StormTranslation Preprocessor::getTranslation(
    const Gahm::Atcf::AtcfSnap &now, const Gahm::Atcf::AtcfSnap &next) {
  const auto dt =
      static_cast<double>(next.date().toSeconds() - now.date().toSeconds());
  const auto [u_dis, v_dis, uv_dis] = Gahm::Physical::Earth::sphericalDx(
      now.position().x(), now.position().y(), next.position().x(),
      next.position().y());
  auto uu = std::abs(u_dis / dt);
  if (next.position().x() - now.position().x() < 0.0) uu *= -1.0;

  double vv = std::abs(v_dis / dt);
  if (next.position().y() - now.position().y() < 0.0) vv *= -1.0;

  double uv = uv_dis / dt;
  double dir = std::atan2(vv, uu);
  if (dir < 0.0) dir += Gahm::Physical::Constants::twoPi();

  return {uv, dir};
}

/*
 * Computes the boundary layer windspeed for each snap
 */
void Preprocessor::computeBoundaryLayerWindspeed() {
  for (auto &snap : *m_atcf) {
    auto transit_speed = snap.translation().transitSpeed();
    for (auto &iso : snap.getIsotachs()) {
      for (auto &quad : iso.getQuadrants()) {
        quad.setVmaxAtBoundaryLayer(
            Preprocessor::compute_simple_vmax_at_boundary_layer(snap.vmax(),
                                                                transit_speed));
      }
    }
  }
}

}  // namespace Gahm