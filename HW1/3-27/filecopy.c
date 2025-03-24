#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        close(fd[0]);
        
        int source = open(argv[1], O_RDONLY);
        if (source == -1) {
            perror("Failed to open source file");
            exit(EXIT_FAILURE);
        }
        
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(source, buffer, BUFFER_SIZE)) > 0) {
            write(fd[1], buffer, bytesRead);
        }
        
        close(source);
        close(fd[1]);
        wait(NULL);
    } else {
        close(fd[1]);
        
        int destination = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (destination == -1) {
            perror("Failed to open destination file");
            exit(EXIT_FAILURE);
        }
        
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(fd[0], buffer, BUFFER_SIZE)) > 0) {
            write(destination, buffer, bytesRead);
        }
        
        close(destination);
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    return 0;
}
