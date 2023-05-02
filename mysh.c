#include <sys/ipc.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/shm.h>
#include "redirections.h"
#include "commands.h"

int pending_signal = 0;

void signal_catcher(int signo) {
    
    pending_signal = signo;
    printf("Signal Cought \n");
}

int main(int argc,char** argv) {

    //Create & Initialize sigaction structure, we work with it since it is more reliable
    static struct sigaction act ;
    act.sa_handler = signal_catcher;                      
    sigfillset(&(act.sa_mask));                     // ignore every signal when you are in signal handler
    sigaction (SIGINT , &act , NULL );             
    sigaction (SIGTSTP , &act , NULL ); 

    //Register signal handler to avoid zombie processes in case we have background commands where we don't wait the process(once it is finished it returns SIGCHLD)
    static struct sigaction bg ;                          
    bg.sa_handler = SIG_IGN;                         
    sigfillset(&(bg.sa_mask));                     
    sigaction (SIGCHLD , &bg , NULL );  
    
    //We keep leader process for a background command in shared memory sice the child process might update and parent should access the newest content
    int id = shm_open("shared", O_CREAT | O_RDWR, 0666);
    ftruncate(id, sizeof(pid_t));
    pid_t* leader = (pid_t*) mmap(0, sizeof(pid_t),PROT_READ |  PROT_WRITE, MAP_SHARED, id, 0);
    int status;

    alias_info info = init(DEFAULT_ALIASES);    

    printf("\nin-mysh-now:> ");           //ready for next command
    fflush(stdout);
    
    char* history[20];    
    int commands = 0 ;
    char* buf = (char*) malloc(1000);                          //reads command
    ssize_t bytesread = read(0,buf,1000);
    char* copy = (char*) malloc(1000);                          //temp string to keep a copy of buf

    int loop = 1;                    //Boolean value to see whether we should keep on looping
    int exitcode = 0 ;              //Program's(shell's) exitcode
    int token_num;                  //Number of tokens in current command
    char** prev = NULL;             //Previous command
    char** arg;                     //Parsed String array

    while(loop) {         

        strcpy(copy,buf);   

        //Determines whether to choose line from archive(history) or read in the next iteration
        int archive = 0;
        char* end; 
        for (char *sentence = strtok_r(buf, "\n", &end);  sentence != NULL; sentence = strtok_r(NULL, "\n", &end))  {

            char *end_str;
            //For each command in the command line , fork and execute child with the order they appear
            for (char *token = strtok_r(sentence, ";", &end_str);  token != NULL; token = strtok_r(NULL, ";", &end_str))  {

                //Child processes we have to wait for
                int process_num  = 1;
                //Determines whether to wait for child process
                int wait = 1 ; 

                int pos = find_alias(info,token,&token_num,&wait,&process_num);
                if (pos != -1 )   {                     //Check if given command is an alias
                    arg = info->aliases[pos]->actual; 
                    prev = NULL;    
                }                   
                else {
                    arg = parse(prev,token,&token_num,&wait,&process_num);
                    prev = arg;
                }

                if( special_command( arg, token_num, info, &archive, commands, history, &loop,  &exitcode, &copy , wait, process_num) )         //Command is executed, skip the loop
                    continue;

                *leader = 0;
                pid_t pid;
                if ( (pid = fork()) == 0 ) { 
                    //Processing special command info
                    for(int i = 1; i < token_num ; i++ ) {

                        if( ! strcmp(arg[i],">") ) {
                            redirect_output(arg[i+1]);
                            arg[i] = NULL ;                 //Output redirection is obviously gonna be the last part of the command (since we have estimated output)
                        }
                        else if ( ! strcmp(arg[i],"<")) {
                            redirect_input(arg[i+1]);
                            arg[i] = NULL ;
                        }
                        else if ( ! strcmp(arg[i],">>")) {
                            redirect_output_existent(arg[i+1]);
                            arg[i] = NULL ;
                        }   
                        else if ( ! strcmp(arg[i],"|")) {                           
                            create_pipe(arg,i,wait,leader);   
                            arg =  after_pipe(arg,&i,&token_num);  
                        }
                    }    

                    if(wait) {                                            //Background commands don't receive signals
                        //Restore signal handler to SIGDFL
                        act.sa_handler = SIG_DFL ; // reestablish DEFAULT behavior
                        sigaction (SIGINT , &act , NULL ); // default for control –c
                        sigaction (SIGTSTP , &act , NULL ); // default for control -z

                        if( pending_signal )                //We received a signal meant for child process while we were in parent process , we have to execute it now
                            raise(pending_signal);
                    }  

                    arg[token_num]=NULL;  

                    if( execvp(arg[0],arg) == -1) {

                        printf("Failed to execute %s  because %s\n", arg[0],strerror(errno));
                        exit(1);                        
                    }
                }
                else {                                                   //Parent Process
                    if(wait) {                                          //Wait for processes if-f it isn't in the background 

                        for(int i = 0; i < process_num; i++ )
                            waitpid(0,&status,WUNTRACED | WCONTINUED);  
                    }
                    //if there had been a pending signal it is now executed so initialize with 0 again so that next commands will be executed normally 
                    pending_signal = 0;             
                }
            }
        }
        if( ! archive && loop) {                           //Read next line if and only if we won't be executing one from history
            fflush(stdout);
            printf("\nin-mysh-now:> ");                    //ready for next command
            fflush(stdout);

            if((bytesread = read(0,buf,100000)) == -1) 
                printf("Reading Failed because %s",strerror(errno));
            
            buf[bytesread] = '\0';        

            //Only update array for new command
            history_update(history,&commands,copy);   
        }
        else 
            strcpy(buf,copy);           //don't read value, use copy of wanted command                   
    }

    free(buf);                             //Deallocate memory
    free(copy);
    dealloc(info);
    dealloc_arg(prev,token_num);
    for(int i = 0; i < commands; i++) 
        free(history[i]); 

    shm_unlink("shared");
    return(exitcode);
}
