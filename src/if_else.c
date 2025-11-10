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
