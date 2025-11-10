#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>  // NEW: For file operations

// Readline headers
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LEN 1024
#define MAXARGS 20
#define ARGLEN 64
#define PROMPT "myshell> "
#define HISTORY_SIZE 20
#define MAX_COMMANDS 10  // NEW: For command chaining

// NEW: Command structure for redirection and pipes
typedef struct {
    char* args[MAXARGS];
    char* input_file;
    char* output_file;
    int pipe_output;
} command_t;

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

// History functions
void init_history();
void add_to_history(char* command);
void builtin_history();
int execute_bang_command(char* cmdline);

// NEW: Redirection and pipe functions
command_t* parse_command(char* cmdline);
void free_command(command_t* cmd);
int execute_redirected(command_t* cmd);
int execute_pipeline(command_t** commands, int num_commands);

#endif
