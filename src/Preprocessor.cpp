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
#include "Preprocessor.h"

#include <numeric>
#include <utility>

#include "Atmospheric.h"
#include "Constants.h"
#include "Logging.h"
#include "Vortex.h"

using namespace Gahm;

Preprocessor::Preprocessor(Atcf *atcf) : m_data(atcf) {}

int Preprocessor::run() {
  if (m_data->format() == Atcf::ASWIP) {
    Logging::warning(
        "You have elected to recompute parameters previously input into the "
        "code. Input will be overwritten.");
  }

  int ierr = 0;
  ierr = this->calculateOverlandTranslationVelocity();
  ierr += this->generateMissingPressureData();
  ierr += this->calculateRadii();
  ierr += this->computeParameters();
  return ierr;
}

/**
 * Compute the U/V translational velocities on sphere
 * @param d1 Previous AtcfLine object
 * @param d2 Current AtcfLine object
 * @param[out] uv u-speed
 * @param[out] vv v-speed
 * @param[out] uuvv uv-speed
 * @return error code
 */
StormMotion Preprocessor::computeTranslationSpeed(const AtcfLine &d1,
                                                  const AtcfLine &d2) {
  const auto [udis, vdis, uvdis] =
      Physical::sphericalDx(d1.lon(), d1.lat(), d2.lon(), d2.lat());

  const double dt = static_cast<double>(d2.datetime().toSeconds() -
                                        d1.datetime().toSeconds());

  auto uv = std::abs(udis / dt);
  if (d2.lon() - d1.lon() <= 0.0) uv *= -1.0;

  auto vv = std::abs(vdis / dt);
  if (d2.lat() - d1.lat() <= 0.0) vv *= -1.0;

  auto uuvv = uvdis / dt;
  auto direction = std::atan2(vv, uv);
  if (direction < 0) direction += Constants::twopi();

  return {uuvv, uv, vv, direction};
}

/**
 * Compute the translation velocities for all records in the storm
 * @return error code
 */
int Preprocessor::calculateOverlandTranslationVelocity() {
  for (auto it = m_data->data()->begin() + 1; it != m_data->data()->end();
       ++it) {
    auto r1 = *(it - 1);
    auto r2 = *(it);
    auto translation = Preprocessor::computeTranslationSpeed(r1, r2);

    if (translation.speed() <
        Units::convert(Units::Knot, Units::MetersPerSecond)) {
      it->stormMotion().set((it - 1)->stormMotion());
    } else {
      it->stormMotion().set(translation);
    }
  }
  m_data->data()->begin()->setStormMotion(
      (m_data->data()->begin() + 1)->stormMotion());
  return 0;
}

/**
 * Sets the isotach to have all radii at rmax
 * @param[inout] radii array of radii in each quadrant
 * @param[inout] quadFlag flag specifying if the quadrant has data
 * @param[in] rmax radius to isotach
 * @param[in] record record number
 * @param[in] isotach isotach number
 */
void Preprocessor::setAllRadiiToRmax(CircularArray<double, 4> &radii,
                                     CircularArray<bool, 4> &quadFlag,
                                     const double rmax, const size_t record,
                                     const size_t isotach) {
  std::fill(quadFlag.begin(), quadFlag.end(), 1);
  std::fill(radii.begin(), radii.end(), rmax);
  m_data->assumptions()->add(generate_assumption(
      Assumption::MAJOR,
      "No isotachs reported. Assuming a constant "
      "radius (RMAX). Record " +
          std::to_string(record) + ", isotach: " + std::to_string(isotach)));
}

/**
 * Sets the radii to half of the sum of the nonzero radii
 * @param[inout] radii array of radii in each quadrant
 * @param[in] radiisum sum of available radii
 * @param[in] record record number
 * @param[in] isotach isotach number
 */
void Preprocessor::setMissingRadiiToHalfNonzeroRadii(
    CircularArray<double, 4> &radii, const double radiisum, const size_t record,
    const size_t isotach) {
  Logging::debug("RADII:HALF:: " + std::to_string(radiisum) + " " +
                 std::to_string(radiisum * 0.5));
  for (auto i = 0; i < radii.size(); ++i) {
    if (radii.at(i) == 0.0) radii.set(i, radiisum * 0.5);
  }
  m_data->assumptions()->add(generate_assumption(
      Assumption::MAJOR,
      "One isotach reported. Missing radii are half "
      "the nonzero radius. Record " +
          std::to_string(record) + ", isotach: " + std::to_string(isotach)));
}

/**
 * Sets the missing radii to half of the average of the two specified radii
 * @param radii array of radii in each quadrant
 * @param radiisum sum of available radii
 * @param record record number
 * @param isotach isotach number
 */
void Preprocessor::setMissingRadiiToHalfOfAverageSpecifiedRadii(
    CircularArray<double, 4> &radii, const double radiisum, size_t record,
    size_t isotach) {
  Logging::debug(
      "RADII:HALFAVERAGE:: " +
      std::to_string(radiisum *
                     Units::convert(Units::Kilometer, Units::NauticalMile)) +
      " " +
      std::to_string(radiisum * 0.25 *
                     Units::convert(Units::Kilometer, Units::NauticalMile)));
  for (auto i = 0; i < radii.size(); ++i) {
    if (radii.at(i) == 0.0) radii.set(i, radiisum * 0.25);
  }
  m_data->assumptions()->add(generate_assumption(
      Assumption::MAJOR,
      "Two isotachs reported. Missing radii are half "
      "the average of the nonzero radii, Record " +
          std::to_string(record) + ", isotach: " + std::to_string(isotach)));
}

/**
 * Sets the radii to the average of the specified adjacent radii
 * @param radii array of radii for this isotach
 * @param lookup_radii radii specified in extended space
 * @param record record number
 * @param isotach isotach number
 */
void Preprocessor::setMissingRadiiToAverageOfAdjacentRadii(
    CircularArray<double, 4> &radii, size_t record, size_t isotach) {
  for (long j = 0; j < static_cast<long>(radii.size()); ++j) {
    if (radii.at(j) == 0.0) {
      Logging::debug("RADII:AVGADJACENT: " + std::to_string(radii.at(j - 1)) +
                     ", " + std::to_string(radii.at(j + 1)) + ", " +
                     std::to_string((radii.at(j - 1) + radii.at(j + 1)) * 0.5));
      radii.set(j, (radii.at(j - 1) + radii.at(j + 1)) * 0.5);
    }
  }
  m_data->assumptions()->add(generate_assumption(
      Assumption::MAJOR,
      "Three isotachs reported. Missing radius is half "
      "the nonzero radius on either side. Record " +
          std::to_string(record) + ", isotach: " + std::to_string(isotach)));
}

/**
 * Computes the isotach radii values when they are not fully specified by the
 * Atcf file
 * @return
 */
int Preprocessor::calculateRadii() {
  Logging::debug("Beginning to compute missing radii");
  for (auto ait = m_data->data()->begin(); ait != m_data->data()->end();
       ++ait) {
    for (size_t i = 0; i < ait->nIsotach(); ++i) {
      const double radiisum =
          std::accumulate(ait->isotach(i)->isotach_radius().begin(),
                          ait->isotach(i)->isotach_radius().end(), 0.0);
      ait->isotach(i)->generateQuadFlag();

      const unsigned int numNonzero =
          Preprocessor::countNonzeroIsotachs(ait, i);

      Logging::debug(
          "NumNonzero Isotachs: " + std::to_string(numNonzero) + " " +
          std::to_string(ait->isotach(i)->quadrant_flag().at(0)) + " " +
          std::to_string(ait->isotach(i)->quadrant_flag().at(1)) + " " +
          std::to_string(ait->isotach(i)->quadrant_flag().at(2)) + " " +
          std::to_string(ait->isotach(i)->quadrant_flag().at(3)));

      const size_t record = ait - m_data->data()->begin();

      switch (numNonzero) {
        case 0:
          this->setAllRadiiToRmax(ait->isotach(i)->isotach_radius(),
                                  ait->isotach(i)->quadrant_flag(),
                                  ait->radiusMaxWinds(), record, i);
          break;
        case 1:
          this->setMissingRadiiToHalfNonzeroRadii(
              ait->isotach(i)->isotach_radius(), radiisum, record, i);
          break;
        case 2:
          this->setMissingRadiiToHalfOfAverageSpecifiedRadii(
              ait->isotach(i)->isotach_radius(), radiisum, record, i);
          break;
        case 3:
          this->setMissingRadiiToAverageOfAdjacentRadii(
              ait->isotach(i)->isotach_radius(), record, i);
          break;
        case 4:
          //...No missing radii
          break;
        default:
          gahm_throw_exception("Number of radii specified is not applicable");
          break;
      }
    }
  }

  return 0;
}

unsigned Preprocessor::countNonzeroIsotachs(
    const std::vector<AtcfLine>::iterator &ait, size_t i) {
  unsigned numNonzero = 0;
  for (auto j = 0; j < 4; ++j) {
    if (ait->isotach(i)->quadrant_flag().at(j)) {
      numNonzero++;
    }
  }
  return numNonzero;
}

/**
 * Computes any missing pressure values in the specified Atcf data using the
 * specified method
 * @param[in] method method to use for hurricane pressure
 * @return error code
 */
int Preprocessor::generateMissingPressureData(
    const HurricanePressure::PressureMethod &method) {
  HurricanePressure hp(method);
  double vmax_global = 0.0;
  for (auto it = m_data->data()->begin(); it != m_data->data()->end(); ++it) {
    vmax_global = std::max(vmax_global, it->vmax());
    if (it->centralPressure() <= 0.0) {
      if (it == m_data->data()->begin()) {
        it->setCentralPressure(
            HurricanePressure::computeInitialPressureEstimate(it->vmax()));
        m_data->assumptions()->add(generate_assumption(
            Assumption::MINOR,
            "Pressure data was assumed using initial "
            "pressure estimate method. Record " +
                std::to_string(it - m_data->data()->begin())));
      } else {
        it->setCentralPressure(hp.computePressure(
            it->vmax(), vmax_global, (it - 1)->vmax(),
            (it - 1)->centralPressure(), it->lat(), it->stormMotion().speed()));
        m_data->assumptions()->add(generate_assumption(
            Assumption::MINOR,
            "Pressure was computed as " +
                std::to_string(it->centralPressure()) +
                "mb using vmax=" + std::to_string(it->vmax()) +
                "m/s, vmax_global=" + std::to_string(vmax_global) +
                "m/s, previous_pressure=" +
                std::to_string((it - 1)->centralPressure()) +
                "mb, lat=" + std::to_string(it->lat()) +
                ", speed=" + std::to_string(it->stormMotion().speed()) +
                "m/s, with method=" +
                HurricanePressure::pressureMethodString(hp.pressureMethod()) +
                " for record " + std::to_string(it - m_data->data()->begin())));
      }
    }
  }
  return 0;
}

int Preprocessor::computeParameters() {
  for (auto &a : *(m_data->data())) {
    //...Compute the global parameters for this period in the storm
    const StormMotion s(a.stormMotion().speed(), a.stormMotion().direction());
    a.setVmaxAtBoundaryLayer(
        Preprocessor::computeVmaxAtBoundaryLayer(a.vmax(), s.speed()));
    a.setHollandB(Atmospheric::calcHollandB(a.vmaxAtBoundaryLayer(),
                                            a.centralPressure(),
                                            Physical::backgroundPressure()));

    // Compute the de-translated wind speeds for each isotach and quadrant
    for (auto &iso : a.isotachs()) {
      for (auto quadrant = 0; quadrant < 4; ++quadrant) {
        iso.quadrant_isotach_wind_speed().set(
            quadrant, Preprocessor::computeIsotachDeTranslatedWindSpeed(
                          a, iso, quadrant));
        iso.quadrant_vmax_boundary_layer().set(quadrant,
                                               a.vmaxAtBoundaryLayer());
      }
    }
    Vortex v(&a, m_data->assumptions_sharedptr());
    v.computeRadiusToMaxWind();
  }
  return 0;
}

double Preprocessor::computeIsotachDeTranslatedWindSpeed(
    const AtcfLine &atcfData, const Isotach &iso, int quadrant) {
  const auto u_vr =
      iso.windSpeed() * std::cos(Constants::quadrantAngle(quadrant));
  const auto v_vr =
      iso.windSpeed() * std::sin(Constants::quadrantAngle(quadrant));
  const auto gamma =
      Preprocessor::computeGamma(atcfData.stormMotion(), u_vr, v_vr,
                                 atcfData.vmaxAtBoundaryLayer(), iso, quadrant);
  return std::sqrt(std::pow(u_vr - gamma * atcfData.stormMotion().u(), 2.0) +
                   std::pow(v_vr - gamma * atcfData.stormMotion().v(), 2.0)) /
         Physical::windReduction();
}

double Preprocessor::computeVmaxAtBoundaryLayer(
    const double vmax, const double stormForwardSpeed) {
  assert(stormForwardSpeed > 0.0);
  assert(vmax > 0.0);
  return (vmax - stormForwardSpeed) / Physical::windReduction();
}

double Preprocessor::computeGamma(const StormMotion &storm_motion,
                                  const double u_vr, const double v_vr,
                                  const double vmax_boundary_layer,
                                  const Isotach &iso, const size_t quadrant) {
  constexpr double windReductionSquared =
      Physical::windReduction() * Physical::windReduction();
  double gamma =
      ((2.0 * u_vr * storm_motion.u() + 2.0 * v_vr * storm_motion.v()) -
       std::sqrt(
           std::pow(
               2.0 * u_vr * storm_motion.u() + 2.0 * v_vr * storm_motion.v(),
               2.0) -
           4.0 *
               (std::pow(storm_motion.speed(), 2.0) -
                std::pow(vmax_boundary_layer, 2.0) * windReductionSquared) *
               std::pow(iso.windSpeed(), 2.0))) /
      (2.0 * (std::pow(storm_motion.speed(), 2.0) -
              std::pow(vmax_boundary_layer, 2.0) * windReductionSquared));
  return std::max(std::min(gamma, 1.0), 0.0);
}