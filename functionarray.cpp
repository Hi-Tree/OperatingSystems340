#include <iostream>

using namespace std;

// Program to invoke arithmetic operations using function pointer array.

// Arithmetic functions for add, subtract, multiply, divide, and modulo.
int add(int i, int j) {

  cout << "Add: " << i << " and " << j << " = " <<  i+j << "\n";
  return i+j;

}

int subtract(int i, int j) {

  cout << "Subtract: " << i << " and " << j << " = " <<  i-j << "\n";
  return i-j;

}

int multiply(int i, int j) {
  cout << "Multiply: " << i << " and " << j << " = " <<  i*j << "\n";
  return i*j;

}

int divide(int i, int j) {
  cout << "Divide: " << i << " and " << j << " = " <<  i/j << "\n";
  return i/j;

}

int modulo (int i, int j) {
  cout << "Modulo: " << i << " and " << j << " = " <<  i%j << "\n";
  return i%j;

}

int main(int argc, const char *argv[]) { 

// Function Pointer Array holding addresses of the add, subtract, multiply, divide, and modulo functions.

	int (*comp[5]) (int i, int j) = {add, subtract, multiply, divide, modulo};

// Nested loop to invoke add, subtract, multiply, divide, and modulo functions with parameters i and j.	
         for (int i = 100; i < 110; ++i)
	      for (int j = 10; j < 20; ++j) 
                  for (int k = 0; k < 5; ++k) (*comp[k]) (i,j);

return 0;
}  
