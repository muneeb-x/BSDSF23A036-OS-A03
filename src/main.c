#include "shell.h"

// External declaration
void init_history();
void add_to_history(char* command);
int execute_bang_command(char* cmdline);

int main() {
    char* cmdline;
    char** arglist;
    
    // Initialize history
    init_history();

    while (1) {
        // NEW: Replace read_cmd with readline()
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

        // NEW: Handle !n commands before adding to history
        if (cmdline[0] == '!') {
            if (execute_bang_command(cmdline)) {
                free(cmdline);
                continue;
            }
        }
        
        // NEW: Add to readline history (instead of our custom history)
        add_history(cmdline);
        
        // Also add to our custom history for !n command
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

    printf("Shell exited.\n");
    return 0;
}
