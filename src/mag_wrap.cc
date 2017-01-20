#include "reflcalc.h"
#include <complex>
#include <emscripten/bind.h>

using namespace emscripten;
const double B2SLD = 2.31604654;  // Scattering factor for B field 1e-6

/*
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
*/


/*
    thetaM = radians(thetaM)
    phiH = radians(Aguide - 270.0)
    thetaH = np.pi/2.0 # by convention, H is in y-z plane so theta = pi/2
    
    sld_h = B2SLD * H
    sld_m_x = rhoM * np.cos(thetaM)
    sld_m_y = rhoM * np.sin(thetaM)
    sld_m_z = 0.0 # by Maxwell's equations, H_demag = mz so we'll just cancel it here
    # The purpose of AGUIDE is to rotate the z-axis of the sample coordinate
    # system so that it is aligned with the quantization axis z, defined to be
    # the direction of the magnetic field outside the sample.
    if rotate_M:
        # rotate the M vector instead of the transfer matrix!
        # First, rotate the M vector about the x axis:
        new_my = sld_m_z * sin(radians(Aguide)) + sld_m_y * cos(radians(Aguide))
        new_mz = sld_m_z * cos(radians(Aguide)) - sld_m_y * sin(radians(Aguide))
        sld_m_y, sld_m_z = new_my, new_mz
        sld_h_x = sld_h_y = 0.0
        sld_h_z = sld_h
        # Then, don't rotate the transfer matrix
        Aguide = 0.0
    else:        
        sld_h_x = sld_h * np.cos(thetaH) # zero
        sld_h_y = sld_h * np.sin(thetaH) * np.cos(phiH)
        sld_h_z = sld_h * np.sin(thetaH) * np.sin(phiH)
    
    sld_b_x = sld_h_x + sld_m_x
    sld_b_y = sld_h_y + sld_m_y
    sld_b_z = sld_h_z + sld_m_z

    # avoid divide-by-zero:
    sld_b_x += EPS*(sld_b_x==0)
    sld_b_y += EPS*(sld_b_y==0)

    # add epsilon to y, to avoid divide by zero errors?
    sld_b = np.sqrt(sld_b_x**2 + sld_b_y**2 + sld_b_z**2)
    u1_num = ( sld_b + sld_b_x + 1j*sld_b_y - sld_b_z )
    u1_den = ( sld_b + sld_b_x - 1j*sld_b_y + sld_b_z )
    u3_num = (-sld_b + sld_b_x + 1j*sld_b_y - sld_b_z ) 
    u3_den = (-sld_b + sld_b_x - 1j*sld_b_y + sld_b_z )
    
    u1 = u1_num/u1_den
    u3 = u3_num/u3_den

*/

/*
void
magnetic_amplitude(const int layers,
                   const double d[], const double sigma[],
                   const double rho[], const double irho[],
                   const double rhoM[], const Cplx u1[], const Cplx u3[],
                   const double Aguide,
                   const int points,
                   const double kz[], const int rho_offset[],
                   Cplx Ra[], Cplx Rb[], Cplx Rc[], Cplx Rd[]);
*/

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
    function("magrefl", &triple_wrapped_magrefl);
    //register_vector<column>("VectorColumn");
    //function("Cr4xa", &wrapped_Cr4xa);
}
