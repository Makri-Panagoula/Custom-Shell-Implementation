#include <stdio.h>
#include <errno.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include "../headers/commands.h"

#pragma once

#define READ 0
#define WRITE 1

//Redirects output aka handles '>' symbol
void redirect_output(const char* file) ;

//Redirects output only when given file already exists aka handles '>>' symbol
void redirect_output_existent(const char* file) ;

//Redirects input aka handles '<' symbol
void redirect_input(const char* file) ; 

//Deals  with |
void create_pipe(char** arg , int it , int wait ,pid_t* leader) ;

//Update arguments array , iterator , tokens number after processing pipe command
char** after_pipe(char** arg,int* cur,int* num) ;



