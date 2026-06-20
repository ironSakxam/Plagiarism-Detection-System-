// post increment 
#include<iostream>
using namespace std ;
class Counter {
    int count ;
    public :
    Counter() {
        count = 0 ;                                // why friend func is used for insertion and excertion overloading ????//
    }
int getcount() {
    return count ;
}
Counter operator ++ (int){
      Counter temp ;
    temp.count = count ;
     count = count + 1 ;
    return temp ;
}
}; 

int main() {
    Counter c1 , c2 ;
    cout << "c1 = " <<c1.getcount() <<endl ;
    cout << "c2 = " <<c2.getcount() <<endl ;
c2 =  c1++;
cout <<"c1 = "<< c1.getcount() <<endl ;
cout <<"c2 = "<< c2.getcount() <<endl ;

return 0 ;
}