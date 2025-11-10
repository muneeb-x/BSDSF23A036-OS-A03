#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

// Readline headers
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LEN 1024
#define MAXARGS 20
#define ARGLEN 64
#define PROMPT "myshell> "
#define HISTORY_SIZE 20
#define MAX_COMMANDS 10
#define MAX_JOBS 20  // NEW: Maximum background jobs

// Command structure for redirection and pipes
typedef struct {
    char* args[MAXARGS];
    char* input_file;
    char* output_file;
    int pipe_output;
    int background;  // NEW: Background execution flag
} command_t;

// NEW: Job structure for background processes
typedef struct {
    pid_t pid;
    char* command;
    int job_id;
    int status; // 0=running, 1=completed
} job_t;

// Function declarations
char* read_cmd(char* prompt, FILE* fp);
char** tokenize(char* cmdline);
int execute(char* arglist[]);
int handle_builtin(char** arglist);

// Built-in command functions
void builtin_exit();
void builtin_cd(char** args);
void builtin_help();
void builtin_jobs();  // Will be updated

// History functions
void init_history();
void add_to_history(char* command);
void builtin_history();
int execute_bang_command(char* cmdline);

// Redirection and pipe functions
command_t* parse_command(char* cmdline);
void free_command(command_t* cmd);
int execute_redirected(command_t* cmd);
int execute_pipeline(command_t** commands, int num_commands);

// NEW: Job control functions
void init_jobs();
int add_job(pid_t pid, char* command);
void remove_job(pid_t pid);
void update_jobs();
void print_jobs();
void reap_zombies();

// NEW: Command chaining functions
command_t** parse_command_chain(char* cmdline, int* num_commands);
void free_command_chain(command_t** commands, int num_commands);

#endif
