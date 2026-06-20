//operator
#include<iostream>
using namespace std ;
class Counter {
    int count ;
    public :
    Counter() {
        count = 0 ;
    }
int getcount() {
    return count ;
}
Counter operator ++ (){
    Counter temp ;
    count = count + 1 ;
    temp.count = count ;
    return temp ;
}
}; 

int main() {
    Counter c1 , c2 ;
    cout << "c1 = " <<c1.getcount() <<endl ;
    cout << "c2 = " <<c2.getcount() <<endl ;
c2 = ++c1 ;
cout <<"c1 = "<< c1.getcount() <<endl ;
cout <<"c2 = "<< c2.getcount() <<endl ;

return 0 ;
}