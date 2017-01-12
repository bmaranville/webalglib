#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "interpolation.h"
#include <emscripten/bind.h>

using std::string;
using std::vector;
//using namespace emscripten;

using namespace alglib;
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

void eval_func(
    void (*func)(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr),
    const real_1d_array &c, 
    const real_2d_array &x, 
    real_1d_array &y
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
        func(c, xx, yy, NULL);
        y[i] = yy;
    }
}

void test() {
    real_1d_array y = "[0.223130, 0.382893, 0.582748, 0.786628, 0.941765, 1.000000, 0.941765, 0.786628, 0.582748, 0.382893, 0.223130]";
    printf("%s\n", y.tostring(6).c_str());
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
    void (*func)(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
)
{
    real_2d_array x = xs.c_str();
    real_1d_array y = ys.c_str();
    real_1d_array w = ws.c_str();
    real_1d_array c = cs.c_str();
    real_1d_array bndl = lower_bound.c_str();
    real_1d_array bndu = upper_bound.c_str();
    string output = "{\n";
    
    double epsf = 0;
    double epsx = 0.000001;
    ae_int_t maxits = 0;
    ae_int_t info;
    lsfitstate state;
    lsfitreport rep;
    double diffstep = 0.0001;    
    
    lsfitcreatewf(x, y, w, c, diffstep, state);
    lsfitsetbc(state, bndl, bndu);
    lsfitsetcond(state, epsf, epsx, maxits);
    alglib::lsfitfit(state, func);
    lsfitresults(state, info, c, rep);
    
    real_1d_array y_fit;
    eval_func(func, c, x, y_fit);
    output += "  \"c\": " + c.tostring(6) + ",\n";
    output += "  \"c_err\": " + rep.errpar.tostring(6) + ",\n";
    output += "  \"iterations\": " + std::to_string(rep.iterationscount) + "\n";
    
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

EMSCRIPTEN_BINDINGS(fit_exp_module) {
    emscripten::register_vector<double>("VectorDouble");
    emscripten::function("test", &test);
    emscripten::function("test_vector", &test_vector);
    emscripten::function("test_string", &test_string);
    emscripten::function("fit_exp_decay", &fit_exp_decay);
    emscripten::function("fit_gaussian", &fit_gaussian);
};
