#include <iostream>
#include <sys/types.h>     
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>

using namespace std;

// Producer (Parent process) and Consumer (Child process) source code using POSIX shared memory IPC.

int main(int argc, const char *argv[]) { 

const int BUFFER_SIZE = 10;    // Number of elements in our array Buffer for Producer and Consumer


// Create the shared memory entity (object) for "SHINTIN" and assign it an identifier.

const char *nameIntIn = "SHINTIN";
int fdIn;

if ( ( fdIn = shm_open(nameIntIn, O_CREAT | O_RDWR, 0666)) < 0) {

	cerr << "shm_open(nameIntIn, O_CREAT | O_RDWR, 0666) Failed" << endl;
	return 1;	
}

// Set the size of the shared memory for "SHINTIN".
ftruncate(fdIn, sizeof(int));


// Provide a memory address *ptrIn for access to this shared memory object "SHINTIN".
int *ptrIn;

if ( (ptrIn = (int *)  mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fdIn, 0)) < 0)
{
	cerr << "ptrIn = (int *)  mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdIn, 0) Failed" << endl;
	return 2;	
}


// Create the shared memory entity (object) for "SHINTOUT" and assign it an identifier.
const char *nameIntOut = "SHINTOUT";
int fdOut;
if ( ( fdOut = shm_open(nameIntOut, O_CREAT | O_RDWR, 0666)) < 0) {

	cerr << "shm_open(nameIntOut, O_CREAT | O_RDWR, 0666) Failed" << endl;
	return 3;	
}

// Set the size of the shared memory for "SHINTOUT".
ftruncate(fdOut, sizeof(int));


// Provide a memory address *ptrOut for access to this shared memory object "SHINTOUT".
int *ptrOut;
if ( (ptrOut = (int *)  mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fdOut, 0)) < 0)
{
	cerr << "ptrOut = (int *)  mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdOut, 0) Failed" << endl;
	return 4;	
}


// Create the shared memory entity (object) for "SHINTBUFF" and assign it an identifier.
const char *nameIntBuff = "SHINTBUFF";
int fdBuff;

if ( ( fdBuff = shm_open(nameIntBuff, O_CREAT | O_RDWR, 0666)) < 0) {

	cerr << "shm_open(nameIntBuff, O_CREAT | O_RDWR, 0666) Failed" << endl;
	return 5;	
}

// Set the size of the shared memory for "SHINTBUFF".
ftruncate(fdBuff, BUFFER_SIZE * sizeof(int));


/* Pointers to shared memory objects for shIntIn and shIntOut */
int *ptrBuff;

if ( (ptrBuff = (int *)  mmap(0, BUFFER_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fdBuff, 0)) < 0)
{
	cerr << "ptrBuff = (int *)  mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdTBuff, 0) Failed" << endl;
	return 6;	
}


// Initialize the IN and OUT variables of the Producer Consumer algorithm.
*ptrIn = *ptrOut = 0;


// Parent is producer and child is consumer.
pid_t pid = fork();

if (pid < 0) {
	cerr << "Fork Failed" << endl;
	return 7;	
}

else if (pid == 0) {  // Child process branch "consumes" (reads) from shared mem.

	while (1) {
		while (*ptrIn == *ptrOut);

		int next_consumed = *(ptrBuff + *ptrOut);    // child's private variable that parent doesn't see.

		int tmpOut = *ptrOut;

		cout << "Child: next element consumed at " << *ptrOut << " has value " << next_consumed << endl << endl;  

		*ptrOut = ( (*ptrOut) + 1) % BUFFER_SIZE;
	
                // We break from this loop after a number of iterations, 35.  
		if (next_consumed == 35 ) break;
		}


		cout << "Child: after consumer loop ... terminating " << endl;
		return 0;
}

else { // Parent proccess branch "produces" (writes) to shared mem. 

	int next_produced = 0;   // Parent's private variable that child doesn't see.

	while (1) {
 
 		while( ( (*(ptrIn) + 1 ) % BUFFER_SIZE) == *ptrOut);

		*(ptrBuff + *ptrIn) = next_produced;

		cout << "Parent: next element produced at " << *ptrIn << " has value " <<  next_produced << endl << endl;

	 	*ptrIn = (*ptrIn + 1) % BUFFER_SIZE;

                // We break from this loop after a number of iterations, 35. 
		if (next_produced++ == 35) break; 
                } 

		cout << "Parent: after producer loop.. waiting for child termination " << endl;

               int status;

               // Wait(block) for child to terminate.
               if (wait(&status) == pid)  {   

                // Program terminated normally (it didin't crash nor was aborted though it may have failed ).
                  if  (WIFEXITED(status)) {    
                       wcout << "Parent: Child normal termination" << endl;
                       int childstat = WEXITSTATUS(status);
		       cout << "Parent: Child exit status is " << childstat << endl;   // Better way to do this?
                  } 
                // Program terminated abnormally.
                  else if (WIFSIGNALED(status))  {
		      cerr << "Parent: Child abnormal termination" << endl ;
                  }  
              }   
              else {   // wait() call failed by parent process.
	            cerr << "Parent: wait() call failed" << endl ;  
               }

	      // Remove the shared memory objects.
	      shm_unlink(nameIntIn);
	      shm_unlink(nameIntOut);
	      shm_unlink(nameIntBuff);

	      cout << "Parent: Terminating " << endl;
	      return 0;
     }   
}  
