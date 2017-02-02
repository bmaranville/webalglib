#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "interpolation.h"
#include <emscripten/bind.h>
#include <emscripten.h>

using std::string;
using std::vector;
using emscripten::val;
using namespace emscripten;

using namespace alglib;

typedef void (*fptr)(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr);

void function_exp_decay(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    // this callback calculates f(c,x)=exp(-c0*sqr(x0))
    // where x is a position on X-axis and c is adjustable parameter
    func = exp(-c[0]*pow(x[0],2));
}

void function_gaussian(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr) 
{
    // c[0] = Amplitude
    // c[1] = center_x
    // c[2] = sigma_x
    // c[3] = y offset
    func = c[0] * exp(-pow((x[0] - c[1]),2)/(2.0*pow(c[2], 2))) + c[3];
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

val test() {
    real_1d_array x = "[0,1,2,4]";
    const double* xa = x.getcontent();
    const size_t length = x.length();
    std::vector<double> xv(xa, xa + length);
    val output = emscripten::val::array();
    val user_def = val::module_property("user_defined");
    user_def.set("func_string", val("myfunc"));
    
    return val(xv);
}

string user_defined(string func_def) {
    real_1d_array x = "[1e-7,1,2,4]";
    string x_def = "with({x: " + x.tostring(-6) + "}) ";
    string to_eval = x_def + func_def;
    //emscripten_run_script(to_eval);
    string output=emscripten_run_script_string(to_eval.c_str()); 
    return output;
}


void test_vector(vector<double> v) {
    real_1d_array y;
    y.setcontent(v.size(), &v[0]);
    printf("%s\n", y.tostring(6).c_str());
}

void test_string(string vs) {
    real_1d_array y = vs.c_str();
    printf("%s\n", y.tostring(6).c_str());
}




string fit_1d(
    const string xs,
    const string ys,
    const string ws,
    const string cs,
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
    string output = "{\n";
    
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
    lsfitsetcond(state, epsf, epsx, maxits);
    alglib::lsfitfit(state, func, NULL, option_ptr);
    lsfitresults(state, info, c, rep);
    
    real_1d_array y_fit;
    eval_func(func, c, x, y_fit, option_ptr);
    output += "  \"c\": " + c.tostring(6) + ",\n";
    output += "  \"c_err\": " + rep.errpar.tostring(6) + ",\n";
    output += "  \"iterations\": " + std::to_string(rep.iterationscount) + ",\n";
    output += "  \"rmserror\": " + std::to_string(rep.rmserror) + ",\n";
    output += "  \"wrmserror\": " + std::to_string(rep.wrmserror) + "\n";
    
    //output += "  \"y_fit\": " + y_fit.tostring(6) + "\n";
    output += "}"; 
    return output;
}

string fit_exp_decay(
    const string xs,
    const string ys,
    const string ws,
    const string cs,
    const string lower_bound,
    const string upper_bound
)
{
    return fit_1d(xs, ys, ws, cs, lower_bound, upper_bound, function_exp_decay);
}

string fit_gaussian(
    const string xs,
    const string ys,
    const string ws,
    const string cs,
    const string lower_bound,
    const string upper_bound
)
{
    return fit_1d(xs, ys, ws, cs, lower_bound, upper_bound, function_gaussian);
}

string fit_user_defined(
    const string xs,
    const string ys,
    const string ws,
    const string cs,
    const string lower_bound,
    const string upper_bound,
    emscripten::val user_func
)
{
    val::module_property("user_defined").set("compiled_function", user_func);
    
    return fit_1d(xs, ys, ws, cs, lower_bound, upper_bound, function_user_defined);
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

EMSCRIPTEN_BINDINGS(fit_exp_module) {
    emscripten::register_vector<double>("VectorDouble");
    emscripten::function("test", &test);
    emscripten::function("test_vector", &test_vector);
    emscripten::function("test_string", &test_string);
    emscripten::function("fit_exp_decay", &fit_exp_decay);
    emscripten::function("fit_gaussian", &fit_gaussian);
    emscripten::function("fit_user_defined", &fit_user_defined);
    emscripten::constant("user_defined", val::object());
    class_<Export_Math>("Math")
        .class_function("erf",  static_cast<double(*)(double)>(erf))
        .class_function("tanh", static_cast<double(*)(double)>(tanh))
        .class_function("cosh", static_cast<double(*)(double)>(cosh))
        .class_function("sinh", static_cast<double(*)(double)>(sinh))
        .class_function("brillouin", &brillouin);
        ;
};
