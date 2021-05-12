#include "Preprocessor.h"
#include <numeric>
#include "Logging.h"
#include "Physical.h"

Preprocessor::Preprocessor(std::vector<AtcfLine> *data,
                           Assumptions *assumptions)
    : m_assumptions(assumptions), m_data(data) {}

int Preprocessor::run() {
  int ierr = 0;
  ierr = this->calculateOverlandTranslationVelocity();
  ierr += this->generateMissingPressureData();
  ierr += this->computeParameters();
  ierr += this->calculateRadii();
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
  for (auto it = m_data->begin() + 1; it != m_data->end(); ++it) {
    auto r1 = *(it - 1);
    auto r2 = *(it);
    double u, v, uv;
    int ierr = Preprocessor::uvTrans(r1, r2, u, v, uv);

    if (ierr != 0) {
      gahm_throw_exception("Error calculating UVTrans");
    }

    if (uv * Physical::ms2kt() < 1.0) {
      uv = 1.0 * Physical::kt2ms();
      it->setStormDirection((it - 1)->stormDirection());
    } else {
      double dir = std::atan2(u, v);
      if (dir < 0.0) dir += Physical::twopi();
      it->setStormDirection(dir * Physical::rad2deg());
    }
    it->setStormTranslationVelocities(u, v, uv);
    it->setStormSpeed(uv);
  }
  m_data->begin()->setStormDirection((m_data->begin() + 1)->stormDirection());
  m_data->begin()->setStormSpeed((m_data->begin() + 1)->stormSpeed());
  auto v = (m_data->begin() + 1)->stormTranslationVelocities();
  m_data->begin()->setStormTranslationVelocities(std::get<0>(v), std::get<1>(v),
                                                 std::get<2>(v));
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
void Preprocessor::setAllRadiiToRmax(CircularArray<double, 4> *radii,
                                     CircularArray<bool, 4> *quadFlag,
                                     const double rmax, const size_t record,
                                     const size_t isotach) {
  std::fill(quadFlag->begin(), quadFlag->end(), 1);
  std::fill(radii->begin(), radii->end(), rmax);
  this->m_assumptions->add(generate_assumption(
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
    CircularArray<double, 4> *radii, const double radiisum, const size_t record,
    const size_t isotach) {
  for (size_t i = 0; i < radii->size(); ++i) {
    if (radii->at(i) == 0.0) radii->set(i, radiisum * 0.5);
  }
  this->m_assumptions->add(generate_assumption(
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
    CircularArray<double, 4> *radii, const double radiisum, size_t record,
    size_t isotach) {
  for (size_t i = 0; i < radii->size(); ++i) {
    if (radii->at(i) == 0.0) radii->set(i, radiisum * 0.25);
  }
  this->m_assumptions->add(generate_assumption(
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
    CircularArray<double, 4> *radii, size_t record, size_t isotach) {
  for (long j = 0; j < radii->size(); ++j) {
    if (radii->at(j) == 0.0) {
      radii->set(j, (radii->at(j - 1) + radii->at(j + 1)) * 0.5);
    }
  }
  this->m_assumptions->add(generate_assumption(
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
  for (auto ait = m_data->begin(); ait != m_data->end(); ++ait) {
    for (size_t i = 0; i < ait->nIsotach(); ++i) {
      const auto radii = ait->cisotach(i)->crmax()->data();
      const double radiisum =
          std::accumulate(radii->cbegin(), radii->cend(), 0.0);
      ait->isotach(i)->generateQuadFlag();
      const int numNonzero =
          std::accumulate(ait->cisotach(i)->cquadFlag()->cbegin(),
                          ait->cisotach(i)->cquadFlag()->cend(), 0);
      const size_t record = ait - m_data->begin();

      switch (numNonzero) {
        case 0:
          this->setAllRadiiToRmax(ait->isotach(i)->rmax(),
                                  ait->isotach(i)->quadFlag(),
                                  ait->radiusMaxWinds(), record, i);
          break;
        case 1:
          this->setMissingRadiiToHalfNonzeroRadii(ait->isotach(i)->rmax(),
                                                  radiisum, record, i);
          break;
        case 2:
          this->setMissingRadiiToHalfOfAverageSpecifiedRadii(
              ait->isotach(i)->rmax(), radiisum, record, i);
          break;
        case 3:
          this->setMissingRadiiToAverageOfAdjacentRadii(ait->isotach(i)->rmax(),
                                                        record, i);
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
  for (auto it = m_data->begin(); it != m_data->end(); ++it) {
    vmax_global = std::max(vmax_global, it->vmax());
    if (it->mslp() <= 0.0) {
      if (it == m_data->begin()) {
        it->setMslp(
            HurricanePressure::computeInitialPressureEstimate(it->vmax()));
        m_assumptions->add(
            generate_assumption(Assumption::MINOR,
                                "Pressure data was assumed using initial "
                                "pressure estimate method. Record " +
                                    std::to_string(it - m_data->begin())));
      } else {
        it->setMslp(hp.computePressure(it->vmax(), vmax_global,
                                       (it - 1)->vmax(), (it - 1)->mslp(),
                                       it->lat(), it->uvTrans()));
        m_assumptions->add(generate_assumption(
            Assumption::MINOR,
            "Pressure was computed as " + std::to_string(it->mslp()) +
                "mb using vmax=" + std::to_string(it->vmax()) +
                "m/s, vmax_global=" + std::to_string(vmax_global) +
                "m/s, previous_pressure=" + std::to_string((it - 1)->mslp()) +
                "mb, lat=" + std::to_string(it->lat()) + ", speed=" +
                std::to_string(it->uvTrans()) + "m/s, with method=" +
                HurricanePressure::pressureMethodString(hp.pressureMethod()) +
                " for record " + std::to_string(it - m_data->begin())));
      }
    }
  }
  return 0;
}

int Preprocessor::computeParameters() {
  size_t rec_counter = 0;
  for (auto &a : *(m_data)) {
    for (size_t i = 0; i < a.nIsotach(); ++i) {
      rec_counter++;

      //...Check if the isotach is zero
      const double vr = a.isotach(i)->windSpeed() == 0.0
                            ? a.vmax()
                            : a.isotach(i)->windSpeed();

      size_t nquadrotat = 300;

      //...Compute the friction angle
      std::array<double, 4> quadRotateAngle(
          {25.0 * Physical::deg2rad(), 25.0 * Physical::deg2rad(),
           25.0 * Physical::deg2rad(), 25.0 * Physical::deg2rad()});
      if (i > 0) {
        nquadrotat = 1;
        for (size_t j = 0; j < 4; ++j) {
          quadRotateAngle[j] =
              Physical::frictionAngle(a.isotach(i)->isotachRadius()->at(j),
                                      a.isotach(0)->rmax()->at(j));
        }
      }

      const auto stormMotion =
          Preprocessor::computeStormMotion(a.stormSpeed(), a.stormDirection());

      const double vmaxBL =
          Preprocessor::computeVMaxBL(a.vmax(), stormMotion.uv);

      std::array<bool, 4> vmwBLflag = {false, false, false, false};

      //...Converge inward rotation angle
      for (size_t j = 0; j < nquadrotat; ++j) {
        Preprocessor::computeQuadrantVr(j, quadRotateAngle, vmwBLflag, vmaxBL,
                                        vr, stormMotion, a.isotach(i));
        Preprocessor::recomputeQuadrantVr(j, quadRotateAngle, vmwBLflag, vmaxBL,
                                          vr, a.vmax(), a.stormDirection(),
                                          stormMotion, a.isotach(i));
      }
    }
  }
  return 0;
}

double Preprocessor::computeGamma(const double uvr, const double vvr,
                                  const double vr,
                                  const StormMotion &stormMotion,
                                  const double vmaxBL) {
  const double g0 =
      (2.0 * uvr * Physical::ms2kt() * stormMotion.u * Physical::ms2kt() +
       2.0 * vvr * Physical::ms2kt() * stormMotion.v * Physical::ms2kt());
  const double g1 = std::pow(g0, 2.0);
  const double g2 =
      4.0 * (std::pow(stormMotion.uv * Physical::ms2kt(), 2.0) -
             std::pow(vmaxBL * Physical::ms2kt(), 2.0) *
                 Physical::windReduction() * Physical::windReduction());
  const double g3 = std::pow(vr * Physical::ms2kt(), 2.0);
  const double g4 =
      2.0 * (std::pow(stormMotion.uv * Physical::ms2kt(), 2.0) -
             std::pow(vmaxBL * Physical::ms2kt(), 2.0) *
                 Physical::windReduction() * Physical::windReduction());

  double g = (g0 - std::sqrt(g1 - g2 * g3)) / g4;
  g = std::max(std::min(g, 1.0), 0.0);
  return g;
}

double Preprocessor::computeEpsilonAngle(const double velocity,
                                         const double quadrantVectorAngle,
                                         const StormMotion &stormMotion) {
  double e =
      Physical::twopi() +
      std::atan2(velocity * std::sin(quadrantVectorAngle) + stormMotion.v,
                 velocity * std::cos(quadrantVectorAngle) + stormMotion.u);
  if (e > Physical::twopi()) e -= Physical::twopi();
  return e;
}

double Preprocessor::computeQuadrantVrWithGamma(
    const double vmaxBL, const double quadrantVectorAngles,
    const StormMotion &stormMotion, const double vr) {
  const double epsilonAngle = Preprocessor::computeEpsilonAngle(
      vmaxBL, quadrantVectorAngles, stormMotion);

  const double uvr = vr * std::cos(epsilonAngle);
  const double vvr = vr * std::sin(epsilonAngle);

  const double gamma =
      Preprocessor::computeGamma(uvr, vvr, vr, stormMotion, vmaxBL);

  const double qvr =
      std::sqrt(std::pow(uvr * Physical::ms2kt() -
                             gamma * stormMotion.u * Physical::ms2kt(),
                         2.0) +
                std::pow(vvr * Physical::ms2kt() -
                             gamma * stormMotion.v * Physical::ms2kt(),
                         2.0)) /
      Physical::windReduction();
  return qvr;
}

double Preprocessor::computeQuadrantVrWithoutGamma(
    const double quadrantVectorAngle, const StormMotion &stormMotion,
    const double vr) {
  const double qvr_1 = (stormMotion.u * std::cos(quadrantVectorAngle) +
                        stormMotion.v * std::sin(quadrantVectorAngle));
  const double qvr =
      (-2.0 * qvr_1 +
       std::sqrt(4.0 * std::pow(qvr_1, 2.0) -
                 4.0 * (std::pow(stormMotion.uv, 2.0) - std::pow(vr, 2.0)))) /
      2.0;
  return qvr;
}

Preprocessor::StormMotion Preprocessor::computeStormMotion(
    const double speed, const double direction) {
  const double stormMotion =
      1.5 * std::pow(speed * Physical::ms2kt(), 0.63) * Physical::kt2ms();
  const double stormMotionU =
      std::sin(direction * Physical::deg2rad()) * stormMotion;
  const double stormMotionV =
      std::cos(direction * Physical::deg2rad()) * stormMotion;
  return {stormMotion, stormMotionU, stormMotionV};
}

double Preprocessor::computeVMaxBL(const double vmax,
                                   const double stormMotion) {
  return (vmax - stormMotion) / Physical::windReduction();
}

double Preprocessor::computeQuadrantVectorAngle(
    const size_t index, const std::array<double, 4> quadRotateAngle) {
  assert(index < 4);
  return Physical::quadrantAngle(index) +
         (Physical::halfpi() + quadRotateAngle[index]);
}

void Preprocessor::computeQuadrantVr(
    const size_t quadrant, const std::array<double, 4> &quadRotateAngle,
    const std::array<bool, 4> &vmwBLflag, const double vmaxBL, const double vr,
    const StormMotion &stormMotion, Isotach *isotach) {
  for (size_t k = 0; k < 4; ++k) {
    const double quadrantVectorAngle =
        Preprocessor::computeQuadrantVectorAngle(k, quadRotateAngle);

    if (quadrant == 0 || !vmwBLflag[quadrant]) {
      const double qvr = Preprocessor::computeQuadrantVrWithGamma(
          vmaxBL, quadrantVectorAngle, stormMotion, vr);
      isotach->quadrantVr()->set(k, qvr);
    } else {
      isotach->quadrantVr()->set(k, vmaxBL);
    }
  }
}

void Preprocessor::recomputeQuadrantVr(
    const size_t quadrant, const std::array<double, 4> &quadRotateAngle,
    std::array<bool, 4> &vmwBLflag, const double vmaxBL, const double vr,
    const double vmax, const double stormDirection,
    const StormMotion &stormMotion, Isotach *isotach) {
  for (size_t k = 0; k < 4; ++k) {
    if (isotach->cquadrantVr()->at(k) > vmax || quadrant == 1) {
      if (quadrant == 1) vmwBLflag[k] = true;

      const double quadrantVectorAngle =
          Preprocessor::computeQuadrantVectorAngle(k, quadRotateAngle);

      double qvr = Preprocessor::computeQuadrantVrWithoutGamma(
          quadrantVectorAngle, stormMotion, vr);

      if (isotach->cisotachRadius()->at(k) > 0) {
        const double epsilonAngle = Preprocessor::computeEpsilonAngle(
            qvr, quadrantVectorAngle, stormMotion);

        qvr /= Physical::windReduction();

        isotach->quadrantVr()->set(k, qvr);
        isotach->vmaxBl()->set(k, qvr);

      } else {
        isotach->vmaxBl()->set(k, vmaxBL);
        const double uvr = vr * std::cos(stormDirection);
        const double vvr = vr * std::sin(stormDirection);
        const double gamma =
            Preprocessor::computeGamma(uvr, vvr, vr, stormMotion, vmaxBL);
        const double qvr2 =
            vr - gamma * stormMotion.uv / Physical::windReduction();
        isotach->quadrantVr()->set(k, qvr2);
      }
    } else {
      isotach->vmaxBl()->set(k, vmaxBL);
    }
  }
}
