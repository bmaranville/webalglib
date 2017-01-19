#include "reflcalc.h"
#include <complex>
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

/*
   const int    layers,
             const double depth[],
             const double sigma[],
             const double rho[],
             const double irho[],
             const int    points,
             const double kz[],
             const int    rho_index[],
             Cplx r[])
*/

std::vector<Cplx> double_wrapped_refl(
    val depth,
    val sigma,
    val rho,
    val irho,
    //const int points,
    val kz
)
{
    const std::vector<double> depth_d = vecFromJSArray<double>(depth);
    const std::vector<double> sigma_d = vecFromJSArray<double>(sigma);
    const std::vector<double> rho_d = vecFromJSArray<double>(rho);
    const std::vector<double> irho_d = vecFromJSArray<double>(irho);
    const std::vector<double> kz_d = vecFromJSArray<double>(kz);
    
    std::vector<Cplx> r(kz_d.size());
    
    reflectivity_amplitude(depth_d.size(), &depth_d[0], &sigma_d[0], &rho_d[0], &irho_d[0], kz_d.size(), &kz_d[0], 0, &r[0]);
    
    return r;
}

val triple_wrapped_refl(
    val depth,
    val sigma,
    val rho,
    val irho,
    //const int points,
    val kz
)
{
    const std::vector<double> depth_d = vecFromJSArray<double>(depth);
    const std::vector<double> sigma_d = vecFromJSArray<double>(sigma);
    const std::vector<double> rho_d = vecFromJSArray<double>(rho);
    const std::vector<double> irho_d = vecFromJSArray<double>(irho);
    const std::vector<double> kz_d = vecFromJSArray<double>(kz);
    
    std::vector<Cplx> r(kz_d.size());
    
    reflectivity_amplitude(depth_d.size(), &depth_d[0], &sigma_d[0], &rho_d[0], &irho_d[0], kz_d.size(), &kz_d[0], 0, &r[0]);
    
    val r_array = val::array();
    
    for (size_t ri=0; ri<r.size(); ri++) {
        val c_array = val::array();
        c_array.set(0, r[ri].real());
        c_array.set(1, r[ri].imag());
        r_array.set(ri, c_array);
    }
    return r_array;
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
    function("refl", &triple_wrapped_refl);
}
