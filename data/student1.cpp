// Sample C++ Program - Student 1
// A simple program to calculate sum of array elements

#include <iostream>
using namespace std;

int main() {
    // Declare an array to store numbers
    int arr[] = {10, 20, 30, 40, 50};
    
    // Variable to store sum
    int sum = 0;
    
    // Loop through the array
    for (int i = 0; i < 5; i++) {
        // Add each element to sum
        sum = sum + arr[i];
    }
    
    // Print the result
    cout << "Sum of array elements: " << sum << endl;
    
    return 0;
}
