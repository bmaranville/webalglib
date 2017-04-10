#include "reflcalc.h"
#include "interpolation.h"
#include <emscripten/bind.h>
#include <emscripten.h>

using std::string;
using std::vector;
using std::norm;
using emscripten::val;
using namespace emscripten;

using namespace alglib;

typedef void (*fptr)(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr);


void function_magrefl(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) {
    // the sld table is broken down into c parameters, with columns:
    // depth, sigma, rho, irho, rhoM and thetaM plus double-precision values for H and AGUIDE
    // so c should always have size (6*n) + 2
    const int num_rows = (c.length() - 2) / 6;
    int offset = 0;
    
    real_1d_array cc = c;

    const double *D = &cc[num_rows * offset++];
    const double *SIGMA = &cc[num_rows * offset++ + 1];
    const double *RHO = &cc[num_rows * offset++];
    const double *IRHO = &cc[num_rows * offset++];
          double *RHOM = &cc[num_rows * offset++];
    const double *thetaM = &cc[num_rows * offset];
    
    const double H = cc[num_rows*offset + 1];
    const double AGUIDE = cc[num_rows*offset + 2];
    
    vector<Cplx> U1(num_rows);
    vector<Cplx> U3(num_rows);    
    
    for (int i=0; i<num_rows; i++) {
        calculate_U1_U3(H, RHOM[i], thetaM[i] * M_PI, AGUIDE, U1[i], U3[i]);
    }
    // done with conversion : do calculation
    
    const int cross_section = std::round(x[1]);
    const int IP = (cross_section < 2) ? 1 : -1;
    const double KZ = x[0];
    
    // cross-section order:  [++, +-, -+, --]
    Cplx R[4];
    
    // use AGUIDE = 0 after calculating U1, U3;
    Cr4xa(num_rows, D, SIGMA, IP, RHO, IRHO, RHOM, &U1[0], &U3[0], 0, KZ, R[0], R[1], R[2], R[3]);
    
    func = norm(R[cross_section]);
}

void function_refl(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) {
    // the sld table is broken down into c parameters, with columns:
    // depth, sigma, rho, irho
    // so c should always have size (4*n)
    
    const int num_rows = ((int)c.length()) / 4;
    int offset = 0;

    const double *D     = &c[num_rows * offset++];
    const double *SIGMA = &c[num_rows * offset++ + 1]; // skip the first sigma
    const double *RHO   = &c[num_rows * offset++];
    const double *IRHO  = &c[num_rows * offset++];
    
    Cplx R;
    
    refl(num_rows, x[0], D, SIGMA, RHO, IRHO, R);
    
    func = norm(R);
}


void eval_func(
    fptr func,
    const real_1d_array &c, 
    const real_2d_array &x, 
    real_1d_array &y,
    void* option_ptr=NULL
) 
{
    int length = x.rows();
    real_1d_array xx;
    xx.setlength(1);
    double yy = 0.0;
    y.setlength(length);
    for (int i=0; i<length; i++) {
        xx.setcontent(1, x[i]); 
        //xx[0] = x[i][0];
        func(c, xx, yy, option_ptr);
        y[i] = yy;
    }
}

string fit_1d(
    const string xs,
    const string ys,
    const string ws,
    const string cs,
    const string ss,
    const string lower_bound,
    const string upper_bound,
    fptr func,
    void* option_ptr=NULL
)
{
    real_2d_array x = xs.c_str();
    real_1d_array y = ys.c_str();
    real_1d_array w = ws.c_str();
    real_1d_array c = cs.c_str();
    real_1d_array s = ss.c_str();
        
    double epsf = 0;
    double epsx = 0.000001;
    ae_int_t maxits = 0;
    ae_int_t info;
    lsfitstate state;
    lsfitreport rep;
    double diffstep = 0.0001;    
    
    lsfitcreatewf(x, y, w, c, diffstep, state);
    if (lower_bound.length() > 0 && upper_bound.length() > 0) { 
        real_1d_array bndl = lower_bound.c_str();
        real_1d_array bndu = upper_bound.c_str();
        lsfitsetbc(state, bndl, bndu);
    }
    alglib::lsfitsetscale(state, s);
    lsfitsetcond(state, epsf, epsx, maxits);
    alglib::lsfitfit(state, func, NULL, option_ptr);
    lsfitresults(state, info, c, rep);
    
    real_1d_array y_fit;
    eval_func(func, c, x, y_fit, option_ptr);
    
    string output = "{\n";
    output += "  \"c\": " + c.tostring(6);
    output += ",\n  \"c_err\": " + rep.errpar.tostring(6);
    output += ",\n  \"iterations\": " + std::to_string(rep.iterationscount);
    output += ",\n  \"rmserror\": " + std::to_string(rep.rmserror);
    output += ",\n  \"wrmserror\": " + std::to_string(rep.wrmserror);
    
    output += ",\n  \"y_fit\": " + y_fit.tostring(6);
    output += "\n}"; 
    return output;
}

string fit_refl(
    const string xs,
    const string ys,
    const string ws,
    const string cs,
    const string ss,
    const string lower_bound,
    const string upper_bound
)
{
    return fit_1d(xs, ys, ws, cs, ss, lower_bound, upper_bound, function_refl);
}


string fit_magrefl(
    const string xs,
    const string ys,
    const string ws,
    const string cs,
    const string ss,
    const string lower_bound,
    const string upper_bound
)
{
    return fit_1d(xs, ys, ws, cs, ss, lower_bound, upper_bound, function_magrefl);
}

void function_user_defined(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{   
    val c_array = val::array();
    for (size_t ci=0; ci<c.length(); ci++) {
        c_array.set(ci, c[ci]);
    }
    val user_defined = val::module_property("user_defined")["compiled_function"];
    val output = user_defined(c_array, x[0]);

    func = output.as<double>();
}

string fit_user_defined(
    const string xs,
    const string ys,
    const string ws,
    const string cs,
    const string ss,
    const string lower_bound,
    const string upper_bound,
    emscripten::val user_func
)
{
    val::module_property("user_defined").set("compiled_function", user_func);
    
    return fit_1d(xs, ys, ws, cs, ss, lower_bound, upper_bound, function_user_defined);
}

struct Export_Math {
};

double brillouin( double J, double x ) {
    if ( x == 0 ) { return 0; } 
    else {
        return (2*J+1)/(2*J)/tanh((2*J+1)/(2*J)*x) - 
            1/(2*J)/tanh(1/(2*J)*x);
    }
}

EMSCRIPTEN_BINDINGS(fit_refl_module) {
    emscripten::function("fit_refl", &fit_refl);
    emscripten::function("fit_magrefl", &fit_magrefl);
    emscripten::function("fit_user_defined", &fit_user_defined);
    emscripten::constant("user_defined", val::object());
    class_<Export_Math>("Math")
        .class_function("erf",  static_cast<double(*)(double)>(erf))
        .class_function("tanh", static_cast<double(*)(double)>(tanh))
        .class_function("cosh", static_cast<double(*)(double)>(cosh))
        .class_function("sinh", static_cast<double(*)(double)>(sinh))
        .class_function("brillouin", &brillouin)
        ;
};
