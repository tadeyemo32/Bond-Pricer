#include <iostream> 
#include "bond.h"

namespace Bonds{

    Bonds::zc_Bond::zc_Bond(){
        std::cout<<"Empty Bond created"<<std::endl; 
    }        

Bonds::zc_Bond::zc_Bond(double face_value,double rate,int maturity):m(maturity),r(rate),FV(face_value){
    std::cout<<"Bond Created Face Value: "<<FV<<" ,Rate(%): "<<r<<" ,Maturity(T yrs): "<<m<<std::endl; 
}




}
