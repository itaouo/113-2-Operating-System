#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MIN_PID 300
#define MAX_PID 5000

int pid_map[MAX_PID - MIN_PID + 1];
pthread_mutex_t mutex;

int allocate_map(void) {
    for (int i = 0; i <= MAX_PID - MIN_PID; i++) {
        pid_map[i] = 0;
    }
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex init failed\n");
        return -1;
    }
    return 1;
}

int allocate_pid(void) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i <= MAX_PID - MIN_PID; i++) {
        if (pid_map[i] == 0) {
            pid_map[i] = 1;
            pthread_mutex_unlock(&mutex);
            return i + MIN_PID;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

void release_pid(int pid) {
    pthread_mutex_lock(&mutex);
    pid_map[pid - MIN_PID] = 0;
    pthread_mutex_unlock(&mutex);
}

void *thread_function(void *param) {
    int pid = allocate_pid();
    if (pid == -1) {
        printf("No PID available\n");
        pthread_exit(0);
    }

    printf("Thread %ld allocated PID %d\n", pthread_self(), pid);
    sleep(rand() % 5 + 1);
    release_pid(pid);
    printf("Thread %ld released PID %d\n", pthread_self(), pid);

    pthread_exit(0);
    return NULL;
}

int main() {
    int num_threads = 100;
    pthread_t threads[num_threads];

    allocate_map();
    srand(time(NULL));

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
