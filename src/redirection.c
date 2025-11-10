#include "shell.h"
#include <fcntl.h>

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
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
}
