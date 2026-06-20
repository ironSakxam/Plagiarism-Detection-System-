//wap to create a class rectangke with neccessary attributes and display the area of rectangle //

#include<iostream>
using namespace std ;
class Rectangle {
    public :
    int l , b ;
    void get_value(int l, int b) {
        l = l;
        b = b ;

    }
    void area(){
        cout <<"Area is " <<l*b << endl ;
      
    }

    } ;
    
    
    int main(){
        Rectangle R ;
        R.get_value(3,4) ;
        R.area() ;
        return 0 ;

    }