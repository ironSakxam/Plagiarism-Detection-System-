//wap to create a class rectangle with neccessary attributes and display the area of rectangle //

#include<iostream>
using namespace std ;
class Rectangle {
    public :
    int l , b ;
    public:

    void get_value(int , int );
   void area(){
        cout <<"Area is " <<l*b << endl ; 
    }
} ;
void Rectangle::get_value(int x , int y ){
        l = x;
        b = y;

    }
    
    
    int main(){
        Rectangle R ;
        R.get_value(3,4) ;
        R.area() ;
        return 0 ;

    }