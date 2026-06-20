//same as practise 3 , use smaller , DMA


#include<iostream>
using namespace std;
int *num , i, smallest;
int main()
{
    num = new int[5];
    cout<<"Enter five numbers :" <<endl;
    for(i=0;i<5;i++){
        cin>>num[i];
    }
    for(i=0;i<5;i++){
        if(num[i]<smallest){
            smallest = num[i];
                            }
        }
        cout<<"Smallest number is: "<<smallest <<endl ;
        return 0;
}