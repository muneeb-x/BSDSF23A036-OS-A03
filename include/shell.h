#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_LEN 1024
#define MAXARGS 20
#define ARGLEN 64
#define PROMPT "myshell> "
#define HISTORY_SIZE 20  // NEW: History size constant

// Function declarations
char* read_cmd(char* prompt, FILE* fp);
char** tokenize(char* cmdline);
int execute(char* arglist[]);
int handle_builtin(char** arglist);

// Built-in command functions
void builtin_exit();
void builtin_cd(char** args);
void builtin_help();
void builtin_jobs();

// NEW: History functions
void init_history();
void add_to_history(char* command);
void builtin_history();
int execute_bang_command(char* cmdline);

#endif
