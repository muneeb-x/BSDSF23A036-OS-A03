#include "shell.h"

static job_t jobs[MAX_JOBS];
static int job_count = 0;
static int next_job_id = 1;

// Initialize job list
void init_jobs() {
    for (int i = 0; i < MAX_JOBS; i++) {
        jobs[i].pid = -1;
        jobs[i].command = NULL;
        jobs[i].job_id = 0;
        jobs[i].status = 0;
    }
    job_count = 0;
}

// Add a new background job
int add_job(pid_t pid, char* command) {
    if (job_count >= MAX_JOBS) {
        return -1; // No space
    }
    
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid == -1) {
            jobs[i].pid = pid;
            jobs[i].command = strdup(command);
            jobs[i].job_id = next_job_id++;
            jobs[i].status = 0; // running
            job_count++;
            return jobs[i].job_id;
        }
    }
    return -1;
}

// Remove a completed job
void remove_job(pid_t pid) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid == pid) {
            free(jobs[i].command);
            jobs[i].pid = -1;
            jobs[i].command = NULL;
            jobs[i].job_id = 0;
            jobs[i].status = 0;
            job_count--;
            break;
        }
    }
}

// Update job status and remove completed ones
void update_jobs() {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid != -1) {
            int status;
            pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);
            if (result > 0) {
                // Job completed
                jobs[i].status = 1;
                printf("[%d] Done\t%s\n", jobs[i].job_id, jobs[i].command);
                remove_job(jobs[i].pid);
            }
        }
    }
}

// Print all active jobs
void print_jobs() {
    int active_jobs = 0;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid != -1 && jobs[i].status == 0) {
            printf("[%d] Running\t%s\n", jobs[i].job_id, jobs[i].command);
            active_jobs++;
        }
    }
    if (active_jobs == 0) {
        printf("No background jobs running.\n");
    }
}

// Reap zombie processes
void reap_zombies() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        remove_job(pid);
    }
}
