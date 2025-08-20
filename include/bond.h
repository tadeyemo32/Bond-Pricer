#ifndef BONDS_PRICER_BOND_H
#define BONDS_PRICER_BOND_H

#include <iostream> 

namespace Bonds{

class zc_Bond {
    private: 
    double FV;//face value
    double r;// rate 
    int m; // in years   
    public: 
    zc_Bond();
    zc_Bond(double face_value,double rate,int maturity);
    void price();
};




}







#endif 
