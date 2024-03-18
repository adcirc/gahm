
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
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/c4da3e820daa42cf93c8ff383f8eb124)](https://app.codacy.com/gh/adcirc/gahm/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

## Example Usage
Below is an example of the GAHM model output for Hurricane Katrina. This image was generated
using the Python interface to the GAHM model and the example script in `examples/ex1_katrina.py` using the following
command:

```commandline
./ex1_katrina.py --type wind ../tests/test_files/bal122005.dat \
                 --time 2005-08-29 --output katrina.png
```

![](https://raw.githubusercontent.com/adcirc/gahm/main/doc/img/katrina.png)

In addition to a Python interface, there is also a Fortran interface. The Fortran interface
is intended for use by the ADCIRC model, so its scope may be somewhat limited. This may change based
on feedback. As a general rule, the Python interface is preferred for general tinkering and the
C++ interface is preferred for performance. The Fortran interface is available for narrow use cases.

A quick comparison of the interfaces is shown below. The Python interface and C++ interface are
nearly identical because they are generated via SWIG. The Fortran interface is wrapped by hand
and care needs to be taken with ensuring safe memory management when moving between languages. 

### C++ Interface
```cpp
#include <iostream>

#include "atcf/AtcfFile.h"
#include "datatypes/WindGrid.h"
#include "preprocessor/Preprocessor.h"
#include "vortex/Vortex.h"

int main(int argc, char** argv) {

  //...Generate a wind grid. A wind grid is useful for generating a 
  // domain for interpolation, however, any arrangement of points can
  // be used. The user only needs to generate a PointCloud object and 
  // pass that to the Vortex object.
  Gahm::Datatypes::WindGrid wg = Gahm::Datatypes::WindGrid::fromCorners(
      -100.0, 22.0, -78.0, 32.0, 0.1, 0.1);

  //...Read the ATCF file. Note that this is a RAW atcf file, not an ADCIRC
  // formatted NWS=XX file
  const std::string filename = "test_files/bal122005.dat";
  auto atcf = Gahm::Atcf::AtcfFile(filename);
  atcf.read();

  //...Run the preprocessor. This is the equivalent of the ADCIRCs ASWIP program
  // This solves for the GAHM model parameters for use by the Vortex object
  Gahm::Preprocessor prep(&atcf);
  prep.solve();

  //...Create the Vortex object using the PointCloud object and the AtcfFile object
  auto v = Gahm::Vortex(&atcf, wg.points());
  
  //...Solve the vortex at the given time
  auto time = Gahm::Datatypes::Date(2005, 8, 29, 0, 0, 0);
  auto solution = v.solve(time);
  
  //...Write the solution to the screen
  std::cout << solution.uvp()[42].u() << ", " 
            << solution.uvp()[42].v() << ", " 
            << solution.uvp()[42].p() << std::endl;

}
```

### Python Interface
```python
import pygahm
import numpy as np

filename = "test_files/bal122005.dat"

# Generate the wind grid
wind_grid = pygahm.WindGrid.fromCorners(-100.0, 22.0, -60.0, 40.0, 0.1, 0.1)

# Read the ATCF file
atcf = pygahm.AtcfFile(filename)
atcf.read()

# Run the preprocessor
prep = pygahm.Preprocessor(atcf)
prep.solve()

# Create the Vortex object
v = pygahm.Vortex(atcf, wind_grid.points())

# Solve the vortex at the given time
time = pygahm.Date(2005, 8, 29, 0, 0, 0) #...NOT a python datetime object

# Solve the vortex
solution = v.solve(time)

# Print the solution
u = np.array(solution.u())
v = np.array(solution.v())
p = np.array(solution.p())
print(u[42], v[42], p[42])
```

### Fortran Interface
```Fortran
subroutine gahm_test()
    use gahm_module
    implicit none
    
    type(gahm_t)   :: gahm
    type(date_t)   :: current_date
    character(200) :: filename
    integer(8)     :: n_pts
    real(8)        :: x(1), y(1)
    real(8)        :: u(1), v(1), p(1)
    
    x(1) = -90.0
    y(1) = 29.0
    n_pts = size(x)
    
    filename = "tests/test_files/bal122005.dat"
    
    call gahm%initialize(filename, n_pts, x, y)
    call current_date%set(2005,8,27)
    call gahm%get(current_date, n_pts, u, v, p)

end subroutine gahm_test
```
Note that the Fortran interface will manage the memory for the GAHM objects automatically using C++ `std::unique_ptr` types. 
Still, though, depending on the compiler, the user should place `gahm_t` objects within subroutines to ensure that 
the destructor is called explicitly as Fortran's behavior isn't as well defined as other OOP languages.

## Installation 
The project is built using CMake. 

The following dependencies are required to build the project:
- CMake (3.18 or greater)
- C++ compiler (with C++17 compatibility, i.e. gcc 7.0 or greater)

The following are optional dependencies:
- Python 3
- SWIG 4.0+ (Python interface)
- Fortran compiler (Fortran interface)
- Clang/LLVM (Fuzz testing)

### CMake
In its most simple form, the project can be built on the command line using:

```commandline
mkdir build
cd build
cmake ..
make
```

In the case that you'd like to see more sophisticated examples, the .github/workflows directory contains
a number of examples of how to build the project for various use cases.
