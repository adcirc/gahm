#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <vector>
#include "Assumption.h"
#include "Atcf.h"

class Preprocessor {
 public:
  Preprocessor(std::vector<AtcfLine> *data, Assumptions *assumptions);

  int run();

 private:
  int uvTrans(const AtcfLine &d1, const AtcfLine &d2, double &uv, double &vv,
              double &uuvv);
  int calculateOverlandTranslationVelocity();

  void setAllRadiiToRmax(CircularArray<double, 4> *radii,
                         CircularArray<bool, 4> *quadFlag, const double rmax,
                         const size_t record, const size_t isotach);

  void setMissingRadiiToHalfNonzeroRadii(CircularArray<double, 4> *radii,
                                         const double radiisum,
                                         const size_t record,
                                         const size_t isotach);

  void setMissingRadiiToHalfOfAverageSpecifiedRadii(
      CircularArray<double, 4> *radii, const double radiisum, size_t record,
      size_t isotach);

  void setMissingRadiiToAverageOfAdjacentRadii(CircularArray<double, 4> *radii,
                                               size_t record, size_t isotach);

  int calculateRadii();

  int computeParameters();

  int generateMissingPressureData(const HurricanePressure::PressureMethod
                                      &method = HurricanePressure::KNAFFZEHR);

  struct StormMotion {
    double u;
    double v;
    double uv;
    StormMotion(double ui, double vi, double uvi) : u(ui), v(vi), uv(uvi) {}
  };

  static StormMotion computeStormMotion(double speed, double direction);

  static double computeGamma(double uvr, double vvr, double vr,
                             const StormMotion &stormMotion, double vmaxBL);

  static double computeEpsilonAngle(double velocity, double quadrantVectorAngle,
                                    const StormMotion &stormMotion);

  static double computeQuadrantVrWithGamma(double vmaxBL,
                                           double quadrantVectorAngles,
                                           const StormMotion &stormMotion,
                                           double vr);

  static double computeQuadrantVrWithoutGamma(const double quadrantVectorAngle,
                                              const StormMotion &stormMotion,
                                              const double vr);

  static double computeVMaxBL(double vmax, double stormMotion);

  static double computeQuadrantVectorAngle(
      size_t index, const std::array<double, 4> quadRotateAngle);

  static void computeQuadrantVr(size_t quadrant,
                                const std::array<double, 4> &quadRotateAngle,
                                const std::array<bool, 4> &vmwBLflag,
                                double vmaxBL, double vr,
                                const StormMotion &stormMotion,
                                Isotach *isotach);

  static void recomputeQuadrantVr(
      const size_t quadrant, const std::array<double, 4> &quadRotateAngle,
      std::array<bool, 4> &vmwBLflag, const double vmaxBL, const double vr,
      const double vmax, const double stormDirection,
      const StormMotion &stormMotion, Isotach *isotach);

  Assumptions *m_assumptions;
  std::vector<AtcfLine> *m_data;
};

#endif  // PREPROCESSOR_H
