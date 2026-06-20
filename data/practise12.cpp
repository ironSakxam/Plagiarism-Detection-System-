// wap to calculate area of triangle//

#include<iostream>
using namespace std ;
class Triangle {
    public :
    int base , height ;

void getValues(int x , int y) {
    base = x ;
    height = y ;

}
void area() {
    cout<<"Area is =" <<base*height << endl ;
}
} ;

int main() {
     Triangle T ;
     T.getValues(5,6) ;
     T.area() ;
     return 0 ;
   }