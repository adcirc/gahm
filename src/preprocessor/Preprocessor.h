// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef GAHM_SRC_PREPROCESSOR_PREPROCESSOR_H_
#define GAHM_SRC_PREPROCESSOR_PREPROCESSOR_H_

#include "atcf/AtcfFile.h"
#include "atcf/AtcfQuadrant.h"
#include "atcf/StormTranslation.h"

namespace Gahm {
class Preprocessor {
 public:
  explicit Preprocessor(Gahm::Atcf::AtcfFile *atcf,
                        bool do_initialization = true);

  void prepareAtcfData();
  void solve();

 private:
  void orderIsotachs();
  void processIsotachRadii();
  void fillMissingAtcfData();
  void computeStormTranslationVelocities();
  void computeBoundaryLayerWindspeed();
  static Gahm::Atcf::StormTranslation getTranslation(
      const Gahm::Atcf::AtcfSnap &now, const Gahm::Atcf::AtcfSnap &next);

  static double removeTranslationVelocity(double wind_speed, double vmax_10m,
                                          int quadrant,
                                          const Atcf::StormTranslation &transit,
                                          double latitude);
  static double removeTranslationVelocity(
      double wind_speed, double vmax_10m,
      const Atcf::StormTranslation &transit);

  Gahm::Atcf::AtcfFile *m_atcf{nullptr};
  bool m_isotachsProcessed;
};
}  // namespace Gahm
#endif  // GAHM_SRC_PREPROCESSOR_PREPROCESSOR_H_
