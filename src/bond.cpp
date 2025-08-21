#include "bond.h"

namespace Bonds {

Bond::Bond() : FV(0.0), r(0.0), T(0) {}
Bond::Bond(double face_value, double rate, int maturity) : FV(face_value), r(rate), T(maturity) {}

zc_Bond::zc_Bond() : Bond() {}
zc_Bond::zc_Bond(double face_value, double rate, int maturity) : Bond(face_value, rate, maturity) {}
double zc_Bond::price() const { return FV / std::pow(1+r, T); }
double zc_Bond::macaulay_duration() const { return T; }
double zc_Bond::modified_duration() const { return T / (1+r); }
double zc_Bond::convexity() const { return T*(T+1)/(1+r)*(1+r); }
double zc_Bond::ytm(double marketPrice, int maxIter, double tol) const {
    double y = r;
    for(int i=0;i<maxIter;i++){
        double f = FV/std::pow(1+y,T)-marketPrice;
        double df = -T*FV/std::pow(1+y,T+1);
        double y_new = y - f/df;
        if(std::abs(y_new-y)<tol) return y_new;
        y=y_new;
    }
    return y;
}

c_Bond::c_Bond() : Bond(), c(0.0), freq(1) {}
c_Bond::c_Bond(double face_value, double coupon_rate, double discount_rate, int maturity, int frequency)
    : Bond(face_value, discount_rate, maturity), c(coupon_rate), freq(frequency) {}

double c_Bond::price() const {
    double pv=0.0;
    for(int i=1;i<=T*freq;i++)
        pv += (FV*c/freq)/std::pow(1+r/freq,i);
    pv += FV/std::pow(1+r/freq,T*freq);
    return pv;
}

double c_Bond::macaulay_duration() const {
    double dur=0.0;
    double P = price();
    for(int i=1;i<=T*freq;i++)
        dur += i*(FV*c/freq)/std::pow(1+r/freq,i)/P;
    dur += T*freq*FV/std::pow(1+r/freq,T*freq)/P;
    return dur/freq;
}

double c_Bond::modified_duration() const { return macaulay_duration()/(1+r/freq); }

double c_Bond::convexity() const {
    double conv=0.0;
    double P = price();
    for(int i=1;i<=T*freq;i++)
        conv += i*(i+1)*(FV*c/freq)/std::pow(1+r/freq,i)/P;
    conv += T*freq*(T*freq+1)*FV/std::pow(1+r/freq,T*freq)/P;
    return conv/(freq*freq);
}

double c_Bond::current_yield(double marketPrice) const { return (FV*c)/marketPrice; }

double c_Bond::ytm(double marketPrice, int maxIter, double tol) const {
    double y=r;
    for(int i=0;i<maxIter;i++){
        double f=price()-marketPrice;
        double df=-modified_duration();
        double y_new=y-f/df;
        if(std::abs(y_new-y)<tol) return y_new;
        y=y_new;
    }
    return y;
}

}
