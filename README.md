# webalglib

Some wrappers for functions from the [ALGLIB C++ math library](http://www.alglib.net/) for use with web applications, using [Emscripten](http://emscripten.org/) to compile C++ to Javascript.

Examples include
* Levenberg-Marquardt fitting of gaussians, exponential decay and arbitrary Javascript functions [demo](https://bmaranville.github.io/webalglib/apps/webfit/app.html)
* Multi-Gaussian peak fitter with interactive fit initializer [demo](https://bmaranville.github.io/webalglib/apps/webfit/multipeak.html)
* Compilation of neutron-reflectivity equations from C to Javascript (src/mag_wrap.cc and src/refl_wrap.cc)
* Combination of neutron reflectivity equations with ALGLIB LM-fitting: [NCNR online reflectivity calculator](https://www.ncnr.nist.gov/instruments/magik/calculators/calcR_d3_dark.html)

