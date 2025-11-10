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
