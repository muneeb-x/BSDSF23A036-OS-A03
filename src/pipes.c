#include "shell.h"
#include <fcntl.h>

// Execute pipeline of commands
int execute_pipeline(command_t** commands, int num_commands) {
    if (num_commands < 1) return -1;

    int pipefds[2];
    int input_fd = STDIN_FILENO;
    pid_t pids[num_commands];

    for (int i = 0; i < num_commands; i++) {
        // Create pipe for all commands except the last one
        if (i < num_commands - 1) {
            if (pipe(pipefds) == -1) {
                perror("pipe");
                return -1;
            }
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return -1;
        }

        if (pid == 0) {
            // Child process
            
            // Redirect input (except first command)
            if (i > 0) {
                if (dup2(input_fd, STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(1);
                }
                close(input_fd);
            }

            // Redirect output (except last command)
            if (i < num_commands - 1) {
                close(pipefds[0]); // Close read end
                if (dup2(pipefds[1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    exit(1);
                }
                close(pipefds[1]);
            }

            // Handle file redirection for this command
            if (commands[i]->input_file) {
                int fd_in = open(commands[i]->input_file, O_RDONLY);
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

            if (commands[i]->output_file) {
                int fd_out = open(commands[i]->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
            execvp(commands[i]->args[0], commands[i]->args);
            perror("execvp failed");
            exit(1);
        } else {
            // Parent process
            pids[i] = pid;
            
            // Close unused file descriptors
            if (i > 0) {
                close(input_fd);
            }
            if (i < num_commands - 1) {
                close(pipefds[1]); // Close write end
                input_fd = pipefds[0]; // Next command reads from this
            }
        }
    }

    // Wait for all child processes
    int status = 0;
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], &status, 0);
    }

    return status;
}
