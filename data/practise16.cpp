#include<iostream>
using namespace std ;
class Y ;
class X {
          int a,b;
        public:
            void setvalues() {
             a = 10 ;
                b = 20 ;
}   
void display(Y);
};

class Y {
    int c ;
    public :
    void setvalues();
    friend void X::display(Y) ;
};

void Y::setvalues() {
    c = 20 ;
}

void X::display(Y y ) {
    cout << a +b+y.c << endl ;
}

int main() {

X xobj;
Y yobj;
   
    xobj.setvalues();
    yobj.setvalues();
    xobj.display(yobj);

    return 0;
   }