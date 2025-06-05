#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* thread(void* args);
void* thread_1(void* args);
void* thread_2(void* args);


int main() {
    pthread_t tid, thread1, thread2;
    int a = 0;

    printf("Main TID: %lu\n", pthread_self());

    // for (int i = 0; i <= 100000; i++) {
    //     pthread_create(&tid, NULL, thread, &a);
    //     printf("%d\n", a);
    //     pthread_join(tid, NULL);
    // }

    pthread_create(&thread1, NULL, thread_1, &a);
    pthread_create(&thread2, NULL, thread_2, &a);

    if (pthread_equal(thread1, thread2)) {
        printf("Thread 1 is the same thread 2\n");
    } else {
        printf("Thread 1 is NOT the same thread 2\n");
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("%d\n", a);

    return 0;
}

// void* thread(void* args) {
//     int* a = (int*)args;
//     (*a)++;
//     usleep(10);
// }

void* thread_1(void* args) {
    int* a = (int*)args;
    printf("Hello i'm thread 1. My TID: %lu\n", pthread_self());
    for (int i = 0; i <=50000; i++) {
        (*a)++;
    }
    return NULL;
}

void* thread_2(void* args) {
    int* a = (int*)args;
    printf("Hello Im thread 2. My TID: %lu\n", pthread_self());
    for (int i = 0; i <=50000; i++) {
        (*a)++;
    }
    return NULL;
}