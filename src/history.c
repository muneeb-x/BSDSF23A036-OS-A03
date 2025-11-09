#include "shell.h"

// Global history variables
static char* history[HISTORY_SIZE];
static int history_count = 0;
static int history_next = 0;

// Initialize history
void init_history() {
    for (int i = 0; i < HISTORY_SIZE; i++) {
        history[i] = NULL;
    }
    history_count = 0;
    history_next = 0;
}

// Add command to history
void add_to_history(char* command) {
    // Skip empty commands
    if (command == NULL || command[0] == '\0' || command[0] == '\n') {
        return;
    }
    
    // Free old entry if we're overwriting
    if (history[history_next] != NULL) {
        free(history[history_next]);
    }
    
    // Allocate and copy new command
    history[history_next] = malloc(strlen(command) + 1);
    strcpy(history[history_next], command);
    
    // Update indices
    history_next = (history_next + 1) % HISTORY_SIZE;
    if (history_count < HISTORY_SIZE) {
        history_count++;
    }
}

// Display command history
void builtin_history() {
    int start_index;
    int display_count = history_count;
    
    if (history_count == HISTORY_SIZE) {
        // History is full, start from oldest entry
        start_index = history_next;
    } else {
        // History not full, start from beginning
        start_index = 0;
    }
    
    for (int i = 0; i < display_count; i++) {
        int actual_index = (start_index + i) % HISTORY_SIZE;
        printf("%d %s\n", i + 1, history[actual_index]);
    }
}
