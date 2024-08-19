//
// Created by Zach Cobell on 7/30/24.
//

#include "AtcfPeriod.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>

#include "datatypes/QuadCode.h"
#include "output/RadialProfile.h"
#include "physical/Constants.h"
#include "storm/Quadrant.h"

namespace Gahm::Atcf {

/**
 * Compute the GAHM parameters for each quadrant
 */
void AtcfPeriod::compute_gahm_parameters() {
  std::ranges::for_each(m_quadrants, [this](Storm::Quadrant &quad) {
    quad.compute_gahm_parameters(m_translation, m_eye_location,
                                 m_central_pressure, m_background_pressure,
                                 m_v_max);
  });
}

/**
 * @brief Generate a gnuplot file for the period and write to the given filename
 *
 */
void AtcfPeriod::to_gnuplot(const std::string &filename) const {
  std::ofstream file(filename);

  // Write the gnuplot header
  file << "set terminal pngcairo\n";
  file << "set output '" << filename << ".png'\n";
  file << "set size ratio -1\n";
  file << "set terminal png size 1800,1800\n";
  file << "set multiplot title 'GAHM Parameters for " << m_datetime << "'\n";

  // Write the first plot for the isotach radii
  file << "# --- GRAPH ISOTACH RADII\n";
  file << "set origin 0.16,0.75\n";
  file << "set size 0.25,0.25\n";
  file << "set xrange [" << eye_location().x() - 5 << ":"
       << eye_location().x() + 5 << "]\n";
  file << "set yrange [" << eye_location().y() - 5 << ":"
       << eye_location().y() + 5 << "]\n";
  file << "set xlabel 'Longitude'\n";
  file << "set ylabel 'Latitude'\n";
  file << "set title 'Isotach Radii'\n";
  file << "set key off\n";
  file << "set grid\n";

  const std::array<std::string, 3> isotach_colors = {"'red'", "'green'",
                                                     "'blue'"};

  // Add annotations for the isotach speeds in a pseudo-legend
  file << "set object 1 rect from screen 0.43,0.94 to screen 0.45,0.943 fc rgb "
          "'red' lw 0\n";
  file << "set label 1 '34' at screen 0.46,0.94\n";
  file << "set object 2 rect from screen 0.43,0.92 to screen 0.45,0.923 fc rgb "
          "'green'\n";
  file << "set label 2 '50' at screen 0.46,0.92\n";
  file << "set object 3 rect from screen 0.43,0.90 to screen 0.45,0.903 fc rgb "
          "'blue'\n";
  file << "set label 3 '64' at screen 0.46,0.90\n";

  // Draw each isotach as a quarter circle with a diameter corresponding to the
  // isotach radius. The center of the circle is the eye location. Doing this
  // for each quadrant will give us the full circle, though it's ok that it is
  // disconnected.
  size_t obj_counter = 4;
  for (const auto &quad : m_quadrants) {
    size_t isotach_counter = 0;
    for (const auto &isotach : quad.isotachs()) {
      // Calculate the start and end angles for the circle
      const auto start_angle = quad.quadrant_code() * 90;
      const auto end_angle = (quad.quadrant_code() + 1) * 90;
      file << "set object " << std::to_string(obj_counter) << " circle at "
           << eye_location().x() << "," << eye_location().y() << " size "
           << 2 * isotach.radius() / 111111.0 << " arc ["
           << std::to_string(start_angle) << ":" << std::to_string(end_angle)
           << "] lw 2 fc " << isotach_colors[isotach_counter] << " nowedge\n";
      obj_counter++;

      ++isotach_counter;
    }
  }
  file << "plot NaN notitle\n";
  file << "unset object\n";

  // Now we do the same thing, but for radius to max winds
  file << "# --- GRAPH RMAX\n";
  file << "set origin 0.57,0.75\n";
  file << "set size 0.25,0.25\n";
  file << "set xrange [" << eye_location().x() - 5 << ":"
       << eye_location().x() + 5 << "]\n";
  file << "set yrange [" << eye_location().y() - 5 << ":"
       << eye_location().y() + 5 << "]\n";
  file << "set xlabel 'Longitude'\n";
  file << "set ylabel 'Latitude'\n";
  file << "set title 'Radius to Max Winds'\n";
  file << "set key off\n";
  file << "set grid\n";

  for (const auto &quad : m_quadrants) {
    size_t isotach_counter = 0;
    for (const auto &isotach : quad.isotachs()) {
      // Calculate the start and end angles for the circle
      const auto start_angle = quad.quadrant_code() * 90;
      const auto end_angle = (quad.quadrant_code() + 1) * 90;
      file << "set object circle at " << eye_location().x() << ","
           << eye_location().y() << " size "
           << 2 * isotach.radius_to_max_winds() / 111111.0 << " arc ["
           << std::to_string(start_angle) << ":" << std::to_string(end_angle)
           << "] lw 2 fc " << isotach_colors[isotach_counter] << " nowedge\n";
      file << "\n";
      ++isotach_counter;
    }
  }
  file << "plot NaN notitle\n";

  // Reset the plot space
  file << "unset object\n";

  std::for_each(m_quadrants.begin(), m_quadrants.end(), [&](const auto &quad) {
    // In the lower 50% of the plot, make 4 plots (one for each quadrant)
    auto plot_origin_x = 0.0;
    auto plot_origin_y = 0.0;
    if (quad.quadrant_code() == Types::QuadCode::QuadrantCode::NE) {
      plot_origin_x = 0.0;
      plot_origin_y = 0.0;
    } else if (quad.quadrant_code() == Types::QuadCode::QuadrantCode::NW) {
      plot_origin_x = 0.0;
      plot_origin_y = 0.375;
    } else if (quad.quadrant_code() == Types::QuadCode::QuadrantCode::SE) {
      plot_origin_x = 0.5;
      plot_origin_y = 0.0;
    } else if (quad.quadrant_code() == Types::QuadCode::QuadrantCode::SW) {
      plot_origin_x = 0.5;
      plot_origin_y = 0.375;
    }

    file << "# --- GRAPH c\n";
    file << "set origin " << plot_origin_x << "," << plot_origin_y << "\n";
    file << "set size 0.5,0.375\n";
    file << "set xrange [0:500]\n";
    file << "set yrange [0:60]\n";
    file << "set size ratio 1\n";
    file << "set xlabel 'Distance (km)'\n";
    file << "set ylabel 'Wind Speed (m/s)'\n";
    file << "set title 'Radial Profile for Quadrant "
         << Types::QuadCode::quadrant_code_to_string(quad.quadrant_code())
         << "'\n";
    file << "set key off\n";
    file << "set grid\n";

    size_t isotach_id = 0;
    std::for_each(quad.isotachs().begin(), quad.isotachs().end(),
                  [&](const auto &isotach) {
                    // Plot a vertical line at the isotach radius between 0 and
                    // the isotach speed
                    file << "set arrow " << std::to_string(isotach_id + 1)
                         << " from " << isotach.radius() / 1000.0 << ",0 to "
                         << isotach.radius() / 1000.0 << ","
                         << isotach.wind_speed() *
                                Physical::Constants::oneMinuteToTenMinuteWind()
                         << " nohead lc " << isotach_colors[isotach_id]
                         << " lw 2\n";
                    isotach_id++;
                  });

    const auto profile =
        Output::RadialProfile::get_profile(*this, quad, 0, 500000, 1000);
    auto data_block_name = "$data_" + std::to_string(quad.quadrant_code()) +
                           "_" + std::to_string(1);
    file << data_block_name << "<< EOD\n";

    std::ranges::for_each(profile.data, [&](const auto &snap) {
      file << snap.distance / 1000.0 << " " << snap.wind_vector.magnitude()
           << "\n";
    });
    file << "EOD\n";
    file << "plot " << data_block_name << " with lines lc rgb 'black' lw 3\n";

    file << "unset object\n";
    file << "unset arrow\n";
  });

  file << "unset multiplot\n";
  file.close();
}

}  // namespace Gahm::Atcf

auto operator<<(std::ostream &stream,
                const Gahm::Atcf::AtcfPeriod &period) -> std::ostream & {
  stream << "Date: " << period.datetime()
         << ", Lat: " << period.eye_location().x()
         << ", Lon: " << period.eye_location().y()
         << ", Max Wind: " << period.v_max()
         << ", Min Pressure: " << period.central_pressure()
         << ", Max Radius: " << period.r_max() << "\n"
         << "  Quadrants: \n";

  for (const auto &quad : period.quadrants()) {
    stream
        << "    Quadrant "
        << Gahm::Types::QuadCode::quadrant_code_to_string(quad.quadrant_code())
        << " ("
        << Gahm::Types::QuadCode::quadrant_code_to_integer(quad.quadrant_code())
        << "): \n";
    stream << quad;
  }
  stream << period.translation();
  return stream;
}