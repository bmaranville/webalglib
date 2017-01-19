#include "magnetic.cc"
#include <complex>
#include <emscripten/bind.h>
#include "json.hpp"

using namespace emscripten;
using json = nlohmann::json;

typedef std::vector<double> column; // return values holder

std::vector<Cplx> wrapped_Cr4xa(
      const std::vector<double> D, const std::vector<double> SIGMA,
      const int IP,
      const std::vector<double> RHO, const std::vector<double> IRHO,
      const std::vector<double> RHOM, const std::vector<Cplx> U1, const std::vector<Cplx> U3,
      const double AGUIDE, const double KZ)
{
    const int N = D.size();
    Cplx YA; 
    Cplx YB;
    Cplx YC;
    Cplx YD;
    Cr4xa(N, &D[0], &SIGMA[0], IP, &RHO[0], &IRHO[0], &RHOM[0], &U1[0], &U3[0], AGUIDE, KZ, YA, YB, YC, YD);
    return std::vector<Cplx>{ YA, YB, YC, YD};
}

/*
Cr4xa(const int &N, const double D[], const double SIGMA[],
      const int &IP,
      const double RHO[], const double IRHO[],
      const double RHOM[], const Cplx U1[], const Cplx U3[],
      const double &AGUIDE, const double &KZ,
      Cplx &YA, Cplx &YB, Cplx &YC, Cplx &YD)

*/

EMSCRIPTEN_BINDINGS(my_module) {
    class_<Cplx>("Cplx")
      .constructor<double, double>()
      .property("real", select_overload<double(void) const>(&Cplx::real), select_overload<void(double)>(&Cplx::real))
      .property("imag", select_overload<double(void) const>(&Cplx::imag), select_overload<void(double)>(&Cplx::imag))
    ;
    register_vector<double>("VectorDouble");
    register_vector<Cplx>("VectorComplex");
    register_vector<column>("VectorColumn");
    function("Cr4xa", &wrapped_Cr4xa);
}
