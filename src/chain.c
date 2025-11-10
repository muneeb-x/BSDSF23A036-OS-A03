#include "shell.h"
#include <string.h>

// Parse command line with semicolon chaining
command_t** parse_command_chain(char* cmdline, int* num_commands) {
    if (cmdline == NULL || cmdline[0] == '\0') {
        *num_commands = 0;
        return NULL;
    }

    command_t** commands = malloc(sizeof(command_t*) * MAX_COMMANDS);
    *num_commands = 0;
    
    char* saveptr;
    char* token = strtok_r(cmdline, ";", &saveptr);
    
    while (token != NULL && *num_commands < MAX_COMMANDS) {
        // Trim whitespace
        while (*token == ' ' || *token == '\t') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t' || *end == '\n')) {
            *end = '\0';
            end--;
        }
        
        if (*token != '\0') {
            commands[*num_commands] = parse_command(token);
            if (commands[*num_commands] != NULL) {
                (*num_commands)++;
            }
        }
        
        token = strtok_r(NULL, ";", &saveptr);
    }
    
    return commands;
}

// Free command chain
void free_command_chain(command_t** commands, int num_commands) {
    for (int i = 0; i < num_commands; i++) {
        free_command(commands[i]);
    }
    free(commands);
}
