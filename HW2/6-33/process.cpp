#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define MAX_RESOURCES 5
int available_resources = MAX_RESOURCES;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void my_sleep(int seconds) {
    #ifdef _WIN32
        Sleep(seconds * 1000);
    #else
        sleep(seconds);
    #endif
}

int decrease_count(int count) {
    pthread_mutex_lock(&mutex);

    while (available_resources < count) {
        pthread_cond_wait(&cond, &mutex);
    }
    available_resources -= count;
    printf("Allocated %d resources, %d remaining.\n", count, available_resources);

    pthread_mutex_unlock(&mutex);
    return 0;
}

int increase_count(int count) {
    pthread_mutex_lock(&mutex);

    available_resources += count;
    printf("Released %d resources, %d remaining.\n", count, available_resources);

    pthread_cond_broadcast(&cond);

    pthread_mutex_unlock(&mutex);
    return 0;
}

void* process(void* arg) {
    int count = *(int*)arg;

    decrease_count(count);

    my_sleep(1);

    increase_count(count);
    return NULL;
}



int main() {
    pthread_t threads[10];
    int request[10] = {1, 1, 2, 1, 1, 2, 1, 1, 1, 1};

    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, process, &request[i]);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}