# Generalized Asymmetric Holland Model (GAHM)

A standalone version of the Generalized Asymmetric Holland Model, GAHM, which is used as NWS=20 in ADCIRC

## Repository Metrics
[![CircleCI](https://circleci.com/gh/adcirc/gahm.svg?style=shield&circle-token=116e719e84c21191a8b39903273590bdbf016d48)](https://circleci.com/gh/adcirc/adcirc-cg)

## Under Construction

Note that this library is under construction and is not currently in a working state

## Building GAHM

GAHM is built using the CMake build system, C++14, Boost, and SWIG. GAHM can be built as either static or shared
library. The library contains interfaces for C++, Fortran, and Python. The SWIG library is used to generate a Python
interface to GAHM as an option.

## Documentation
Documentation is provided via Doxygen and Github Pages [here](https://adcirc.github.io/gahm).

### Sample build command

```bash
mkdir build
cd build
cmake .. -DCMAKE_CXX_COMPILER=g++
make -j4
make test
```

