#include <iostream> 
#include "bond.h"



int main(){

Bonds::zc_Bond myBond = {1000,0.05,10};
Bonds::c_Bond myBond2 = {1000,0.04,0.05,3};

std::cout<<myBond.price()<< " --> "<< myBond2.price()<<std::endl; 


}
