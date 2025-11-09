#include "shell.h"

// External declaration
void init_history();
void add_to_history(char* command);
int execute_bang_command(char* cmdline);

int main() {
    char* cmdline;
    char** arglist;
    
    // NEW: Initialize history
    init_history();

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {
        // NEW: Handle !n commands before tokenization
        if (cmdline[0] == '!') {
            if (execute_bang_command(cmdline)) {
                free(cmdline);
                continue; // Command was handled, skip to next iteration
            }
        }
        
        // NEW: Add command to history (before tokenization)
        add_to_history(cmdline);
        
        if ((arglist = tokenize(cmdline)) != NULL) {
            // Check if it's a built-in command first
            if (handle_builtin(arglist) == 0) {
                // Not a built-in, execute as external command
                execute(arglist);
            }

            // Free the memory allocated by tokenize()
            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }
            free(arglist);
        }
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
