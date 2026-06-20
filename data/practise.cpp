#include<iostream>
using namespace std;

//first namespace
namespace first_space
{
    void func()
    {cout<<"Inside first_space"<<endl ;}
} 
//second namespace
namespace second_space {
void func() {cout<<"Inside second_space"<<endl ;
}

}
int main()
{
    //calls function from first name space
    first_space :: func() ; 
    //calls function from second name space
    second_space :: func() ;
    return 0 ;
}