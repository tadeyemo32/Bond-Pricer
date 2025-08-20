 #ifndef BONDS_PRICER_BOND_H
#define BONDS_PRICER_BOND_H

#include <iostream> 

namespace Bonds {

class Bond {
protected:
     double FV;
    double r;
    int m;
public:
    Bond();
    Bond(double face_value, double rate, int maturity);
    virtual double price() const;
};

class zc_Bond : public Bond {
public: 
    zc_Bond();
    zc_Bond(double face_value, double rate, int maturity);
    double price() const override;
};

class c_Bond : public Bond {
private:
    double c; 
    
public: 
    c_Bond();
    c_Bond(double face_value, double rate,double coupon_r, int maturity);
    double price() const override;
};

}
#endif 
