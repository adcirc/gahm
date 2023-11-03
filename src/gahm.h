//
// Created by Zach Cobell on 10/31/23.
//

#ifndef GAHM_H
#define GAHM_H

#include <string>

#include "atcf/AtcfFile.h"
#include "atcf/AtcfIsotach.h"
#include "atcf/AtcfQuadrant.h"
#include "atcf/AtcfSnap.h"
#include "atcf/StormPosition.h"
#include "atcf/StormTranslation.h"
#include "datatypes/Date.h"
#include "datatypes/Point.h"
#include "datatypes/PointCloud.h"
#include "datatypes/PointPosition.h"
#include "datatypes/Uvp.h"
#include "datatypes/VortexSolution.h"
#include "datatypes/WindGrid.h"
#include "gahm/GahmEquations.h"
#include "gahm/GahmSolver.h"
#include "output/OutputFile.h"
#include "output/OwiOutput.h"
#include "physical/Atmospheric.h"
#include "physical/Constants.h"
#include "physical/Earth.h"
#include "physical/Units.h"
#include "preprocessor/Preprocessor.h"
#include "vortex/Vortex.h"

namespace Gahm {

std::string library_version() { return "0.0.1"; }

bool is_debug() {
#ifdef GAHM_DEBUG
  return true;
#else
  return false;
#endif
}

}  // namespace Gahm

#endif  // GAHM_H
