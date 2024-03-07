#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define DEFAULT_ALIASES 1

#pragma once

typedef struct{

    char* nickname;
    //The actual command nickname corresponds
    char** actual;
    int wait ;          //We save the info about the command since we won't reprocess it
    int processes;
    int tokens;
}alias_struct, *alias;

typedef struct{
    //Upper limit for aliases array items
    int capacity ;
    //Index of first "free" position in aliases array
    int num ;
    alias* aliases ;
} info_struct, *alias_info;

//Creates an alias array with capacity size 
alias_info init(int capacity) ;

//Allocates memory and initializes fields in aliases[num], s1 for nickname,s2 for actual commands' parsed strings and args is the number of these strings
void create_alias(char* s1,char** s2,alias_info info,int args,int wait,int processes) ;

//Returns -1 if the wanted can't be found in aliases' nicknames otherwise alias index in the array,returns info about the command if pointers given are not NULL
int find_alias(alias_info info,char* wanted,int* tokens,int* wait,int* processes) ;

//In case wanted exists in aliases array, destroys it and deals with memory occupation(sets its place in the array equal to NULL)
void destroy_alias(alias_info info,char* wanted) ;

//Deallocates memory for the alias info struct and the pointer itself
void dealloc(alias_info info) ;
