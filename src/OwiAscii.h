//
// Created by Zach Cobell on 1/23/21.
//

#ifndef METGET_LIBRARY_OWIASCII_H_
#define METGET_LIBRARY_OWIASCII_H_

#include <string>

#include "OwiAsciiDomain.h"
#include "WindData.h"
#include "WindGrid.h"

class OwiAscii {
 public:
  OwiAscii(const Date &date_start, const Date &date_end, unsigned time_step);

  int addDomain(const WindGrid &w, const std::string &pressureFile,
                const std::string &windFile);

  int write(const Date &date, const size_t domain_index, const WindData &data);

  int write(const Date &date, const size_t domain_index,
            const std::vector<double> &pressure,
            const std::vector<double> &wind_u,
            const std::vector<double> &wind_v);

 private:
  const Date m_startdate;
  const Date m_enddate;
  const unsigned m_timestep;
  std::vector<std::unique_ptr<OwiAsciiDomain>> m_domains;
};

#endif  // METGET_LIBRARY_OWIASCII_H_
