#include <iostream>  
using namespace std;
 class Rectangle { 
    private:  
    int length, breadth;   
    public:  
    void setSize(int length, int breadth) 
{   
    this->length = length;  
      this->breadth = breadth;    }     
    void getArea() { 
          int area = length * breadth;  
      cout << "Area of Rectangle = " << area << endl; 
} 
        void getPerimeter() {  
             int perimeter = 2 * (length + breadth);    
                 cout <<"Perimeter of Rectangle = " << perimeter << endl;  
  }  
 };   
  int main() { 
Rectangle r; 
r.setSize(3, 6); 
r.getArea(); 
r.getPerimeter(); 
return 0; 
 }   
