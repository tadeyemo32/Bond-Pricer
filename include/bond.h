#ifndef BONDS_PRICER_BOND_H
#define BONDS_PRICER_BOND_H

#include <cmath>
#include <vector>
#include <algorithm>

namespace Bonds {

class Bond {
protected:
    double FV;
    double r;
    int T;
public:
    Bond();
    Bond(double face_value, double rate, int maturity);
    virtual double price() const = 0;
};

class zc_Bond : public Bond {
public:
    zc_Bond();
    zc_Bond(double face_value, double rate, int maturity);
    double price() const override;
    double macaulay_duration() const;
    double modified_duration() const;
    double convexity() const;
    double ytm(double marketPrice, int maxIter=1000, double tol=1e-6) const;
};

class c_Bond : public Bond {
private:
    double c;
    int freq;
public:
    c_Bond();
    c_Bond(double face_value, double coupon_rate, double discount_rate, int maturity, int frequency);
    double price() const override;
    double macaulay_duration() const;
    double modified_duration() const;
    double convexity() const;
    double current_yield(double marketPrice) const;
    double ytm(double marketPrice, int maxIter=1000, double tol=1e-6) const;
};

}
#endif

