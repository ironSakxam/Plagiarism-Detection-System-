//addition of two complex numbers //   
//object return to function
#include<iostream>
using namespace std ;
class Complex {
 public :
    int real , img ;
};
 
Complex getValues(){
    Complex c ;
    c.real = 12 ;
    c.img = 2 ;
    return c ;

}
int main(){
   Complex c1 = getValues() ;
    cout << c1.real <<"+" << "i" << c1.img << endl ;

}