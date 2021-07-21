
lib_LTLIBRARIES = libgahm.la

libgahm_la_SOURCES =  \
  Gahm.cpp \
  Atcf.cpp \
  AtcfLine.cpp \
  Isotach.cpp \
  Date.cpp \
  HurricanePressure.cpp \
  Logging.cpp \
  Vortex.cpp \
  Assumptions.cpp \
  Assumption.cpp \
  Preprocessor.cpp \
  VortexSolver.cpp \
  GahmState.cpp \
  OwiAscii.cpp \
  OwiAsciiDomain.cpp \
  WindData.cpp \
  WindGrid.cpp \
  Point.cpp

include_HEADERS = Gahm.h GahmState.h StormParameters.h WindData.h WindGrid.h

AM_CPPFLAGS = -I${top_srcdir}/thirdparty/date_hh -I${top_srcdir}/thirdparty/boost_1_76_0
