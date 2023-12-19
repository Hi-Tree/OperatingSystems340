// Filename: sumthread.cpp
// Compile command: gcc sumthread.cpp -o sumthread.exe -pthread

#include <iostream>
#include <string>
#include <pthread.h>

using namespace std;

// Global variables
long sum = 0;                     // The final sum
long pSumArr[4] = {0, 0, 0, 0};  // The partial sums

// Parameter class used for invoking pthread function.
class MyParam {

public:
  MyParam(int l, int u, int i) :   lb(l), ub(u), lid(i) { };
  int getLower() { return lb;}
  int getUpper() { return ub;}
  int getLid()   { return lid;}

private:
  int lb, ub, lid;
};

// pthread function
void *myrunner (void *param) {
   MyParam *p = (MyParam *) param;

// Parameters: lower bound, upper boudn, and thread logical id, respectivel.    
   int lower = p->getLower();
   int upper = p->getUpper();
   int idx   = p->getLid();

// Get the partial sum.
   for (int i = lower; i < upper; ++i)
      pSumArr[idx] += i;

   string s = to_string(lower) + ',' + to_string(upper) + ',' + to_string(idx) + ',' + to_string(pSumArr[idx]) + '\n';
   cout << s;

   pthread_exit(0);
} 

int main(int argc, const char *argv[]) {

// Create the parameters for each of the 4 pthreads.
MyParam *p[4];
p[0] = new MyParam(1,   100, 0);
p[1] = new MyParam(101, 200, 1);
p[2] = new MyParam(201, 300, 2);
p[3] = new MyParam(301, 400, 3);

// Create the pthreads
pthread_t tid[4];
pthread_attr_t attr[4];

for (int i = 0; i < 4; ++i) {
   pthread_attr_init(&(attr[i]));
   pthread_create(&(tid[i]), &(attr[i]), myrunner, p[i]);
   cout << "Created thread\n";
}

// wait for pthreads to finish 
for (int i = 0; i < 4; ++i) {
   pthread_join(tid[i], NULL);
   cout << "Joined thread\n"; 
}

// Get and print final sum
for (int i = 0; i < 4; ++i)
   sum += pSumArr[i];

cout << "Sum of 1 to 400 is : " << sum << endl;

// Cleanup
for (int i = 0; i < 4; ++i) delete p[i];

return 0;
} 
