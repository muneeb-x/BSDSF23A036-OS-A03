#include "shell.h"

// External declarations
void init_history();
void add_to_history(char* command);
int execute_bang_command(char* cmdline);
command_t* parse_command(char* cmdline);
void free_command(command_t* cmd);
int execute_redirected(command_t* cmd);
int execute_pipeline(command_t** commands, int num_commands);
void init_jobs();
void update_jobs();
void reap_zombies();
command_t** parse_command_chain(char* cmdline, int* num_commands);
void free_command_chain(command_t** commands, int num_commands);
if_block_t* parse_if_block();
void free_if_block(if_block_t* block);
int execute_if_block(if_block_t* block);
int is_control_keyword(char* line);
void init_variables();        // NEW
void expand_variables(char*** arglist);  // NEW

// Helper function to convert command_t to arglist for builtins
char** command_to_arglist(command_t* cmd) {
    if (cmd == NULL) return NULL;
    
    char** arglist = malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS; i++) {
        if (cmd->args[i] != NULL) {
            arglist[i] = strdup(cmd->args[i]);
        } else {
            arglist[i] = NULL;
            break;
        }
    }
    return arglist;
}

// Execute a single command (with possible background/redirection)
void execute_single_command(command_t* cmd) {
    if (cmd == NULL) return;
    
    char** arglist = command_to_arglist(cmd);
    if (arglist != NULL) {
        // NEW: Expand variables before execution
        expand_variables(&arglist);
        
        if (handle_builtin(arglist) == 0) {
            // Not a built-in, check for redirection/background
            if (cmd->input_file || cmd->output_file || cmd->background) {
                execute_redirected(cmd);
            } else if (cmd->pipe_output) {
                // Handle pipeline - simplified message
                printf("Note: Multi-command pipelines need full command line parsing\n");
            } else {
                // Regular command execution
                execute(arglist);
            }
        }
        
        // Free arglist
        for (int i = 0; arglist[i] != NULL; i++) {
            free(arglist[i]);
        }
        free(arglist);
    }
}

int main() {
    char* cmdline;
    
    // Initialize history, jobs, and variables
    init_history();
    init_jobs();
    init_variables();  // NEW

    while (1) {
        // Reap zombie processes before prompt
        reap_zombies();
        update_jobs();
        
        cmdline = readline(PROMPT);
        
        // Check for EOF (Ctrl+D)
        if (cmdline == NULL) {
            printf("\n");
            break;
        }
        
        // Skip empty lines
        if (cmdline[0] == '\0') {
            free(cmdline);
            continue;
        }

        // Check for if statement
        if (is_control_keyword(cmdline) && strncmp(cmdline, "if", 2) == 0) {
            // Handle if-then-else block
            if_block_t* block = parse_if_block();
            if (block != NULL) {
                execute_if_block(block);
                free_if_block(block);
            }
            free(cmdline);
            continue;
        }

        // Handle !n commands
        if (cmdline[0] == '!') {
            if (execute_bang_command(cmdline)) {
                free(cmdline);
                continue;
            }
        }
        
        // Add to readline history and custom history
        add_history(cmdline);
        add_to_history(cmdline);
        
        // Parse command chain with semicolons
        int num_commands;
        command_t** commands = parse_command_chain(cmdline, &num_commands);
        
        if (commands != NULL && num_commands > 0) {
            // Execute each command in sequence
            for (int i = 0; i < num_commands; i++) {
                execute_single_command(commands[i]);
            }
            free_command_chain(commands, num_commands);
        }
        
        free(cmdline);
    }

    printf("Shell exited.\n");
    return 0;
}
