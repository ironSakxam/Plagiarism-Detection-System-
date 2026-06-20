#include<iostream>
using namespace std ;
class Box {
    double width ;
    public :
    friend void printWidth(Box &box) ;
    void set_Width(double wid) ;
} ;

void Box::set_Width(double wid)
{
    width = wid ;
}
void printWidth(Box &box) {
    cout << "Width of the box = " <<box.width <<endl ;
}

int main() {
    Box box ;
    box.set_Width(10.0) ;
    printWidth (box) ;
return 0 ;
}



