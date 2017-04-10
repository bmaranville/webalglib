#include "reflcalc.h"
#include <complex>
#include <emscripten/bind.h>

using namespace emscripten;
const double EPS = std::numeric_limits<double>::epsilon();
const double B2SLD = 2.31604654;  // Scattering factor for B field 1e-6

val vectorCplxToArray(const std::vector<Cplx> &r) {
    val r_array = val::array();

    for (size_t ri=0; ri<r.size(); ri++) {
        val c_array = val::array();
        c_array.set(0, r[ri].real());
        c_array.set(1, r[ri].imag());
        r_array.set(ri, c_array);
    }
    return r_array;
}
   
val triple_wrapped_magrefl(
    val depth,
    val sigma,
    val rho,
    val irho,
    val rhoM,
    val u1Real,
    val u1Imag,
    val u3Real,
    val u3Imag,
    double Aguide,
    val kz
)
{
    const std::vector<double> depth_d = vecFromJSArray<double>(depth);
    const std::vector<double> sigma_d = vecFromJSArray<double>(sigma);
    const std::vector<double> rho_d = vecFromJSArray<double>(rho);
    const std::vector<double> irho_d = vecFromJSArray<double>(irho);
    const std::vector<double> rhoM_d = vecFromJSArray<double>(rhoM);
    const std::vector<double> u1_r = vecFromJSArray<double>(u1Real);
    const std::vector<double> u1_i = vecFromJSArray<double>(u1Imag);
    const std::vector<double> u3_r = vecFromJSArray<double>(u3Real);
    const std::vector<double> u3_i = vecFromJSArray<double>(u3Imag);
    const std::vector<double> kz_d = vecFromJSArray<double>(kz);
    
    std::vector<Cplx> u1_c(u1_r.size());
    std::vector<Cplx> u3_c(u3_r.size());
    
    for (size_t ui=0; ui<u1_r.size(); ui++) {
        u1_c[ui].real(u1_r[ui]);
        u1_c[ui].imag(u1_i[ui]);
    }
        
    for (size_t ui=0; ui<u3_r.size(); ui++) {
        u3_c[ui].real(u3_r[ui]);
        u3_c[ui].imag(u3_i[ui]);
    }
    
    std::vector<Cplx> Ra(kz_d.size());
    std::vector<Cplx> Rb(kz_d.size());
    std::vector<Cplx> Rc(kz_d.size());
    std::vector<Cplx> Rd(kz_d.size());
    
    magnetic_amplitude(depth_d.size(), &depth_d[0], &sigma_d[0], &rho_d[0], &irho_d[0], &rhoM_d[0], &u1_c[0], &u3_c[0], Aguide, kz_d.size(), &kz_d[0], 0, &Ra[0], &Rb[0], &Rc[0], &Rd[0]);
    
    val output = val::object();
    output.set("Ra", vectorCplxToArray(Ra));
    output.set("Rb", vectorCplxToArray(Rb));
    output.set("Rc", vectorCplxToArray(Rc));
    output.set("Rd", vectorCplxToArray(Rd));
    
    return output;
}

val less_wrapped_magrefl(
    val depth,
    val sigma,
    val rho,
    val irho,
    val rhoM,
    val thetaM,
    double H,
    double Aguide,
    val kz
)
{
    const std::vector<double> depth_d = vecFromJSArray<double>(depth);
    const std::vector<double> sigma_d = vecFromJSArray<double>(sigma);
    const std::vector<double> rho_d = vecFromJSArray<double>(rho);
    const std::vector<double> irho_d = vecFromJSArray<double>(irho);
          std::vector<double> rhoM_d = vecFromJSArray<double>(rhoM);
    const std::vector<double> thetaM_d = vecFromJSArray<double>(thetaM);
    const std::vector<double> kz_d = vecFromJSArray<double>(kz);
    
    std::vector<Cplx> u1_c(rhoM_d.size());
    std::vector<Cplx> u3_c(rhoM_d.size());
                
    for (size_t i=0; i<rhoM_d.size(); i++) {
        calculate_U1_U3(H, rhoM_d[i], thetaM_d[i], Aguide, u1_c[i], u3_c[i]);
        //rhoM_d[i] = fabs(rhoM_d[i]);
    }
    
    std::vector<Cplx> Ra(kz_d.size());
    std::vector<Cplx> Rb(kz_d.size());
    std::vector<Cplx> Rc(kz_d.size());
    std::vector<Cplx> Rd(kz_d.size());
    
    //printf("rhoM[0]: %.4f\n", rhoM_d[0]);
    magnetic_amplitude(depth_d.size(), &depth_d[0], &sigma_d[0], &rho_d[0], &irho_d[0], &rhoM_d[0], &u1_c[0], &u3_c[0], Aguide, kz_d.size(), &kz_d[0], 0, &Ra[0], &Rb[0], &Rc[0], &Rd[0]);
    
    val output = val::object();
    output.set("Ra", vectorCplxToArray(Ra));
    output.set("Rb", vectorCplxToArray(Rb));
    output.set("Rc", vectorCplxToArray(Rc));
    output.set("Rd", vectorCplxToArray(Rd));
    
    return output;
}

EMSCRIPTEN_BINDINGS(my_module) {
    class_<Cplx>("Cplx")
      .constructor<double, double>()
      .property("real", select_overload<double(void) const>(&Cplx::real), select_overload<void(double)>(&Cplx::real))
      .property("imag", select_overload<double(void) const>(&Cplx::imag), select_overload<void(double)>(&Cplx::imag))
    ;
    register_vector<double>("VectorDouble");
    register_vector<Cplx>("VectorComplex");
    function("magrefl", &triple_wrapped_magrefl);
    function("magrefl_less", &less_wrapped_magrefl);
    //register_vector<column>("VectorColumn");
    //function("Cr4xa", &wrapped_Cr4xa);
}
