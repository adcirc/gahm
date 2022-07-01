//
// Created by Zach Cobell on 5/10/22.
//

#include "Atcf.h"

#include <fstream>
#include <utility>

#include "Logging.h"
#include "StringUtilities.h"

using namespace Gahm;

Atcf::Atcf() : m_format(Atcf::FORMAT::NONE) {}

Atcf::Atcf(std::string filename, Atcf::FORMAT format)
    : m_filename(std::move(filename)), m_format(format) {}

void Atcf::addForecastPeriod(const ForecastPeriod& f) {
  const auto it =
      std::lower_bound(m_forecastData.begin(), m_forecastData.end(), f);
  if (it == m_forecastData.end()) {
    m_forecastData.push_back(f);
  } else if (f.date() == it->date()) {
    Logging::throwError(
        "This date already exists in the data structure. "
        "Please remove or add as an additional isotach.");
  } else {
    m_forecastData.insert(it, f);
  }
}

std::vector<Gahm::ForecastPeriod>& Atcf::data() { return m_forecastData; }

const std::vector<Gahm::ForecastPeriod>& Atcf::data() const {
  return m_forecastData;
}

void Atcf::read() {
  //...Open the input file
  std::ifstream input_file(m_filename);

  //...Loop over the input file lines
  for (std::string line; std::getline(input_file, line);) {
    //...Split the ATCF lines by comma
    const auto tokens = StringUtilities::splitString(line);

    //...Check and make sure the data length is appropriate
    if (tokens.size() < 27) continue;

    //...Start by extracting the date. We'll use this to check if this is a new
    // forecast period, or just an additional isotach
    const auto date = Atcf::getDateFromAtcfLine(tokens);

    //...No matter what, we need the isotach data, so grab that now and build a
    // StormIsotach object
    const auto iso = Atcf::generateStormIsotachFromAtcfLine(tokens);

    //...If we don't have the current time in the forecast object already, we
    // will create a new ForecastPeriod object with the full set of data
    if (!this->containsTime(date)) {
      auto period = Atcf::generateForecastPeriodFromAtcfLine(tokens, date, iso);
      this->addForecastPeriod(period);
    } else {
      //...Since we don't need all the data, we're just going to add the isotach
      // to the appropriate ForecastPeriod object
      auto period = this->findTime(date);
      period->addIsotach(iso);
    }
  }
  this->computeTranslationVelocities();
  input_file.close();
}

Date Atcf::getDateFromAtcfLine(const std::vector<std::string>& tokens) {
  auto date = Date::fromString(boost::trim_copy(tokens[2]), "%Y%m%d%H");
  const auto tau = StringUtilities::readValueCheckBlank<int>(tokens[5]);
  date.addHours(tau);
  return date;
}

StormIsotach Atcf::generateStormIsotachFromAtcfLine(
    const std::vector<std::string>& tokens) {
  constexpr double kt2ms = Units::convert(Units::Knot, Units::MetersPerSecond);
  constexpr double nmi2m = Units::convert(Units::NauticalMile, Units::Meter);

  auto isotach_wind_speed =
      StringUtilities::readValueCheckBlank<double>(tokens[11]) * kt2ms;
  if (isotach_wind_speed == 0.0) {
    // If no isotach speed is given, we set the isotach speed to vmax and the
    // radii to rmax
    const auto v_max =
        StringUtilities::readValueCheckBlank<double>(tokens[8]) * kt2ms;
    const auto r_max =
        StringUtilities::readValueCheckBlank<double>(tokens[19]) * nmi2m;
    return {v_max, {r_max, r_max, r_max, r_max}};
  } else {
    const std::array<double, 4> radii = {
        StringUtilities::readValueCheckBlank<double>(tokens[13]) * nmi2m,
        StringUtilities::readValueCheckBlank<double>(tokens[14]) * nmi2m,
        StringUtilities::readValueCheckBlank<double>(tokens[15]) * nmi2m,
        StringUtilities::readValueCheckBlank<double>(tokens[16]) * nmi2m};
    return {isotach_wind_speed, radii};
  }
}

ForecastPeriod Atcf::generateForecastPeriodFromAtcfLine(
    const std::vector<std::string>& tokens, const Date& date,
    const StormIsotach& iso) {
  constexpr double kt2ms = Units::convert(Units::Knot, Units::MetersPerSecond);
  constexpr double nmi2m = Units::convert(Units::NauticalMile, Units::Meter);

  const auto basin =
      ForecastPeriod::stringToBasinId(boost::trim_copy(tokens[0]));
  const auto storm_id = StringUtilities::readValueCheckBlank<int>(tokens[1]);

  auto lat = stod(tokens[6].substr(0, tokens[6].size() - 1)) / 10.0;
  if (*(tokens[6].rbegin()) == 'S') lat *= -1.0;

  auto lon = stod(tokens[7].substr(0, tokens[7].size() - 1)) / 10.0;
  if (*(tokens[7].rbegin()) == 'W') lon *= -1.0;

  const auto v_max =
      StringUtilities::readValueCheckBlank<double>(tokens[8]) * kt2ms;
  const auto p_min = StringUtilities::readValueCheckBlank<double>(tokens[9]);

  const auto r_max =
      StringUtilities::readValueCheckBlank<double>(tokens[19]) * nmi2m;
  const auto storm_name = tokens[27];

  ForecastPeriod period(basin, date, lon, lat, 0, 0, r_max, v_max, p_min,
                        Physical::backgroundPressure(), storm_id, storm_name);
  period.addIsotach(iso);
  return period;
}

bool Atcf::containsTime(const Date& d) {
  return std::any_of(m_forecastData.begin(), m_forecastData.end(),
                     ForecastPeriodTimeComparison(d));
}

Gahm::ForecastPeriod* Atcf::findTime(const Date& d) {
  if (m_forecastData.empty()) {
    return nullptr;
  } else {
    const auto pos = std::find_if(m_forecastData.begin(), m_forecastData.end(),
                                  ForecastPeriodTimeComparison(d));
    if (pos == m_forecastData.end()) {
      return nullptr;
    } else {
      return &(*pos);
    }
  }
}

std::vector<Gahm::ForecastPeriod>::const_iterator Atcf::selectForecastPeriod(
    const Date& d) const {
  return std::lower_bound(m_forecastData.begin(), m_forecastData.end(), d);
}
std::string Atcf::filename() const { return this->m_filename; }

void Atcf::computeTranslationVelocities() {
  for (auto it = (m_forecastData.begin() + 1); it != m_forecastData.end();
       ++it) {
    auto d1 = (it - 1)->stormPosition();
    auto d2 = (it)->stormPosition();
    const auto [u_dis, v_dis, uv_dis] = Physical::sphericalDx(
        d1.longitude(), d1.latitude(), d2.longitude(), d2.latitude());
    const auto dt = static_cast<double>(it->date().toSeconds() -
                                        (it - 1)->date().toSeconds());
    auto uu = std::abs(u_dis / dt);
    if (d2.longitude() - d1.longitude() <= 0.0) uu *= -1.0;

    auto vv = std::abs(v_dis / dt);
    if (d2.latitude() - d1.latitude() <= 0.0) vv *= -1.0;

    auto uv = uv_dis / dt;
    auto dir = std::atan2(vv, uu);
    if (dir < 0) dir += Constants::twopi();

    it->stormPosition().setSpeedAndDirection(uv, dir);
  }
  m_forecastData.front().stormPosition().setSpeedAndDirection(
      m_forecastData[1].stormPosition().forwardSpeed(),
      m_forecastData[1].stormPosition().transitDirection());
}

void Atcf::write(const std::string& filename, Gahm::Atcf::FORMAT format) const {
  switch (format) {
    case ASWIP:
      return this->writeAswip(filename);
    default:
      Logging::throwError("Invalid output format selected");
      return;
  }
}

void Atcf::writeAswip(const std::string& filename) const {

}
