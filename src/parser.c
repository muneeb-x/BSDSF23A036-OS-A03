#include "shell.h"
#include <string.h>

// Parse command line into command structure
command_t* parse_command(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0') {
        return NULL;
    }

    command_t* cmd = malloc(sizeof(command_t));
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->pipe_output = 0;
    
    for (int i = 0; i < MAXARGS; i++) {
        cmd->args[i] = NULL;
    }

    char* token;
    char* rest = cmdline;
    int arg_count = 0;

    while ((token = strtok_r(rest, " \t", &rest))) {
        if (strcmp(token, "<") == 0) {
            // Input redirection
            token = strtok_r(rest, " \t", &rest);
            if (token) {
                cmd->input_file = strdup(token);
            }
        } else if (strcmp(token, ">") == 0) {
            // Output redirection
            token = strtok_r(rest, " \t", &rest);
            if (token) {
                cmd->output_file = strdup(token);
            }
        } else if (strcmp(token, "|") == 0) {
            // Pipe operator
            cmd->pipe_output = 1;
            break; // Stop parsing for this command
        } else {
            // Regular argument
            if (arg_count < MAXARGS - 1) {
                cmd->args[arg_count++] = strdup(token);
            }
        }
    }

    if (arg_count == 0) {
        free_command(cmd);
        return NULL;
    }

    return cmd;
}

// Free command structure
void free_command(command_t* cmd) {
    if (cmd == NULL) return;
    
    for (int i = 0; i < MAXARGS && cmd->args[i] != NULL; i++) {
        free(cmd->args[i]);
    }
    
    if (cmd->input_file) free(cmd->input_file);
    if (cmd->output_file) free(cmd->output_file);
    free(cmd);
}
