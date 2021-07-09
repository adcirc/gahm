//
// Created by Zach Cobell on 1/23/21.
//
#include "OwiAscii.h"

#include <cassert>
OwiAscii::OwiAscii(const Date& startDate, const Date& endDate,
                   const unsigned time_step)
    : m_startdate(startDate), m_enddate(endDate), m_timestep(time_step) {}

int OwiAscii::addDomain(const WindGrid& w, const std::string& pressureFile,
                        const std::string& windFile) {
  m_domains.push_back(std::make_unique<OwiAsciiDomain>(
      &w, m_startdate, m_enddate, m_timestep, pressureFile, windFile));
  return 0;
}

int OwiAscii::write(const Date& date, const size_t domain_index,
                    const std::vector<double>& pressure,
                    const std::vector<double>& wind_u,
                    const std::vector<double>& wind_v) {
  assert(domain_index < m_domains.size());
  return m_domains[domain_index]->write(date, pressure, wind_u, wind_v);
}

int OwiAscii::write(const Date& date, const size_t domain_index,
                    const WindData& data) {
  return this->write(date, domain_index, data.p(), data.u(), data.v());
}
