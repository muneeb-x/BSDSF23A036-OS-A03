#include "shell.h"
#include <stdio.h>

// External declarations
void builtin_history();
void print_jobs();
void builtin_set();  // NEW

// Exit the shell
void builtin_exit() {
    printf("Shell exiting...\n");
    exit(0);
}

// Change directory
void builtin_cd(char** args) {
    if (args[1] == NULL) {
        // No argument - go to home directory
        char* home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME environment variable not set\n");
            return;
        }
        if (chdir(home) != 0) {
            perror("cd");
        }
    } else {
        // Change to specified directory
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

// Display help message
void builtin_help() {
    printf("Built-in commands:\n");
    printf("  cd <directory>    - Change current directory\n");
    printf("  exit              - Exit the shell\n");
    printf("  help              - Display this help message\n");
    printf("  history           - Display command history\n");
    printf("  jobs              - Display background jobs\n");
    printf("  set               - Display all variables\n");  // NEW
}

// Jobs command
void builtin_jobs() {
    print_jobs();
}

// Handle built-in commands
int handle_builtin(char** arglist) {
    if (arglist[0] == NULL) {
        return 0; // No command
    }

    if (strcmp(arglist[0], "exit") == 0) {
        builtin_exit();
        return 1;
    }
    else if (strcmp(arglist[0], "cd") == 0) {
        builtin_cd(arglist);
        return 1;
    }
    else if (strcmp(arglist[0], "help") == 0) {
        builtin_help();
        return 1;
    }
    else if (strcmp(arglist[0], "history") == 0) {
        builtin_history();
        return 1;
    }
    else if (strcmp(arglist[0], "jobs") == 0) {
        builtin_jobs();
        return 1;
    }
    else if (strcmp(arglist[0], "set") == 0) {  // NEW
        builtin_set();
        return 1;
    }

    return 0; // Not a built-in command
}
