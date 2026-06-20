#include <iostream>
using namespace std;

// Base Class
class Person {
public:
    string name;

    void getName() {
        cout << "Enter Name: ";
        cin >> name;
    }
};

// Derived Class
class Student : public Person {
public:
    int roll;

    void getData() {
        cout << "Enter Roll No: ";
        cin >> roll;
    }

    void display() {
        cout << "\nStudent Details" << endl;
        cout << "Name: " << name << endl;
        cout << "Roll No: " << roll << endl;
    }
};

int main() {
    Student s;

    s.getName();
    s.getData();
    s.display();

    return 0;
}