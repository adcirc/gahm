// GAHM SWIG Module

%module pygahm

%insert("python")
%{
  import signal
  signal.signal(signal.SIGINT, signal.SIG_DFL)
%}


%{
#define SWIG_FILE_WITH_INIT

#include "physical/Constants.h"
#include "physical/Earth.h"
#include "physical/Atmospheric.h"

#include "datatypes/Date.h"
#include "datatypes/Point.h"
#include "datatypes/PointCloud.h"
#include "datatypes/Uvp.h"
#include "datatypes/VortexSolution.h"
#include "datatypes/WindGrid.h"

#include "atcf/AtcfFile.h"
#include "atcf/AtcfSnap.h"
#include "atcf/AtcfIsotach.h"
#include "atcf/AtcfQuadrant.h"
#include "atcf/StormPosition.h"
#include "atcf/StormTranslation.h"

#include "output/OutputFile.h"
#include "output/OwiOutput.h"

#include "preprocessor/Preprocessor.h"

#include "vortex/Vortex.h"
%}

%include <std_string.i>
%include <exception.i>
%include <std_vector.i>
%include <windows.i>

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  } catch (const std::string& e) {
    SWIG_exception(SWIG_RuntimeError, e.c_str());
  }
}

namespace std {
    %template(IntVector) vector<int>;
    %template(SizetVector) vector<size_t>;
    %template(DoubleVector) vector<double>;
    %template(DoubleDoubleVector) vector<vector<double>>;
    %template(SizetSizetVector) vector<vector<size_t>>;
}


%include "physical/Constants.h"
%include "physical/Earth.h"
%include "physical/Atmospheric.h"

%include "datatypes/Date.h"
%include "datatypes/Point.h"
%include "datatypes/PointCloud.h"
%include "datatypes/Uvp.h"
%include "datatypes/VortexSolution.h"
%include "datatypes/WindGrid.h"

%include "atcf/AtcfFile.h"
%include "atcf/AtcfSnap.h"
%include "atcf/AtcfIsotach.h"
%include "atcf/AtcfQuadrant.h"
%include "atcf/StormPosition.h"
%include "atcf/StormTranslation.h"

%include "output/OutputFile.h"
%include "output/OwiOutput.h"

%include "preprocessor/Preprocessor.h"

%include "vortex/Vortex.h"
