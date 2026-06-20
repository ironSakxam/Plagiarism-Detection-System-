// pass by reference //
//WAP using pass by reference that calculates the value of x and y from two linear equations ax+by = m and cx+dy = n. 
//The solutions are given as x = (md-bn)/(ad-bc) and y = (na-mc)/(ad-bc) 
//[ to check program : x – y = - 1 and 3x + y = 9; x = 2 ,y = 3]//

#include<iostream>
using namespace std ;
void   calculate(float &a, float &b, float &c, float &d, float &m, float &n, float &x, float &y) {
    x = (m*d-b * n) / (a*d- b*c);
    y = (n*a -m* c) / (a*d -b*c);
}   
int main() {
    float a, b, c, d, m, n, x, y;
    cout << "Enter coefficients a, b, c, d and constants m, n: "<<endl;
    cin >>a>>b>>c>>d>>m>>n;

    calculate(a, b, c, d, m, n, x, y);

    cout << "The solution is: x = " << x << ", y = " << y << endl;
    return 0;
}






