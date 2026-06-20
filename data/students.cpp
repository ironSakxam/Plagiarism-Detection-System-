// class for 10 students to store their roll no and full name of student //

#include<iostream>
#include<string>
using namespace std ;
class Student { 
    public :
    
    string name ;
    int rollno ;
    string faculty ;

    public :

void inputDetails() {
    cout << "Enter the name of the student : " ;
    getline(cin , name ) ;
    cout << "Enter roll no. of the student : " ;
    cin >> rollno ;
    cin.ignore() ;
    cout << "Enter faculty :";
    getline(cin , faculty ) ;
}  
 void displayDetails() {
    cout << " Name of the student : " << name << endl ;
    cout << " Roll no, of the student : " << rollno <<endl ;
    cout << " Faculty  : " << faculty <<endl ;
    
 }

} ;

int main() {
    Student students[10] ;
   
    //input
    for (int i =0 ; i<1 ; i++) {
        cout << "Student Details "<< i+1 << ":" <<endl ;
        students[i].inputDetails() ;   }
   
        //display
        cout << "Diplay Details of all Students : " << endl ;
   for(int i =0 ;i<1 ;i++) {
     cout <<" Students Details " <<i+1 << ":" <<endl ;
     students[i].displayDetails() ;
    }
     return 0 ;
}
    