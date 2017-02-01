# webalglib

Some wrappers for functions from the [ALGLIB C++ math library](http://www.alglib.net/) for use with web applications, using [Emscripten](http://emscripten.org/) to compile C++ to Javascript.

Examples include
* Levenberg-Marquardt fitting of gaussians, exponential decay and arbitrary Javascript functions (webfit.cpp)
* Compilation of neutron-reflectivity equations from C to Javascript (mag_wrap.cc and refl_wrap.cc)
* Combination of neutron reflectivity equations with ALGLIB LM-fitting: [NCNR online reflectivity calculator](https://www.ncnr.nist.gov/instruments/magik/calculators/calcR_d3_dark.html)

