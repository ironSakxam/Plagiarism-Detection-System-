#include<iostream>
using namespace std ;

class Y;
class X {

int a;
public:
void setvalues(){
       a=10;
}
            friend void swap( X &,Y&);
            void display();
};

             void X :: display() {
            cout << a << endl;
}
class Y {
int b;
public:
void setvalues() {
b=20;
}
   friend void swap( X &,Y&);
    void display();
} ;
 

void Y::display(){
     cout << b << endl;
}  

      void swap(X &x, Y &y){
        int temp = x.a;
        x.a = y.b;
        y.b = temp;
      }

        int main() {
        X xobj;
        Y yobj;
              xobj.setvalues();
              yobj.setvalues();
              swap(xobj,yobj);
                xobj.display();
                yobj.display();
return 0;
}

