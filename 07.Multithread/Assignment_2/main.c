#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* thread_1(void* args);
void* thread_2(void* args);

int main() {
    pthread_t tid, thread1, thread2;
    int a = 0;

    printf("Main TID: %lu\n", pthread_self());

    pthread_create(&thread1, NULL, thread_1, &a);
    pthread_create(&thread2, NULL, thread_2, &a);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("%d\n", a);

    return 0;
}

void* thread_1(void* args) {
    pthread_mutex_lock(&lock);
    int* a = (int*)args;
    printf("Hello i'm thread 1. My TID: %lu\n", pthread_self());
    for (int i = 0; i < 50000; i++) {
        (*a)++;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* thread_2(void* args) {
    pthread_mutex_lock(&lock);
    int* a = (int*)args;
    printf("Hello Im thread 2. My TID: %lu\n", pthread_self());
    for (int i = 0; i < 50000; i++) {
        (*a)++;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}
