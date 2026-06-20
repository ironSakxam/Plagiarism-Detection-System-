// Task-II: Add two complex numbers using constructor and function returning object
#include <iostream>
using namespace std;

class Complex {
private:
    int real;
    int imag;

public:
    Complex() : real(0), imag(0) {}
   
    Complex(int r, int i) : real(r), imag(i) {}

    Complex add(const Complex &c1, const Complex &c2) {
        return Complex(c1.real + c2.real, c1.imag + c2.imag);
    }

    void display() const {
        cout << real << " + " << imag << "i";
    }
};

int main() {
    Complex c1(3, 4);  // 3 + 4i
    Complex c2(1, 2);  // 1 + 2i
    Complex sum = c1.add(c1, c2);

    cout << "First Complex Number: ";
    c1.display();
    cout << endl;

    cout << "Second Complex Number: ";
    c2.display();
    cout << endl;

    cout << "Sum of Complex Numbers: ";
    sum.display();
    cout << endl;

    cout << "Sum printed using temporary object directly: ";
    c1.add(c1, c2).display();
    cout << endl;

    return 0;
}
