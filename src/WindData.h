//
// Created by Zach Cobell on 1/21/21.
//

#ifndef METGET_LIBRARY_WINDDATA_H_
#define METGET_LIBRARY_WINDDATA_H_

#include <array>
#include <cstdlib>
#include <vector>

#include "StormParameters.h"

class WindData {
 public:
  WindData();

  explicit WindData(size_t n);

  WindData(size_t n, double u, double v, double p);

  const std::vector<double> &u() const;
  const std::vector<double> &v() const;
  const std::vector<double> &p() const;
  size_t size() const;

  void set(size_t index, double u, double v, double p);

  void setU(size_t index, double value);
  void setV(size_t index, double value);
  void setP(size_t index, double value);

  void setU(const std::vector<double> &u);
  void setV(const std::vector<double> &v);
  void setP(const std::vector<double> &p);

  void setStormParameters(const StormParameters &sp);
  const StormParameters *stormParameters() const;

  void setSize(size_t n);

 private:
  size_t m_n;
  std::vector<double> m_u;
  std::vector<double> m_v;
  std::vector<double> m_p;
  StormParameters m_stormParameters;
};

#endif  // METGET_LIBRARY_WINDDATA_H_
