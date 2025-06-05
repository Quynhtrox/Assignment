#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* thread(void* args);

int main() {
    pthread_t tid;
    printf("Main TID: %lu\n", pthread_self());
    pthread_create(&tid, NULL, thread, NULL);
    printf("thread is created\n");
    pthread_detach(tid);
    // pthread_join(tid, NULL);
    printf("Hello main thread is running\n");
    while(1);
    return 0;
}

void* thread(void* args) {
    printf("Hello i'm thread. My TID: %lu\n", pthread_self());
    sleep(5);
    printf("Thread exit\n");
    pthread_exit(NULL);
}