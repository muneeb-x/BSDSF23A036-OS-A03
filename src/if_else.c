#include "shell.h"
#include <string.h>

// Check if a line is a control keyword
int is_control_keyword(char* line) {
    if (line == NULL) return 0;
    
    // Remove leading whitespace
    while (*line == ' ' || *line == '\t') line++;
    
    if (strncmp(line, "if", 2) == 0 && (line[2] == ' ' || line[2] == '\t' || line[2] == '\0')) {
        return 1;
    }
    if (strncmp(line, "then", 4) == 0 && (line[4] == ' ' || line[4] == '\t' || line[4] == '\0')) {
        return 1;
    }
    if (strncmp(line, "else", 4) == 0 && (line[4] == ' ' || line[4] == '\t' || line[4] == '\0')) {
        return 1;
    }
    if (strncmp(line, "fi", 2) == 0 && (line[2] == ' ' || line[2] == '\t' || line[2] == '\0')) {
        return 1;
    }
    
    return 0;
}

// Free if block memory
void free_if_block(if_block_t* block) {
    if (block == NULL) return;
    
    if (block->condition) free(block->condition);
    
    for (int i = 0; i < block->then_count; i++) {
        if (block->then_commands[i]) free(block->then_commands[i]);
    }
    
    for (int i = 0; i < block->else_count; i++) {
        if (block->else_commands[i]) free(block->else_commands[i]);
    }
    
    free(block);
}

// Parse if-then-else-fi block
if_block_t* parse_if_block() {
    if_block_t* block = malloc(sizeof(if_block_t));
    block->condition = NULL;
    block->then_count = 0;
    block->else_count = 0;
    block->has_else = 0;
    
    // Read the condition line (after 'if')
    char* line = readline("");
    if (line == NULL) {
        free(block);
        return NULL; // EOF
    }
    
    // Store the condition (everything after 'if')
    char* condition_start = line;
    while (*condition_start == ' ' || *condition_start == '\t') condition_start++;
    condition_start += 2; // Skip 'if'
    while (*condition_start == ' ' || *condition_start == '\t') condition_start_start++;
    
    block->condition = strdup(condition_start);
    free(line);
    
    // Look for 'then'
    line = readline("");
    if (line == NULL) {
        free_if_block(block);
        return NULL; // EOF
    }
    
    // Skip 'then' line
    free(line);
    
    // Read then-block commands until 'else' or 'fi'
    int in_then_block = 1;
    int in_else_block = 0;
    
    while (1) {
        line = readline("");
        if (line == NULL) {
            free_if_block(block);
            return NULL; // EOF
        }
        
        // Check for control keywords
        if (is_control_keyword(line)) {
            if (strncmp(line, "else", 4) == 0) {
                in_then_block = 0;
                in_else_block = 1;
                block->has_else = 1;
                free(line);
                continue;
            } else if (strncmp(line, "fi", 2) == 0) {
                free(line);
                break; // End of if block
            }
        }
        
        // Store the command in appropriate block
        if (in_then_block && block->then_count < MAX_BLOCK_LINES) {
            block->then_commands[block->then_count++] = strdup(line);
        } else if (in_else_block && block->else_count < MAX_BLOCK_LINES) {
            block->else_commands[block->else_count++] = strdup(line);
        }
        
        free(line);
    }
    
    return block;
}

// Execute if-then-else block
int execute_if_block(if_block_t* block) {
    if (block == NULL || block->condition == NULL) {
        return -1;
    }
    
    // Execute the condition command
    char** condition_args = tokenize(block->condition);
    if (condition_args == NULL) {
        return -1;
    }
    
    int condition_status = 0;
    
    // Check if it's a built-in command
    if (handle_builtin(condition_args) == 0) {
        // Not a built-in, execute as external command
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            execvp(condition_args[0], condition_args);
            perror("execvp failed");
            exit(1);
        } else if (pid > 0) {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            condition_status = WEXITSTATUS(status);
        } else {
            perror("fork failed");
            condition_status = 1;
        }
    }
    
    // Free condition args
    for (int i = 0; condition_args[i] != NULL; i++) {
        free(condition_args[i]);
    }
    free(condition_args);
    
    // Execute appropriate block based on condition status
    char** commands_to_execute;
    int command_count;
    
    if (condition_status == 0) {
        // Condition succeeded - execute then block
        commands_to_execute = block->then_commands;
        command_count = block->then_count;
    } else if (block->has_else) {
        // Condition failed and else exists - execute else block
        commands_to_execute = block->else_commands;
        command_count = block->else_count;
    } else {
        // Condition failed and no else - do nothing
        return condition_status;
    }
    
    // Execute commands in the block
    for (int i = 0; i < command_count; i++) {
        if (commands_to_execute[i] != NULL) {
            char** args = tokenize(commands_to_execute[i]);
            if (args != NULL) {
                if (handle_builtin(args) == 0) {
                    execute(args);
                }
                
                // Free args
                for (int j = 0; args[j] != NULL; j++) {
                    free(args[j]);
                }
                free(args);
            }
        }
    }
    
    return condition_status;
}
