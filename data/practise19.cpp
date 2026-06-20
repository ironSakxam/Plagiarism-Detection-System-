// default //overloading //copy //
#include<iostream>
using namespace std ;
class Alpha {
    static int count ;
    public :
     Alpha(){
           count++ ;
           cout << "No. of object created :" <<count << endl ;
     }
    ~Alpha(){
        cout <<"No. of object destroyed :" <<count << endl ;
        count-- ;

     }
};
int Alpha::count ;
int main () {
    cout << "Enter Main " << endl ;
    Alpha A1 , A2, A3,A4 ; {
        cout << "Enter Block1 " << endl ;
        Alpha A5;
    }
    {cout << "Enter Block2" <<endl ;
         Alpha A6 ;
    }
    cout <<"Re-enter main" << endl ;
    return 0 ;
}