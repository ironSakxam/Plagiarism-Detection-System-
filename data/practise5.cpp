//wap to find the max of two numbers using inline fucntion//

#include<iostream>
using namespace std;

inline int largest(int x ,int y){
if(x>y)   return x;
   else   return y  ; }
int main()
{
int a,b ;
cout<<"Enter two numbers :" <<endl;
cin>>a>>b  ;
cout<<"The largest number is: " << largest(a,b) << endl;
}

