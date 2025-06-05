#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREAD  5

int a = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* thread_handler(void* args);


int main() {
    pthread_t thread[MAX_THREAD];
    
    printf("Main TID: %lu\n", pthread_self());

    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_create(&thread[i], NULL, thread_handler, NULL);
    }

    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(thread[i], NULL);
    }

    printf("%d\n", a);

    return 0;
}

void* thread_handler(void* args) {
    pthread_mutex_lock(&lock);
    printf("Hello my TID: %lu\n", pthread_self());
    for (int i = 0; i < 50000; i++) {
        a++;
    }
    pthread_mutex_unlock(&lock);
    
    pthread_exit(NULL);
}