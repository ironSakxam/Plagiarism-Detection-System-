//pointers and array 

#include<iostream>
using namespace std;

int *num ,i, largest;
int main()

{
    num =new int[10];
  cout<<"Enter ten numbers :"<<endl;
  for(i=0;i<10;i++)
  {
    cin>>num[i];

  }
        for(i=0;i<10;i++)
        {
            if(num[i]>largest)
            {
                largest =num[i];

            }
        }
        cout<<"Largest number is "<<largest <<endl ;
        return 0 ;

}