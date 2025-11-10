#include "shell.h"
#include <fcntl.h>

// External declaration for job functions
extern int add_job(pid_t pid, char* command);

// Execute command with I/O redirection
int execute_redirected(command_t* cmd) {
    if (cmd == NULL || cmd->args[0] == NULL) {
        return -1;
    }

    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {
        // Child process
        
        // Input redirection
        if (cmd->input_file) {
            int fd_in = open(cmd->input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("open input file");
                exit(1);
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                perror("dup2 stdin");
                exit(1);
            }
            close(fd_in);
        }

        // Output redirection
        if (cmd->output_file) {
            int fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                perror("open output file");
                exit(1);
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2 stdout");
                exit(1);
            }
            close(fd_out);
        }

        // Execute the command
        execvp(cmd->args[0], cmd->args);
        perror("execvp failed");
        exit(1);
    } else {
        // Parent process
        if (cmd->background) {
            // NEW: Background execution - don't wait, add to job list
            char command_str[MAX_LEN] = {0};
            for (int i = 0; cmd->args[i] != NULL && i < MAXARGS - 1; i++) {
                if (i > 0) strcat(command_str, " ");
                strcat(command_str, cmd->args[i]);
            }
            int job_id = add_job(pid, command_str);
            printf("[%d] %d\n", job_id, pid);
            return 0;
        } else {
            // Foreground execution - wait for completion
            int status;
            waitpid(pid, &status, 0);
            return status;
        }
    }
}
