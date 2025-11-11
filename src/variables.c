#include "shell.h"
#include <string.h>

static variable_t* variable_list = NULL;
static int variable_count = 0;

// Initialize variable system
void init_variables() {
    variable_list = NULL;
    variable_count = 0;
    
    // Set some default environment variables
    set_variable("PATH", getenv("PATH"));
    set_variable("HOME", getenv("HOME"));
    set_variable("USER", getenv("USER"));
}

// Set a variable
void set_variable(char* name, char* value) {
    if (name == NULL || value == NULL) return;
    
    // Check if variable already exists
    variable_t* current = variable_list;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // Update existing variable
            free(current->value);
            current->value = strdup(value);
            return;
        }
        current = current->next;
    }
    
    // Create new variable
    if (variable_count >= MAX_VARIABLES) {
        printf("Variable limit reached\n");
        return;
    }
    
    variable_t* new_var = malloc(sizeof(variable_t));
    new_var->name = strdup(name);
    new_var->value = strdup(value);
    new_var->next = variable_list;
    variable_list = new_var;
    variable_count++;
}

// Get a variable value
char* get_variable(char* name) {
    if (name == NULL) return NULL;
    
    variable_t* current = variable_list;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    // Not found in our variables, check environment
    return getenv(name);
}

// Check if a token is an assignment (contains = with no spaces around it)
int is_assignment(char* token) {
    if (token == NULL) return 0;
    
    char* equals = strchr(token, '=');
    if (equals == NULL) return 0;
    
    // Check that = is not at start or end, and no spaces around =
    if (equals == token || *(equals + 1) == '\0') {
        return 0;
    }
    
    // Check for spaces around the =
    if (equals > token && *(equals - 1) == ' ') return 0;
    if (*(equals + 1) == ' ') return 0;
    
    return 1;
}

// Handle variable assignment
void handle_assignment(char* assignment) {
    if (assignment == NULL) return;
    
    char* equals = strchr(assignment, '=');
    if (equals == NULL) return;
    
    // Split into name and value
    *equals = '\0';
    char* name = assignment;
    char* value = equals + 1;
    
    // Remove quotes if present
    if (value[0] == '"' && value[strlen(value)-1] == '"') {
        value[strlen(value)-1] = '\0';
        value++;
    }
    
    set_variable(name, value);
}

// Expand variables in argument list
void expand_variables(char*** arglist_ptr) {
    if (arglist_ptr == NULL || *arglist_ptr == NULL) return;
    
    char** arglist = *arglist_ptr;
    
    for (int i = 0; arglist[i] != NULL; i++) {
        char* arg = arglist[i];
        
        // Check if argument starts with $ (variable expansion)
        if (arg[0] == '$') {
            char* var_name = arg + 1; // Skip the $
            
            // Handle ${VAR} syntax
            if (var_name[0] == '{') {
                char* end = strchr(var_name, '}');
                if (end != NULL) {
                    *end = '\0';
                    var_name++; // Skip the {
                }
            }
            
            // Get variable value
            char* value = get_variable(var_name);
            if (value != NULL) {
                // Replace the argument with the variable value
                free(arglist[i]);
                arglist[i] = strdup(value);
            } else {
                // Variable not found, replace with empty string
                free(arglist[i]);
                arglist[i] = strdup("");
            }
        }
    }
}

// Set command to display all variables
void builtin_set() {
    variable_t* current = variable_list;
    int count = 0;
    
    printf("Shell variables:\n");
    while (current != NULL) {
        printf("  %s=%s\n", current->name, current->value);
        current = current->next;
        count++;
    }
    
    printf("Environment variables:\n");
    extern char** environ;
    for (char** env = environ; *env != NULL; env++) {
        printf("  %s\n", *env);
    }
    
    printf("Total: %d shell variables\n", count);
}
