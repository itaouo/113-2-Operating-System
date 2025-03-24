#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

void execute_command_shm(char *command[]) {
    struct timeval end_time, start_time;
    pid_t pid;
    int status;
    int shm_fd;
    struct timeval *shm_ptr;

    shm_fd = shm_open("/time_shm", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(struct timeval));
    shm_ptr = mmap(0, sizeof(struct timeval), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    if (shm_ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        gettimeofday(&start_time, NULL);
        memcpy(shm_ptr, &start_time, sizeof(struct timeval));
        execvp(command[0], command);
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, &status, 0);
        gettimeofday(&end_time, NULL);
        memcpy(&start_time, shm_ptr, sizeof(struct timeval));
        shm_unlink("/time_shm");
        
        double elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                              (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        printf("[SHM] Elapsed time: %.6f seconds\n", elapsed_time);
    }
}

int main(int argc, char *argv[]) {
    execute_command_shm(&argv[1]);
    return 0;
}
