/*Write a C++ program to demonstrate hierarchical inheritance. 
Create a base class Vehicle with a method start. Derive two classes Car and Bike from Vehicle.
 Each derived class should have its own method (drive) for Car and ride for Bike.
  In main, create objects of both Car and Bike and call their methods.
*/
#include<iostream>
using namespace std;
class Vehicle {
    public : 
    void start() {
        cout<< "Vehicle start" <<endl ;
    }

};
class Car : public Vehicle{
    public :
    void drive() {
        cout<<"Car driving" <<endl ;
    }

};
class Bike : public Vehicle {
    public :
    void Ride(){
        cout<<"BIke riding" <<endl ;
}
} ;

int main(){
     Car c ;
     Bike b ;

     cout <<"Car :" <<endl ;
     c.start() ;
     c.drive() ;

     cout<<"Bike :" <<endl ;
     b.start() ;
     b.Ride() ;

     return 0 ;
}