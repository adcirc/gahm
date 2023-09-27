
# Generalized Asymmetric Holland Model (GAHM)

The GAHM model uses hurricane parameters in ATCF format to construct a hurricane vortex
for use in storm surge modeling or other analysis. 

The project reimplements the GAHM model originally developed by Gao et al. and included in
the ADCIRC model. The reimplementation allows users to run the GAHM model without ADCIRC 
and through a C++, Fortran, or Python interface. 

## Project Status
The project is under active development but is not considered production ready. 

[![Testing](https://github.com/adcirc/gahm/actions/workflows/testing.yaml/badge.svg)](https://github.com/adcirc/gahm/actions/workflows/testing.yaml)
[![codecov](https://codecov.io/gh/adcirc/gahm/graph/badge.svg?token=4hphc6xaqj)](https://codecov.io/gh/adcirc/gahm)

## Installation 
The project is built using CMake. 

The following dependencies are required to build the project:
- CMake
- C++ compiler

The following are optional dependencies:
- Python 3
- Fortran compiler
- Clang/LLVM (Fuzz testing)

### CMake
The project can be build on the command line using:

```
mkdir build
cd build
cmake ..
make
```
