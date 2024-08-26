%module pygahm

%insert("python")
%{
  import signal
  signal.signal(signal.SIGINT, signal.SIG_DFL)
%}

%{
#define SWIG_FILE_WITH_INIT

#include "datatypes/Datetime.hpp"
#include "datatypes/Grid.hpp"
#include "datatypes/Point.hpp"
#include "datatypes/PointCloud.hpp"
#include "datatypes/QuadCode.hpp"
#include "datatypes/QuadUnitVec.hpp"
#include "datatypes/Vec.hpp"
#include "atcf/AtcfIO.hpp"
#include "atcf/AtcfPeriod.hpp"
#include "atcf/AtcfTrack.hpp"
#include "output/RadialProfile.hpp"
#include "output/PointOutput.hpp"
#include "storm/Isotach.hpp"
#include "storm/Quadrant.hpp"
#include "storm/StormTranslation.hpp"

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
  %template(PeriodVector) vector<Gahm::Atcf::AtcfPeriod>;
  %template(DoubleVector) vector<double>;
  %template(DoubleDoubleVector) vector<vector<double>>;
  %template(PointVector) vector<Gahm::Types::Point>;
  %template(SolutionPointVector) vector<Gahm::Output::PointOutput::SolutionPoint>;
}

%include "datatypes/Datetime.hpp"
%include "datatypes/Grid.hpp"
%include "datatypes/Point.hpp"
%include "datatypes/PointCloud.hpp"
%include "datatypes/QuadCode.hpp"
%include "datatypes/QuadUnitVec.hpp"
%include "datatypes/Vec.hpp"
%include "atcf/AtcfIO.hpp"
%include "atcf/AtcfPeriod.hpp"
%include "atcf/AtcfTrack.hpp"
%include "output/RadialProfile.hpp"
%include "output/PointOutput.hpp"
%include "storm/Isotach.hpp"
%include "storm/Quadrant.hpp"
%include "storm/StormTranslation.hpp"