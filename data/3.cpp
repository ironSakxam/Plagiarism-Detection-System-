#include <iostream>  
#include <cmath>
  using namespace std;   
 class Point  {
     int x, y; 
       public:
         void setPoint(int a, int b)   
       {  
     x = a;  y = b;   
}    
 void distance(Point p1, Point p2)   
{    
    float d;   
             d = sqrt(((p2.x - p1.x) * (p2.x - p1.x)) +  ((p2.y - p1.y) * (p2.y - p1.y)));   
            cout << "Distance between two points = " << d;   
 }  
 };
    int main()   
 {   
 Point p1, p2, p3; 
     p1.setPoint(3, 3); 
     p2.setPoint(5, 8);  
     p3.distance(p1, p2);   
 return 0;  
 }