#include "../headers/commands.h"

//Inserts p string in arguments array and properly updates its capacity and number
char** insert(char** arg,int *num ,int *capacity,char* p , int* wildcard, glob_t* globbuf,int* globs) {

    //Make sure p is in wanted format,otherwise convert it
    if(p[0] == '\0')
        return arg;
    
    if(p[0] == '"')         //if string is in quotations marks(aliases case) remove them
        p++;
    
    if(p[strlen(p) - 1] == '"')
        p[strlen(p) - 1] = '\0';

    if (p[0] == '$') 
        p = getenv(p + 1) ;    //Get value of wanted variable (ignore $ character)

    if( *wildcard ) {               //If we need to perform a wildcard expansion

        int result;

        if(*globs == 0)  {      //If it is first call of glob we should call with DOOFFS,otherwise append

            globbuf->gl_offs = 0;                                         //We only use glob to get the paths so we don't need to save anything :)
            result = glob(p, GLOB_DOOFFS, NULL, globbuf);
            *globs = *globs + 1;
        }
        else
            result = glob(p, GLOB_APPEND, NULL, globbuf);

        if( result != 0)
            printf("Failed because \n%s\n",strerror(errno));
        
        //Insert all the strings it expands to in arguments array(essentially what glob does as well but that way we also save next commands p.ex. redirection)
        for (int i = 0 ; i < globbuf->gl_pathc; i++) {          

           char* expanded = globbuf->gl_pathv[i]; 
            arg[*num] = malloc((strlen(expanded) + 1));
            strcpy(arg[*num],expanded);       

            //Update array info
            if(++ (*num) == *capacity) {         //dynamically increase array space

                *capacity = *capacity * 2 ;
                arg = realloc(arg,*capacity * sizeof(char*));
            }  
        }
        *wildcard = 0;              //Restore wildcard value for next expansion if needed
    }
    else {                                                 //If there is no need for expansion,just insert current string
        arg[*num] = malloc((strlen(p) + 1));
        strcpy(arg[*num],p);       

        //Update array info
        if(++ (*num) == *capacity) {         //dynamically increase array space

            *capacity = *capacity * 2 ;
            arg = realloc(arg,*capacity * sizeof(char*));
        }  
    }
    return arg;
}

//Parses given command in arguments in case there are no white spaces,handles special symbols and returns information regarding it.Deallocates previous content of arg when it is not NULL   
char** parse(char** prev , char* token,int* num,int* wait,int* processes)   { 

    char *end_token;
    int capacity = ARGUMENT_NUMBER;
    //Boolean variable to determine whether the character should be expanded
    int wildcard = 0;
    glob_t globbuf;
    int globs = 0;

    //Deallocate memory for previous array
    if (prev != NULL)
        dealloc_arg(prev,*num);

    char** arg = malloc(capacity * sizeof(char*));               //initialize arguments array for each command
    *num = 0;
    
    for (char *p = strtok_r(token, " ", &end_token); p != NULL; p = strtok_r(NULL, " ", &end_token) )  {
        
        int i = 0;

        //Traverse the string looking for special characters to seperate the arguments and insert them in array
        while( p[i] != '\0') {
            
            char c = p[i];

            //P is remaining unprocessed string that hasn't been saved in arguments array

            if  ( c == '|' || c == '>' || c == '<' ) {

                //Exactly before the redirection character we have the executable's name so save it as a seperate argument in array
                p[i] = '\0';                                        
                arg = insert(arg,num,&capacity,p,&wildcard,&globbuf,&globs);

                //p is gonna be the remaining unprocessed string => update iterator as well
                p = p + i + 1;
                i = 0;

                if(p[i] == '>') {

                    arg = insert(arg,num,&capacity,">>",&wildcard,&globbuf,&globs);
                    p++;
                }
                else {
                    char str[2] = {c , '\0'};               //Save redirection characters in arguments array
                    arg = insert(arg,num,&capacity,str,&wildcard,&globbuf,&globs);
                } 
                if( c == '|') 
                    *processes = *processes + 1;
            }
            else if (c == '&') {  

                p[i] = '\0';            //make sure you don't save & as part of the actual command => "eat" that character
                arg = insert(arg,num,&capacity,p,&wildcard,&globbuf,&globs);
                *wait = 0;                      //background process => parent shouldn't wait
                p = p + i +1;                   //don't save symbol
            }
            else if ((c == '*' || c == '?') && p[i-1] != '/') {             //Check for wild characters and take them into consideration as lons as there isn't an escape character before
                wildcard = 1 ;   
                i++;                                                        //Read next character
            }
            else                    //Move one to next character since we don't have a delimiter
                i++;
        }

        arg = insert(arg,num,&capacity,p,&wildcard,&globbuf,&globs);      //Insert last argument as well
    }

    if(globs)                 //Check if there has been any call of globs and if so restore its value for next iteration and free struct
        globfree(&globbuf);
    
    return arg;
}

//Updates history array with copy string
void history_update(char** history ,int* num,char* copy) {

    int commands = *num;

    if(commands < 20) {                 //Save first commands

        history[commands] = (char*) malloc(strlen(copy) + 1);
        strcpy(history[commands++],copy);
    }
    else {
        //New command => keep the order =>shift left to make room for new element => oldest (first) element is removed
        for(int i = 0; i < 19 ; i++) {

            history[i] = realloc(history[i],strlen(history[i+1]) + 1);
            strcpy(history[i],history[i+1]);
        }

        //Last element is the latest
        history[19] = realloc(history[19],strlen(copy) + 1);
        strcpy(history[19],copy);
    } 

    *num = commands; 
}

//Deallocate memory for arg array
void dealloc_arg(char** arg,int token_num) {
    
    for(int i = 0; i < token_num ; i++) 
        free(arg[i]);
    free(arg);
}

int special_command(char** arg,int num,alias_info info,int* archive,int commands,char** history,int* loop, int* exitcode,char** copy,int wait,int processes) {

    if( ! strcmp("createalias",arg[0]) && num > 2) {

        create_alias(arg[1],arg + 2,info,num - 2,wait,processes);
        return 1;
    }
    else if( ! strcmp("destroyalias",arg[0]) && num == 2  ) {

        destroy_alias(info,arg[1]);
        return 1;
    }
    else if( ! strcmp("myHistory",arg[0])) {

        int line;

        if( num == 1 ) {                      //When we don't have any tokens we print all the available command lines in history
            
            for(int i = 0; i < commands; i++)
                printf("\n%s",history[i]);
        }
        else if (((line = atoi(arg[1])) <= 20 || (commands < 20 && line <= commands)) && line >= 1){                 //Check that line is in acceptable range

            *archive = 1;                        //Don't read next line , execute wanted from archive
            strcpy(*copy,history[line - 1]);     //Keep copy of wanted line to pass instead of reading input 
        }
        return 1;

    }  
    else if( ! strcmp("exit",arg[0]) ) {          //Exit is the spesific UNIX command to escape shell with given exit code (if given)

        *loop = 0;                                   //Stop the loop
        if(num == 2)
            *exitcode = atoi(arg[1]);               //Return given exitcode
        
        return 1;
    }     
    if( ! strcmp("cd",arg[0])) {

        if(chdir(arg[1]) == -1)               //According to instructions we will have a path and not an open file descriptor
            printf("Failed to execute chdir because %s\n", strerror(errno));
        
        return 1;
    }            
    return 0;
}
