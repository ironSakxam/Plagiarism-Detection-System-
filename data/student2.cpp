// Sample C++ Program - Student 2
// A program to find sum of array numbers

#include <iostream>
using namespace std;

int main() {
    // Initialize array with values
    int numbers[] = {10, 20, 30, 40, 50};
    
    // Initialize sum variable to zero
    int total = 0;
    
    // Iterate through array
    for (int j = 0; j < 5; j++) {
        // Accumulate the sum
        total = total + numbers[j];
    }
    
    // Output the result
    cout << "Sum of array: " << total << endl;
    
    return 0;
}
