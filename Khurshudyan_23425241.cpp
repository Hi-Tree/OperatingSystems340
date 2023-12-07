/*
Meri Khurshudyan 
CSCI 340 222
Project 1
*/

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

int main(int argc, const char *argv[]){
    if(argv[1]==nullptr){   
        write(STDOUT_FILENO,"Please specify command to run\n",31);
        exit(999);
    }
    int fd[2], df[2];
    ////////////////////////////////////////////////////////// PIPES /////////////////////////////////////////////////////////////////////

    if(pipe(fd) < 0){
        write(STDOUT_FILENO, "Pipe1 filed to crate\n", 21); //create one unidirectional pipe 
        return 1;
    }

    if(pipe(df) < 0){
        write(STDOUT_FILENO, "Pipe2 filed to crate\n", 21); //create another unidirectional pipe
        return 2;
    }

    ////////////////////////////////////////////////////////// FORK /////////////////////////////////////////////////////////////////////
    pid_t pid = fork(); //create child


    if (pid<0){
        write(STDOUT_FILENO, "Fork Failed\n", 13);
        return 3;
    }

    ///////////////////////////////////////////////////////// CHILD ////////////////////////////////////////////////////////////////////
    else if (pid==0){
        char message[10];

        //child will read from fd
        close (fd[1]);
        //child will write to df
        close (df[0]);


        int n = read(fd[0], message, 10); //read a char buffer of n bytes max buffer size is 10 characters

        struct timeval curTime;
        gettimeofday(&curTime,nullptr);
        time_t microSec = curTime.tv_usec;
        int Start_time = int(microSec);
        write(df[1], &Start_time, sizeof(Start_time)); //pipe out start time
        execlp(string(message,n).c_str(),string(message,n).c_str(),nullptr);
}

///////////////////////////////////////////////////////////// PARENT //////////////////////////////////////////////////////////////////
else{//parent
    int status, Stime;
    string message = argv[1];

    //parent will write to fd
    close (fd[0]);
    //parent will read from df
    close (df[1]);

    /* FIRST PRINT */
    string output = "Parent: before write to pipe1 argv[1]="+message+"\n";
    write(STDOUT_FILENO,output.c_str(),output.length());
    

    //// WRITE TO AND READ FROM PIPE
    write(fd[1], argv[1], message.length()); 
    int n = read(df[0], &Stime, sizeof(Stime));

    double Start_Time = double(Stime)/1000000;

    /* SECOND PRINT */
    output = "Parent: After read from pipe2 "+to_string(Start_Time)+"Sec\n";
    write(STDOUT_FILENO,output.c_str(),output.length());


    if (wait(&status) == pid)  {   //WAIT

          // Program terminated normally 
           if  (WIFEXITED(status)) {    
                struct timeval curTime;
                gettimeofday(&curTime,nullptr);
                time_t microSec = curTime.tv_usec;
                double End_time = double(microSec)/1000000;

                /* THIRD PRINT */
                output="Parent: Ending time is "+to_string(End_time)+"Sec\n";
                write(STDOUT_FILENO,output.c_str(),output.length());

                /* FOURTH PRINT */
                double duration = End_time-Start_Time;
                output="Parent: Elapsed time is: "+to_string(duration)+"Sec\n";
                write(STDOUT_FILENO,output.c_str(),output.length());
           } 
           // Program terminated abnormally
           else if (WIFSIGNALED(status))  {
		        write(STDOUT_FILENO, "Parent: Child abnormal termination\n", 32) ;
           }  
     }   
           else {   // wait() call failed by parent process
	            write(STDERR_FILENO, "parent: wait() call failed\n", 27) ;  
                return 8;
     }
}
}