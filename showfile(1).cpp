// Filename: showfile.cpp
// Compile command: g++ showfile.cpp -oshowfile.exe

#include <fcntl.h>
#include <unistd.h>

using namespace std;

const int SIZ=128;

// Display source file. Adapted from The C Programming Language
// by Kernighan and Ritchie.
int main(int argc, const char *argv[]) {

	int f1;	

// Do we have right number of arguments?

	if (argc != 2){
		write(STDERR_FILENO, "Wrong number of command line arguments\n", 36);
		return 1;
	}

// Can we access thge source file? 

	if ((f1 = open(argv[1], O_RDONLY, 0)) == -1){
		write(STDERR_FILENO, "Can't open input file \n", 23) ;
		return 2;
	}


// Display source file contents to output console screen. 

char buf[SIZ];
int n;

while ((n = read( f1, buf, SIZ)) > 0)           // Read from source file
	if (write(STDOUT_FILENO, buf, n) != n)  // Output to consoole screen
	{
		write(STDERR_FILENO, "Can't display file\n", 19) ;
		close(f1);
		return 3;
	}

close(f1);

write(STDOUT_FILENO, "\n\n*** Success! ***\n", 19) ;

return 0;
} 
