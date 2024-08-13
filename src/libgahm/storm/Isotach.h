//
// Created by Zach Cobell on 7/30/24.
//

#ifndef GAHM_ISOTACH_H
#define GAHM_ISOTACH_H

#include <cassert>
#include <ostream>

namespace Gahm::Storm {

class Isotach {
 public:
  constexpr Isotach() : m_wind_speed(0.0), m_distance_to_isotach(0.0) {}

  constexpr explicit Isotach(double wind_speed, double radius)
      : m_wind_speed(wind_speed), m_distance_to_isotach(radius) {}

  [[nodiscard]] constexpr auto wind_speed() const -> double {
    return m_wind_speed;
  }

  [[nodiscard]] constexpr auto radius() const -> double {
    return m_distance_to_isotach;
  }

  [[nodiscard]] constexpr auto is_populated() const -> bool {
    return m_wind_speed != 0.0;
  }

  [[nodiscard]] constexpr auto operator==(const Isotach &rhs) const -> bool {
    return m_wind_speed == rhs.m_wind_speed;
  }

 private:
  //  static constexpr auto sanitize_radii(const std::array<double, 4> &radii)
  //      -> QuadrantParameter {
  //    return sanitize_radii(QuadrantParameter(radii));
  //  }
  //
  //  static constexpr auto sanitize_radii(const QuadrantParameter &radii)
  //      -> QuadrantParameter {
  //    const auto n_populated =
  //        std::count_if(radii.begin(), radii.end(),
  //                      [](double radius) { return radius != 0.0; });
  //    if (n_populated == 0) {
  //      return {0.0, 0.0, 0.0, 0.0};
  //    } else if (n_populated == 1) {
  //      return Isotach::fill_radius_from_single_radii(radii);
  //    } else if (n_populated == 2) {
  //      return Isotach::fill_radius_from_two_radii(radii);
  //    } else if (n_populated == 3) {
  //      return Isotach::fill_radius_from_three_radii(radii);
  //    } else {
  //      return radii;
  //    }
  //  }
  //
  //  static constexpr auto fill_radius_from_three_radii(
  //      const QuadrantParameter &radii) -> QuadrantParameter {
  //    const auto index_not_populated = std::distance(
  //        radii.begin(), std::find(radii.begin(), radii.end(), 0.0));
  //    const auto index_1 = static_cast<unsigned>((index_not_populated - 1) %
  //    4); const auto index_2 = static_cast<unsigned>((index_not_populated + 1)
  //    % 4); const auto mean_radius = (radii.at(index_1) + radii.at(index_2))
  //    / 2.0; QuadrantParameter filled_radii = radii;
  //    filled_radii.set(static_cast<unsigned>(index_not_populated),
  //    mean_radius); return filled_radii;
  //  }
  //
  //  static auto fill_radius_from_two_radii(const QuadrantParameter &radii)
  //      -> QuadrantParameter {
  //    const auto mean_radius =
  //        std::accumulate(radii.begin(), radii.end(), 0.0) / 2.0;
  //    QuadrantParameter filled_radii;
  //    std::replace_if(
  //        filled_radii.begin(), filled_radii.end(),
  //        [](double radius) { return radius == 0.0; }, mean_radius);
  //    return filled_radii;
  //  }
  //
  //  static constexpr auto fill_radius_from_single_radii(
  //      const QuadrantParameter &radii) noexcept -> QuadrantParameter {
  //    const auto populated_radius =
  //        std::reduce(radii.begin(), radii.end(), 0.0, std::plus{});
  //    return {populated_radius, populated_radius, populated_radius,
  //            populated_radius};
  //  }
  double m_wind_speed;
  double m_distance_to_isotach;
};

}  // namespace Gahm::Storm

auto operator<<(std::ostream &stream,
                const Gahm::Storm::Isotach &isotach) -> std::ostream &;

#endif  // GAHM_ISOTACH_H
