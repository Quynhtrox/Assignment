#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* thread_1(void* args);
void* thread_2(void* args);

int main() {
    pthread_t thread1, thread2;

    printf("Main TID: %lu\n", pthread_self());

    pthread_create(&thread1, NULL, thread_1, NULL);
    pthread_create(&thread2, NULL, thread_2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}

void* thread_1(void* args) {
    printf("Hello i'm thread 1. My TID: %lu\n", pthread_self());
    return NULL;
}

void* thread_2(void* args) {
    printf("Hello Im thread 2. My TID: %lu\n", pthread_self());
    return NULL;
}
