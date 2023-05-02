#include "alias.h"

//Creates an alias array with capacity size 
alias_info init(int capacity) {

    alias_info info = (alias_info) malloc(sizeof (info_struct));
    info->aliases = (alias*) malloc(capacity * sizeof(alias));
    for(int i =0 ; i < capacity; i++)
        info->aliases[i] = NULL ;
    info->capacity = capacity;
    info->num = 0;
    return info;
}

//Allocates memory and initializes fields in aliases[num], s1 for nickname,s2 for actual commands' parsed strings and args is the number of these strings
void create_alias(char* s1,char** s2,alias_info info,int args,int wait,int processes) {

    //Allocate memory
    info->aliases[info->num] = (alias) malloc(sizeof(alias_struct));
    info->aliases[info->num]->tokens = args;
    info->aliases[info->num]->wait = wait;
    info->aliases[info->num]->processes = processes;


    info->aliases[info->num]->nickname = (char*) malloc(strlen(s1) + 1);
    strcpy(info->aliases[info->num]->nickname,s1);

    //Allocate memory and copy strings for actual array
    char** actual = (char**) malloc(args * sizeof(char*));

    for(int i = 0; i < args ; i++) {
        actual[i] = (char*) malloc(strlen(s2[i]) + 1);
        strcpy(actual[i],s2[i]);
    }

    actual[args] = NULL;            //Ending pointer for exec "pact"
    info->aliases[info->num]->actual = actual;
    info->num ++ ;

    if( info->num == info->capacity) {

        info->capacity *= 2;
        info->aliases = realloc(info->aliases,info->capacity * sizeof(alias));
        for(int i = info->capacity / 2; i < info->capacity; i++)                    //Set new elements NULL as they don't contain any information
            info->aliases[i] = NULL;
    }    
}

//Returns -1 if the wanted can't be found in aliases' nicknames otherwise alias index in the array,returns info about the command if pointers given are not NULL
int find_alias(alias_info info,char* wanted,int* tokens,int* wait,int* processes) {

    //Simply traversing given array checking if any of the nicknames matches given,skipping empty cells aka NULL values in nicknames
    for(int i = 0 ; i < info->capacity; i++) {
        if(info->aliases[i] != NULL && strcmp(info->aliases[i]->nickname,wanted) == 0) {

            if(tokens == NULL)
                return i;

            *wait = info->aliases[i]->wait;
            *processes = info->aliases[i]->processes;
            *tokens = info->aliases[i]->tokens;  
            return i;      
        }    
    }
    return -1;
}


//In case wanted exists in aliases array, destroys it and deals with memory occupation(sets its place in the array equal to NULL)
void destroy_alias(alias_info info,char* wanted) {
    
    int pos = find_alias(info,wanted,NULL,NULL,NULL);
    //Take care of memory so that there won't be any leaks

    if( pos != -1 ) {

        char ** actual = info->aliases[pos]->actual;
        for(int i = 0; i < info->aliases[pos]->tokens; i++)
            free(actual[i]);

        free(actual);
        free(info->aliases[pos]->nickname);
        free(info->aliases[pos]);
        info->aliases[pos] = NULL;
    }
}

//Deallocates memory for the alias info struct and the pointer itself
void dealloc(alias_info info) {

    for(int i = 0; i < info->capacity; i++) {

        if(info->aliases[i] != NULL) {

            char ** actual = info->aliases[i]->actual;

            for(int j = 0; j < info->aliases[i]->tokens; j++)
                free(actual[j]);

            free(actual);
            free(info->aliases[i]->nickname);
            free(info->aliases[i]);            
        }
    }
    free(info->aliases);
    free(info);
}


