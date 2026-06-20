// passing object by argument in a function//   //make class ,
// take object of that class and pass it as argument in func and display//

#include<iostream>
#include<string>
using namespace std ;
class Student {
    public : 
    int rollno ;
    string name ;
       
 };

void display(Student s) {
    cout << " Name of the student :" <<s.name << endl ;
    cout << " Rollno. of the student : " << s.rollno  << endl ;
}  
int main() {
    Student s1 ;
    s1.name = "Lutey Kaley" ;
    s1.rollno = 8 ;
    display(s1) ;
    return 0 ;
}
