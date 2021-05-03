# Generalized Asymmetric Holland Model (GAHM)

A standalone version of the Generalized Asymmetric Holland Model, GAHM, which is used as NWS=20 in ADCIRC

## Repository Metrics

## Under Construction

Note that this library is under construction and is not currently in a working state

## Building GAHM

GAHM is built using the CMake build system, C++14, Boost, and SWIG. GAHM can be built as either static or shared
library. The library contains interfaces for C++, Fortran, and Python. The SWIG library is used to generate a Python
interface to GAHM as an option.

### Sample build command

```bash
mkdir build
cd build
cmake .. -DCMAKE_CXX_COMPILER=g++
make -j4
make test
```

