# Himalaya

Himalaya can calculate corrections of the order O((alpha_b + alpha_t)*alpha_s^2 ) to the CP-even Higgs mass matrix in the DR-bar scheme.

## Requirements
The program requires:
* CMake >= 3.0

## Installation
CMake is used to generate build files.
To create these build files, you have to create a separate build directory.
Run:
```
mkdir build
cd build
cmake PATH_TO_HIMALAYA_DIR
```
By default the code is compiled optimized and with debugging symbols.

After calling `cmake` the build directory contains all required build files. Assuming that Makefiles are used, you can now run:
```
make
```

## Running the code
After the compilation the static library `libhimalaya.a` has been created which should be linked to your code.

### Example
We present a brief step by step guide how to run Himalaya and obtain the three-loop results. First you have to include the headers
```cpp
#include "HierarchyCalculator.hpp"
#include "Himalaya_interface.hpp"
#include "HierarchyObject.hpp"
```
to your file. In the next step you have to initialize all needed parameters in the `Parameters` `struct`:
```cpp
himalaya::Parameters point;
point.scale = <renormalization scale>;
...
```
Now you can create a `HierarchyCalculator` object with the given `struct`:
```
himalaya::HierarchyCalculator hc(point);
```
To calculate the results you just have to call:
```cpp
himalaya::HierarchyObject ho = hc.calculateDMh3L(false, 1); // the bool argument switches between corrections proportional to alpha_t (false) or alpha_b (true). The integer is a flag to choose among the DR- (0) or the MDR-scheme (1)
```
All information which has been gathered during the calculation will be stored in a `HierarchyObject` and can be accessed by member functions. To obtain the 3-loop correction to the Higgs mass matrix you have to call:
```cpp
ho.getDMh(3); // this returns a 2x2 matrix which contains the alpha_t*alpha_s^2 corrections for the given parameter point
```
The returned matrix should be added to your two-loop results **before** diagonalization.

A full and detailed example can be found in `source/example.cpp` with its executable `example`.

## Code Documentation
Doxygen can be used to generate code documentation. Go to the `doc` directory
and run
```
doxygen
```
to generate `html/index.html` and a LaTeX version.
