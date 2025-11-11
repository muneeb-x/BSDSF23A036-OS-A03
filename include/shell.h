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
#define MAX_JOBS 20
#define MAX_BLOCK_LINES 50
#define MAX_VARIABLES 100  // NEW: Maximum variables

// NEW: Variable structure
typedef struct variable {
    char* name;
    char* value;
    struct variable* next;
} variable_t;

// If-then-else block structure
typedef struct {
    char* condition;
    char* then_commands[MAX_BLOCK_LINES];
    char* else_commands[MAX_BLOCK_LINES];
    int then_count;
    int else_count;
    int has_else;
} if_block_t;

// Command structure for redirection and pipes
typedef struct {
    char* args[MAXARGS];
    char* input_file;
    char* output_file;
    int pipe_output;
    int background;
} command_t;

// Job structure for background processes
typedef struct {
    pid_t pid;
    char* command;
    int job_id;
    int status;
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
void builtin_jobs();

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

// Job control functions
void init_jobs();
int add_job(pid_t pid, char* command);
void remove_job(pid_t pid);
void update_jobs();
void print_jobs();
void reap_zombies();

// Command chaining functions
command_t** parse_command_chain(char* cmdline, int* num_commands);
void free_command_chain(command_t** commands, int num_commands);

// If-then-else functions
if_block_t* parse_if_block();
void free_if_block(if_block_t* block);
int execute_if_block(if_block_t* block);
int is_control_keyword(char* line);

// NEW: Variable functions
void init_variables();
void set_variable(char* name, char* value);
char* get_variable(char* name);
void builtin_set();
int is_assignment(char* token);
void handle_assignment(char* assignment);
void expand_variables(char*** arglist);

#endif
