// Filename: showfile.cpp
// Compile command: g++ runfile.cpp -orunfile.exe

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;


int main(int argc, const char *argv[]) {


// fork a child process.
pid_t pid = fork();

if (pid < 0) {
	write(STDERR_FILENO, "Can't fork the process\n", 23) ;
        return 2;
}
else if (pid == 0) {   // child process' code branch

        // Execute the showfile.exe program with one argument, the file to be displayed.
        int ret = execlp("./showfile.exe", "showfile.exe", "myfile.txt", (char *) NULL);
        if (ret < 0) {
		write(STDERR_FILENO, "Can't execute showfile.exe \n", 28) ;
                return 3;
        } 
}
else {   // parent process' code branch 
     int status;

     // Wait(block) for child to terminate.
     if (wait(&status) == pid)  {   

          // Program terminated normally (it didin't crash nor was aborted though it may have failed ).
           if  (WIFEXITED(status)) {    
                write(STDOUT_FILENO, "Child normal termination\n", 25) ;
                int childstat = WEXITSTATUS(status);
		cout << "Child exit status is " << childstat << endl;   // Better way to do this?
           } 
           // Program terminated abnormally.
           else if (WIFSIGNALED(status))  {
		write(STDOUT_FILENO, "Child abnormal termination\n", 24) ;
           }  
     }   
     else {   // wait() call failed by parent process.
	       write(STDERR_FILENO, "wait() call failed\n", 19) ;  
               return 4;
     }
}

 
return 0;
} 
