#include <iostream>   
using namespace std;    
 class Count  {
        static int x;    
 public:    
 static void show() {    
            x++;  
     cout << "Value of x = " << x  << endl;    
      }     
} ; 
  int Count::x = 0;    
 
  int main()  {    
 Count::show();    
 Count::show();    
 Count::show();    
 return 0; 
              }     
   
