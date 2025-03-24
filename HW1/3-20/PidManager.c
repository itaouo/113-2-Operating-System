#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN_PID 300
#define MAX_PID 5000
#define PID_RANGE (MAX_PID - MIN_PID + 1)

int *pid_map;

int allocate_map() {
    pid_map = (int *)malloc(PID_RANGE * sizeof(int));
    if (pid_map == NULL) {
        return -1;
    }
    for (int i = 0; i < PID_RANGE; i++) {
        pid_map[i] = 0;
    }
    return 1;
}

int allocate_pid() {
    for (int i = 0; i < PID_RANGE; i++) {
        if (pid_map[i] == 0) {
            pid_map[i] = 1;
            return MIN_PID + i;
        }
    }
    return -1;
}

void release_pid(int pid) {
    if (pid < MIN_PID || pid > MAX_PID) {
        return;
    }
    pid_map[pid - MIN_PID] = 0;
}

void free_map() {
    free(pid_map);
}

int main() {
    if (allocate_map() == -1) {
        printf("Failed to initialize PID manager.\n");
        return 1;
    }
    
    int pid1 = allocate_pid();
    printf("Allocated PID: %d\n", pid1);
    
    int pid2 = allocate_pid();
    printf("Allocated PID: %d\n", pid2);
    
    release_pid(pid1);
    printf("Released PID: %d\n", pid1);
    
    int pid3 = allocate_pid();
    printf("Allocated PID: %d\n", pid3);
    
    free_map();
    return 0;
}
