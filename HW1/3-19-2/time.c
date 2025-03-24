#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

void execute_command_pipe(char *command[]) {
    struct timeval end_time, start_time;
    pid_t pid;
    int status;
    int pipe_fd[2];
    
    if (pipe(pipe_fd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        gettimeofday(&start_time, NULL);
        close(pipe_fd[0]);
        write(pipe_fd[1], &start_time, sizeof(struct timeval));
        close(pipe_fd[1]);
        execvp(command[0], command);
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, &status, 0);
        gettimeofday(&end_time, NULL);
        close(pipe_fd[1]);
        read(pipe_fd[0], &start_time, sizeof(struct timeval));
        close(pipe_fd[0]);

        double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                              (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        printf("[PIPE] Elapsed time: %.6f seconds\n", elapsed_time);
    }
}


int main(int argc, char *argv[]) {
    execute_command_pipe(&argv[1]);
    return 0;
}