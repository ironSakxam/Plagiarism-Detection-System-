//WAP to add 2 complex numbers using class and object
#include<iostream>
#include<cmath>
using namespace std;
class Complex
{
 float i,r;
   public:
    void get_values(float x,float y){
    i=x;
    r=y;
}Complex sum(Complex d)
{
Complex c1;
c1.r=r+d.r;
c1.i=i+d.i;
return c1;
}
void display(){
    cout<<("Addition of two complex numbers")<<endl;
    cout<<r<<"+"<<i<<"i"<<endl;
}

};

int main(){
   Complex C,Q,T;
   C.get_values(2,2);
   Q.get_values(2,2);
   T=Q.sum(C);
   T.display();
   return 0;

}