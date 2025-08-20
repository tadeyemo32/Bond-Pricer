#include <iostream> 
#include "bond.h"
#include <cmath>

namespace Bonds {

Bond::Bond() : FV(0), r(0), m(0) {
}

Bond::Bond(double face_value, double rate, int maturity) 
    : FV(face_value), r(rate), m(maturity) {
}

double Bond::price() const {
    return 0.0;
}


zc_Bond::zc_Bond(double face_value, double rate, int maturity) 
    : Bond(face_value, rate, maturity) {  
}

double zc_Bond::price() const {
    return FV / pow(1 + r, m); 
}


c_Bond::c_Bond(double face_value, double rate,double coupon_r, int maturity) 
    : Bond(face_value, rate, maturity),c(coupon_r) {
}

double c_Bond::price() const {
 
    double total = 0; 
    double coupon = FV*c;

    for (int t=1;t<=m;t++){
    total+= coupon/pow(1+r,t);
    }

    total+=FV/pow(1+r,m);
return total; 
}  

}


