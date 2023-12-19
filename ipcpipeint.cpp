// Filename: ipcpipe.cpp
// Compile command: g++ ipcpipe.cpp -oipcpipe.exe

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;



int main(int argc, const char *argv[]) {

int   n, fd[2];

// The pipe is created and its file descriptors returned.
if ( pipe(fd) < 0) {
	write(STDERR_FILENO, "Parent: Pipe failed\n", 20) ;
	return 1;
}


// fork a child process.
pid_t pid = fork();

if (pid < 0) {
	write(STDERR_FILENO, "Parent: Can't fork the process\n", 31) ;
        return 2;
}
else if (pid == 0) {   // child process' code branch
	int  childInt[4] = {0,0,0,0};

 	cout << "Child Before " << childInt[0] << " " << childInt[1] << " " ;
        cout << childInt[2] << " " << childInt[3] << endl;    // display what we got from parent

	close (fd[1]);       // not used by child so we will close 'write' end of pipe	

	n = read (fd[0], childInt, sizeof(childInt));   // read from the pipe what is sent by parent

        write(STDOUT_FILENO, "Child received: \n", 17); 
 	cout << "Child After " << childInt[0] << " " << childInt[1] << " " ;
        cout << childInt[2] << " " << childInt[3] << endl;    // display what we got from parent
         
}
else {   // parent process' code branch 
     int status;

      int  parentInt[4] = {1,2,3,4};

      close (fd[0]);    // not used by parent so close 'read' end of pipe
      write(STDOUT_FILENO, "Parent sending integers \n", 26); 
      write (fd[1], parentInt, sizeof(parentInt));  // put something in pipe for the child


     // Wait(block) for child to terminate.
     if (wait(&status) == pid)  {   

          // Program terminated normally (it didin't crash nor was aborted though it may have failed ).
           if  (WIFEXITED(status)) {    
                write(STDOUT_FILENO, "Parent: Child normal termination\n", 33) ;
                int childstat = WEXITSTATUS(status);
		cout << "Child exit status is " << childstat << endl;   // Better way to do this?
           } 
           // Program terminated abnormally.
           else if (WIFSIGNALED(status))  {
		write(STDOUT_FILENO, "Parent: Child abnormal termination\n", 32) ;
           }  
     }   
     else {   // wait() call failed by parent process.
	       write(STDERR_FILENO, "parent: wait() call failed\n", 27) ;  
               return 3;
     }
}

 
return 0;
} 
