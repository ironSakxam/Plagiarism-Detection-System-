//passing object and returning it using function//

#include <iostream>
using namespace std;

class Complex{
public :
    float real, imag ;
};
Complex add(Complex a, Complex b){
    Complex sum ;
    sum.real = a.real + b.real;
    sum.imag = a.imag + b.imag;
    return sum;
}
int main() {
    Complex c1, c2, sum;

    cout << "Enter first complex (real): " <<endl ;
    cin >> c1.real ;
    cout << "Enter first complex (imaginary): " <<endl ;
    cin >> c1.imag;

    cout << "Enter second complex (real): " <<endl ;
    cin >> c2.real ; 
    cout << "Enter second complex (imaginary): " <<endl ;
    cin >> c2.imag;

    sum = add(c1, c2);
    cout << "Sum of complex number  = " << sum.real << " + " << sum.imag << "i" ;
}