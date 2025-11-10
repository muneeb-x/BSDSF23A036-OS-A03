#include "shell.h"

// External declarations
void init_history();
void add_to_history(char* command);
int execute_bang_command(char* cmdline);
command_t* parse_command(char* cmdline);  // NEW
void free_command(command_t* cmd);        // NEW
int execute_redirected(command_t* cmd);   // NEW
int execute_pipeline(command_t** commands, int num_commands);  // NEW

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

int main() {
    char* cmdline;
    
    // Initialize history
    init_history();

    while (1) {
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
        
        // NEW: Parse command with redirection and pipe support
        command_t* cmd = parse_command(cmdline);
        if (cmd != NULL) {
            if (cmd->pipe_output) {
                // Handle pipeline - for now, single pipe
                command_t* commands[2];
                commands[0] = cmd;
                
                // Parse the second command after pipe
                char* pipe_pos = strchr(cmdline, '|');
                if (pipe_pos) {
                    command_t* cmd2 = parse_command(pipe_pos + 1);
                    if (cmd2) {
                        commands[1] = cmd2;
                        execute_pipeline(commands, 2);
                        free_command(cmd2);
                    }
                }
            } else {
                // Handle single command with possible redirection
                char** arglist = command_to_arglist(cmd);
                if (arglist != NULL) {
                    if (handle_builtin(arglist) == 0) {
                        // Not a built-in, check for redirection
                        if (cmd->input_file || cmd->output_file) {
                            execute_redirected(cmd);
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
            
            free_command(cmd);
        }
        
        free(cmdline);
    }

    printf("Shell exited.\n");
    return 0;
}
