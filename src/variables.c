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
