
/* Gahm Interface File */
%module pygahm

%insert("python") %{
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
%}


%{
#define SWIG_FILE_WITH_INIT
#include "Gahm.h"
%}

%include <std_string.i>
%include <exception.i>
%include <std_vector.i>
%include <std_array.i>
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
    %template(DoubleVector) vector<double>; 
    %template(DoubleDoubleVector) vector<vector<double>>;
}

%include "Assumption.h"
%include "Assumptions.h"
%include "Atcf.h"
%include "AtcfLine.h"
%include "Constants.h"
%include "Date.h"
%include "GahmState.h"
%include "GahmVortex.h"
%include "HurricanePressure.h"
%include "Isotach.h"
%include "Logging.h"
%include "OwiAscii.h"
%include "OwiAsciiDomain.h"
%include "Point.h"
%include "Preprocessor.h"
%include "StormParameters.h"
%include "Vortex.h"
%include "WindData.h"
%include "WindGrid.h"
%include "ParameterPack.h"
