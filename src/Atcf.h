//
// Created by Zach Cobell on 5/10/22.
//

#ifndef GAHM2_SRC_ATCF_H_
#define GAHM2_SRC_ATCF_H_

#include <string>
#include <vector>

#include "ForecastPeriod.h"

namespace Gahm {
class Atcf {
 public:
  enum FORMAT { NONE, NHC, ASWIP };

  Atcf();

  explicit Atcf(std::string filename,
                Gahm::Atcf::FORMAT format = Gahm::Atcf::FORMAT::NHC);
  void read();

  void write(const std::string &filename,
             Gahm::Atcf::FORMAT format = Gahm::Atcf::ASWIP) const;

  [[nodiscard]] std::string filename() const;

  void addForecastPeriod(const Gahm::ForecastPeriod &f);

  [[nodiscard]] bool containsTime(const Date &d);

  [[nodiscard]] const std::vector<Gahm::ForecastPeriod> &data() const;

  [[nodiscard]] std::vector<Gahm::ForecastPeriod> &data();

  [[nodiscard]] Gahm::ForecastPeriod *findTime(const Date &d);

  [[nodiscard]] std::vector<Gahm::ForecastPeriod>::const_iterator
  selectForecastPeriod(const Date &d) const;

  [[nodiscard]] size_t size() const { return m_forecastData.size(); }

 private:
  [[nodiscard]] static ForecastPeriod generateForecastPeriodFromAtcfLine(
      const std::vector<std::string> &tokens, const Date &date,
      const StormIsotach &iso);

  [[nodiscard]] static StormIsotach generateStormIsotachFromAtcfLine(
      const std::vector<std::string> &tokens);

  [[nodiscard]] static Date getDateFromAtcfLine(
      const std::vector<std::string> &tokens);

  void computeTranslationVelocities();

  void writeAswip(const std::string &filename) const;

  struct ForecastPeriodTimeComparison {
    explicit ForecastPeriodTimeComparison(const Date &d) : m_d(d) {}
    [[nodiscard]] bool operator()(const ForecastPeriod &a) const {
      return m_d == a.date();
    }
    Date m_d;
  };

  Gahm::Atcf::FORMAT m_format;
  std::string m_filename;
  std::vector<Gahm::ForecastPeriod> m_forecastData;
};
}  // namespace Gahm
#endif  // GAHM2_SRC_ATCF_H_
