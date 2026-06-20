#include<iostream>
using namespace std ;
class student{
    public :                                               //call by value = copy pass -> no change in original value                                                            // & call by reference -> change in original value
    int marks ;                                            //& call by reference -> change in original value 
};                                                                           
void displayUpdated(student s) {
    s.marks = 100 ;
}
int main(){
    student s1 ;
    s1.marks = 50 ;                      // 100??? 
                                           //50???
    displayUpdated(s1);
    cout << s1.marks << endl ;
}

