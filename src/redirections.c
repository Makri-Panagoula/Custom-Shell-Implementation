#include "../headers/redirections.h"

//Redirects output aka handles '>' symbol
void redirect_output(const char* file){

    int out_fd = open(file,O_CREAT|O_WRONLY|O_APPEND,0666);           //keep in mind to use chmod to read the contents of a file in case you create it
    close(1);               //close standard output fd
    dup2(out_fd,1);         //use file as stdout
    close(out_fd);          //restore
}

//Redirects output only when given file already exists aka handles '>>' symbol
void redirect_output_existent(const char* file){
    int out_fd = open(file,O_APPEND|O_RDWR);           //same as previously only now we don't create it incase it doesn't exist
    close(1);                                          
    dup2(out_fd,1);                                   
    close(out_fd);
}

//Redirects input aka handles '<' symbol
void redirect_input(const char* file){

    int in_fd = open(file,O_RDONLY);
    close(0);                           //close standard input fd
    dup2(in_fd,0);                     //use file as stdin
    close(in_fd);                      //restore
}

//Deals  with |
void create_pipe(char** arg , int it , int wait , pid_t* leader) {

    int fd[2] ;
    pid_t pid;

    if (pipe(fd) == -1){ perror("pipe"); exit(1); }
    
    // child & writer 
    if ( (pid = fork()) == 0 ) {    

        close(fd[READ]);            //keeping the unidirectionality => close reading end
        dup2(fd[WRITE],1);          //copy stdout in the writing end of the pipe
        close(fd[WRITE]);
        // place null where the '|' character is since we only execute left part of the command
        //left part is absolutely necessary since it becomes input for the right
        arg[it] = NULL;      

        //  When the entire command is executed in the background create a process group with all of the processes in command
        if(wait) {
            if( ! *leader ) {           //If it is the first one (leader will be 0 from initialization) set leader as current pid
                setpgid(pid,0);   
                *leader = pid ;
            }
            else                            //Otherwise , add process to group with leader
                setpgid(pid,*leader);
        }

        if(execvp(arg[0],arg) == -1)
            printf("Failed to execute because %s\n", strerror(errno));
        exit(1);        
    }   
    else { 
        //We don't wait here for the child but in the end of symbol processing , when we have forked the last process
        //so that we have a parallel execution ( there will be some ms difference ).(The left part of the command is executed
        //either when we find another symbol (as soon as we have parsed the command) or once we ae done symbol processing.

        close(fd[WRITE]);
        dup2(fd[READ],0);
        close(fd[READ]);
        if(wait) {                  //Parent should call setpgid for child process as well in case of process groups and bg execution , symmetrical case
            if( ! *leader ) {
                setpgid(pid,pid);
                *leader = pid ;
            }
            else
                setpgid(pid,*leader);
        }
    }
}

//Update arguments array , iterator , tokens number after processing pipe command
char** after_pipe(char** arg,int* cur,int* num) {

    // We have executed left part of the command, so only the right one remains now => "update" array so that we won't execute the same command again
    //since we shift the beginning of the array we have to change the rest of the parameters for consistency.Parameters will be referring to the first command
    //to be executed again.

    int new_num = *num - *cur - 1;                              //Remaining elements
    char** new = (char** ) malloc(new_num * sizeof(char*));
    int copy = 0;

    //Copy the remaining part of the array in the new one

    for(int i = *cur + 1; i < *num; i++) {

        new[copy] = malloc((strlen(arg[i]) + 1));
        strcpy(new[copy++],arg[i]);       
    }

    dealloc_arg(arg,*num);      //Deallocate memory for old array
    *num = new_num;
    *cur = 0 ;
    return new;
}

