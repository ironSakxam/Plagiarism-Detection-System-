
//check whether the equation has real solutions or imaginary  with roots values//

#include <iostream>
#include <cmath>
using namespace std;
int main()
{
    float a,b,c,d,root1,root2 ;                     
    cout<<"Enter the coefficients :" ;
    cin>>a>>b>>c ;
                                                    
    d=b*b-4*a*c ;
     
    if(d>0) {                                                                 //check 1,5,6//
       root1 = (-b + sqrt(d)) / (2*a);
       root2 = (-b - sqrt(d)) / (2*a);
        cout<<"Solution is real and distinct"  <<endl;
        cout<<"Root 1 = " <<root1 <<endl;
        cout<<"Root 2 = " <<root2 <<endl;
    }
    else if(d==0) {                                                          //check 1,4,4
        root1 = -b / (2*a); 
        cout<<"Solution is real and equal"  <<endl;
        cout<<"Root 1 = " <<root1 <<endl;

    }
    else {
        cout<<"Solution is imaginary " <<endl ;                                        // check  1,2,5
        cout<<"Root 1 = " <<-b/(2*a) << " + " <<sqrt(-d)/(2*a) << "i" <<endl;               //n+i//
        cout<<"Root 2 = " <<-b/(2*a) << " - " <<sqrt(-d)/(2*a) << "i" <<endl;              //n-i//
    }
    return 0 ;
}