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
int Preprocessor::uvTrans(const AtcfLine &d1, const AtcfLine &d2, double &uv,
                          double &vv, double &uuvv) {
  const auto dxdy =
      Physical::sphericalDx(d1.lon(), d1.lat(), d2.lon(), d2.lat());
  const double dt = static_cast<double>(d2.datetime().toSeconds() -
                                        d1.datetime().toSeconds());

  uv = std::abs(std::get<0>(dxdy) / dt);
  if (d2.lon() - d1.lon() <= 0.0) uv *= -1.0;

  vv = std::abs(std::get<1>(dxdy) / dt);
  if (d2.lat() - d1.lat() <= 0.0) vv *= -1.0;

  uuvv = std::get<2>(dxdy) / dt;

  return 0;
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
    double u, v, uv;
    int ierr = Preprocessor::uvTrans(r1, r2, u, v, uv);

    if (ierr != 0) {
      gahm_throw_exception("Error calculating UVTrans");
    }

    if (uv * Units::convert(Units::MetersPerSecond, Units::Knot) < 1.0) {
      uv = 1.0 * Units::convert(Units::Knot, Units::MetersPerSecond);
      it->setStormDirection((it - 1)->stormDirection());
    } else {
      double dir = std::atan2(u, v);
      if (dir < 0.0) dir += Constants::twopi();
      it->setStormDirection(dir * Units::convert(Units::Radian, Units::Degree));
    }
    it->setStormTranslationVelocities(u, v, uv);
    it->setStormSpeed(uv);
  }
  m_data->data()->begin()->setStormDirection(
      (m_data->data()->begin() + 1)->stormDirection());
  m_data->data()->begin()->setStormSpeed(
      (m_data->data()->begin() + 1)->stormSpeed());
  auto v = (m_data->data()->begin() + 1)->stormTranslationVelocities();
  m_data->data()->begin()->setStormTranslationVelocities(
      std::get<0>(v), std::get<1>(v), std::get<2>(v));
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
          std::accumulate(ait->isotach(i).isotachRadius().begin(),
                          ait->isotach(i).isotachRadius().end(), 0.0);
      ait->isotach(i).generateQuadFlag();

      const unsigned int numNonzero =
          Preprocessor::countNonzeroIsotachs(ait, i);

      Logging::debug("NumNonzero Isotachs: " + std::to_string(numNonzero) +
                     " " + std::to_string(ait->isotach(i).quadFlag().at(0)) +
                     " " + std::to_string(ait->isotach(i).quadFlag().at(1)) +
                     " " + std::to_string(ait->isotach(i).quadFlag().at(2)) +
                     " " + std::to_string(ait->isotach(i).quadFlag().at(3)));

      const size_t record = ait - m_data->data()->begin();

      switch (numNonzero) {
        case 0:
          this->setAllRadiiToRmax(ait->isotach(i).isotachRadius(),
                                  ait->isotach(i).quadFlag(),
                                  ait->radiusMaxWinds(), record, i);
          break;
        case 1:
          this->setMissingRadiiToHalfNonzeroRadii(
              ait->isotach(i).isotachRadius(), radiisum, record, i);
          break;
        case 2:
          this->setMissingRadiiToHalfOfAverageSpecifiedRadii(
              ait->isotach(i).isotachRadius(), radiisum, record, i);
          break;
        case 3:
          this->setMissingRadiiToAverageOfAdjacentRadii(
              ait->isotach(i).isotachRadius(), record, i);
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
    if (ait->isotach(i).quadFlag().at(j)) {
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
            (it - 1)->centralPressure(), it->lat(), it->uvTrans()));
        m_data->assumptions()->add(generate_assumption(
            Assumption::MINOR,
            "Pressure was computed as " +
                std::to_string(it->centralPressure()) +
                "mb using vmax=" + std::to_string(it->vmax()) +
                "m/s, vmax_global=" + std::to_string(vmax_global) +
                "m/s, previous_pressure=" +
                std::to_string((it - 1)->centralPressure()) +
                "mb, lat=" + std::to_string(it->lat()) + ", speed=" +
                std::to_string(it->uvTrans()) + "m/s, with method=" +
                HurricanePressure::pressureMethodString(hp.pressureMethod()) +
                " for record " + std::to_string(it - m_data->data()->begin())));
      }
    }
  }
  return 0;
}

int Preprocessor::computeParameters() {
  size_t rec_counter = 0;
  for (auto &a : *(m_data->data())) {
    //...Compute the global parameters for this period in the storm
    const auto stormMotion =
        Preprocessor::computeStormMotion(a.stormSpeed(), a.stormDirection());
    a.setVmaxBl(Preprocessor::computeVMaxBL(a.vmax(), stormMotion.uv));

    a.setHollandB(Physical::calcHollandB(a.vmaxBl(), a.centralPressure(),
                                         Physical::backgroundPressure()));
    for (size_t i = 0; i < a.nIsotach(); ++i) {
      rec_counter++;

      //...Check if the isotach is zero
      const double vr =
          a.isotach(i).windSpeed() == 0.0 ? a.vmax() : a.isotach(i).windSpeed();

      //...Compute the inward rotation angles
      const std::array<double, 4> quadRotateAngle =
          Preprocessor::computeQuadRotateAngle(a, i);

      //...Initialize the flag that checks for violations in vmax vs vmaxbl
      std::array<bool, 4> vmwBLflag = {false, false, false, false};

      //...Fill the holland b and phi in all quadrants for this isotach
      a.isotach(i).hollandB().fill(a.hollandB());
      a.isotach(i).phi().fill(1.0);

      //...Converge inward rotation angle
      Preprocessor::convergeInwardRotationAngle(rec_counter, i, a, stormMotion,
                                                vr, quadRotateAngle, vmwBLflag);
    }
  }
  return 0;
}

void Preprocessor::convergeInwardRotationAngle(
    size_t rec_counter, size_t i, AtcfLine &a,
    const Preprocessor::StormMotion &stormMotion, const double vr,
    const std::array<double, 4> &quadRotateAngle,
    std::array<bool, 4> &vmwBLflag) const {
  const size_t nquadrotat = i == 0 ? 300 : 1;
  // const size_t nquadrotat = 1;
  for (auto j = 0; j < nquadrotat; ++j) {
    Preprocessor::computeQuadrantVrLoop(j, quadRotateAngle, vmwBLflag,
                                        a.vmaxBl(), vr, stormMotion,
                                        a.isotach(i));
    Preprocessor::recomputeQuadrantVrLoop(j, quadRotateAngle, vmwBLflag,
                                          a.vmaxBl(), vr, a.stormDirection(),
                                          stormMotion, a.isotach(i));

    //...Create a new vortex object and compute the radius to max wind
    // in each quadrant of the storm

    Vortex v(&a, rec_counter, i, m_data->assumptions_sharedptr());
    v.computeRadiusToWind();
  }
}

std::array<double, 4> Preprocessor::computeQuadRotateAngle(const AtcfLine &a,
                                                           size_t i) {
  constexpr double initial_quadRotateAngle =
      25.0 * Units::convert(Units::Degree, Units::Radian);
  std::array<double, 4> quadRotateAngle{
      initial_quadRotateAngle, initial_quadRotateAngle, initial_quadRotateAngle,
      initial_quadRotateAngle};
  if (i > 0) {
    for (auto j = 0; j < 4; ++j) {
      quadRotateAngle[j] = Physical::frictionAngle(
          a.isotach(i).isotachRadius().at(j), a.isotach(i).rmax().at(j));
    }
  }
  return quadRotateAngle;
}

void Preprocessor::computeQuadrantVrLoop(
    const size_t quadrotindex, const std::array<double, 4> &quadRotateAngle,
    const std::array<bool, 4> &vmwBLflag, const double vmaxBL, const double vr,
    const StormMotion &stormMotion, Isotach &isotach) {
  for (auto k = 0; k < 4; ++k) {
    const double quadrantVectorAngle =
        Preprocessor::computeQuadrantVectorAngle(k, quadRotateAngle);

    if (quadrotindex == 0 || !vmwBLflag[k]) {
      const double qvr = Preprocessor::computeQuadrantVrValue(
          vmaxBL, quadrantVectorAngle, stormMotion, vr);
      isotach.quadrantVr().set(k, qvr);
    }
  }
}

double Preprocessor::computeQuadrantVrValue(const double vmaxBL,
                                            const double quadrantVectorAngles,
                                            const StormMotion &stormMotion,
                                            const double vr) {
  const double epsilonAngle = Preprocessor::computeEpsilonAngle(
      vmaxBL, quadrantVectorAngles, stormMotion);

  const double uvr = vr * std::cos(epsilonAngle);
  const double vvr = vr * std::sin(epsilonAngle);

  const double gamma =
      Preprocessor::computeGamma(uvr, vvr, vr, stormMotion, vmaxBL);

  constexpr double mps2kt = Units::convert(Units::MetersPerSecond, Units::Knot);

  const double qvr =
      std::sqrt(std::pow(uvr * mps2kt - gamma * stormMotion.u * mps2kt, 2.0) +
                std::pow(vvr * mps2kt - gamma * stormMotion.v * mps2kt, 2.0)) /
      Physical::windReduction();
  return qvr * Units::convert(Units::Knot, Units::MetersPerSecond);
}

void Preprocessor::recomputeQuadrantVrLoop(
    const size_t quadrotindex, const std::array<double, 4> &quadRotateAngle,
    std::array<bool, 4> &vmwBLflag, const double vmaxBL, const double vr,
    const double stormDirection, const StormMotion &stormMotion,
    Isotach &isotach) {
  constexpr double deg2rad = Units::convert(Units::Degree, Units::Radian);
  for (auto k = 0; k < 4; ++k) {
    if (isotach.quadrantVr().at(k) > vmaxBL || vmwBLflag[k]) {
      if (quadrotindex == 1) vmwBLflag[k] = true;

      if (!isotach.isotachRadiusNullInInput().at(k)) {
        const double quadrantVectorAngle =
            Preprocessor::computeQuadrantVectorAngle(k, quadRotateAngle);
        double qvr = Preprocessor::computeQuadrantVrValue(quadrantVectorAngle,
                                                          stormMotion, vr);
        qvr /= Physical::windReduction();
        isotach.quadrantVr().set(k, qvr);
        isotach.vmaxBl().set(k, qvr);
      } else {
        isotach.vmaxBl().set(k, vmaxBL);
        const double uvr = vr * std::cos(stormDirection * deg2rad);
        const double vvr = vr * std::sin(stormDirection * deg2rad);
        const double gamma =
            Preprocessor::computeGamma(uvr, vvr, vr, stormMotion, vmaxBL);
        const double qvr2 =
            (vr - gamma * stormMotion.uv) / Physical::windReduction();
        isotach.quadrantVr().set(k, qvr2);
      }
    } else {
      isotach.vmaxBl().set(k, vmaxBL);
    }
  }
}

double Preprocessor::computeQuadrantVrValue(const double quadrantVectorAngle,
                                            const StormMotion &stormMotion,
                                            const double vr) {
  constexpr double mps2kt = Units::convert(Units::MetersPerSecond, Units::Knot);

  const double qvr_1 = (stormMotion.u * mps2kt * std::cos(quadrantVectorAngle) +
                        stormMotion.v * mps2kt * std::sin(quadrantVectorAngle));
  const double qvr =
      (-2.0 * qvr_1 + std::sqrt(4.0 * std::pow(qvr_1, 2.0) -
                                4.0 * (std::pow(stormMotion.uv * mps2kt, 2.0) -
                                       std::pow(vr * mps2kt, 2.0)))) /
      2.0;
  return qvr * Units::convert(Units::Knot, Units::MetersPerSecond);
}

double Preprocessor::computeGamma(const double uvr, const double vvr,
                                  const double vr,
                                  const StormMotion &stormMotion,
                                  const double vmaxBL) {
  constexpr double ms2kt = Units::convert(Units::MetersPerSecond, Units::Knot);

  const double g0 = (2.0 * uvr * ms2kt * stormMotion.u * ms2kt +
                     2.0 * vvr * ms2kt * stormMotion.v * ms2kt);
  const double g1 = std::pow(g0, 2.0);
  const double g2 =
      4.0 * (std::pow(stormMotion.uv * ms2kt, 2.0) -
             std::pow(vmaxBL * ms2kt, 2.0) * Physical::windReduction() *
                 Physical::windReduction());
  const double g3 = std::pow(vr * ms2kt, 2.0);
  const double g4 =
      2.0 * (std::pow(stormMotion.uv * ms2kt, 2.0) -
             std::pow(vmaxBL * ms2kt, 2.0) * Physical::windReduction() *
                 Physical::windReduction());

  double g = (g0 - std::sqrt(g1 - g2 * g3)) / g4;
  g = std::max(std::min(g, 1.0), 0.0);
  return g;
}

double Preprocessor::computeEpsilonAngle(const double velocity,
                                         const double quadrantVectorAngle,
                                         const StormMotion &stormMotion) {
  double e =
      Constants::twopi() +
      std::atan2(velocity * std::sin(quadrantVectorAngle) + stormMotion.v,
                 velocity * std::cos(quadrantVectorAngle) + stormMotion.u);
  if (e > Constants::twopi()) e -= Constants::twopi();
  return e;
}

Preprocessor::StormMotion Preprocessor::computeStormMotion(
    const double speed, const double direction) {
  double stormMotion =
      1.5 *
      std::pow(speed * Units::convert(Units::MetersPerSecond, Units::Knot),
               0.63) *
      Units::convert(Units::Knot, Units::MetersPerSecond);
  double stormMotionU =
      std::sin(direction * Units::convert(Units::Degree, Units::Radian)) *
      stormMotion;
  double stormMotionV =
      std::cos(direction * Units::convert(Units::Degree, Units::Radian)) *
      stormMotion;
  return {stormMotionU, stormMotionV, stormMotion};
}

double Preprocessor::computeVMaxBL(const double vmax,
                                   const double stormMotion) {
  return (vmax - stormMotion) / Physical::windReduction();
}

double Preprocessor::computeQuadrantVectorAngle(
    const size_t index, const std::array<double, 4> quadRotateAngle) {
  assert(index < 4);
  return Constants::quadrantAngle(index) +
         (Constants::halfpi() + quadRotateAngle[index]);
}
