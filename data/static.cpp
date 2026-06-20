#include<iostream>
using namespace std ;
class Sample {
    int code ;
    static int count ; 
                       public :
   void set_code(){
   code = ++count  ;
    }
    void show_code() {
      cout << "object number" <<code << endl ;
   } 
   static void display_count() {
    cout << "count :" <<count << endl ;
    }
      };

    int Sample::count ;   

    int main() {
    Sample obj1 , obj2 ;
    obj1.set_code() ;
    obj2.set_code() ;
                Sample::display_count ;
          Sample obj3 ;
          obj3.set_code() ;
          Sample::display_count ;
         
          obj1.show_code() ;
          obj2.show_code() ;
          obj3.show_code() ;
    return 0 ;
   } 