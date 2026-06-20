//wap to calculate volume of cube ,cuboid, cylinder using fucntion loading //

#include<iostream>
using namespace std ;
float volume(float l)
{ return l*l*l ;  }
float volume(float l, float b, float h)
{ return l*b*h ; }
float volume(float r, float h)
{ return 3.143*r*r*h  ;}

int main()
{
    cout<<"Volume of Cube :"<<volume(3)<<endl ;
    cout<<"Volume of cuboid :"<<volume(2,3,4)<<endl ;
    cout<<"Volume of Cylinder :"<<volume(1,1)<<endl ;
    return 0;

}