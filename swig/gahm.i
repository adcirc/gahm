
/* Gahm Interface File */
%module pygahm

%insert("python") %{
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
%}

%{
#define SWIG_FILE_WITH_INIT
#include "Gahm.h"
#include "OwiAscii.h"
#include "WindData.h"
#include "WindGrid.h"
#include "Date.h"
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

%include "Gahm.h"
%include "OwiAscii.h"
%include "WindData.h"
%include "WindGrid.h"
%include "Date.h"
