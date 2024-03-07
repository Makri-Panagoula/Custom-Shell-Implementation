#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glob.h>
#include <errno.h>
#include <unistd.h>
#include "../headers/alias.h"

#pragma once

#define ARGUMENT_NUMBER 1                //number of arguments we are starting with

//Parses given command in arguments in case there are no white spaces,handles special symbols and returns information regarding it.Deallocates previous content of arg when it is not NULL 
char** parse(char** prev,char* token,int* num,int* wait,int* processes);

//Updates history array with copy string
void history_update(char** history ,int* num,char* copy) ;

//Deallocate memory for arg array
void dealloc_arg(char** arg,int num);

//Check whether we have a non - standard executable , a special command and if so handle the way it should
int special_command(char** arg,int num,alias_info info,int* archive,int commands,char** history,int* loop, int* exitcode,char** copy,int wait,int processes) ;
