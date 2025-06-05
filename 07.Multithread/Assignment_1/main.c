#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t key = PTHREAD_COND_INITIALIZER;

int condition1, condition2;

void* thread_1(void* args);
void* thread_2(void* args);
void* thread_3(void* args);


int main() {
    pthread_t thread1, thread2, thread3;

    printf("Main TID: %lu\n", pthread_self());

    pthread_create(&thread1, NULL, thread_1, NULL);
    pthread_create(&thread2, NULL, thread_2, NULL);
    pthread_create(&thread3, NULL, thread_3, NULL);

    // if (pthread_equal(thread1, thread2)) {
    //     printf("Thread 1 is the same thread 2\n");
    // } else {
    //     printf("Thread 1 is NOT the same thread 2\n");
    // }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    return 0;
}

void* thread_1(void* args) {
    sleep(2);
    printf("Hello i'm thread 1. My TID: %lu\n", pthread_self());
    
    pthread_mutex_lock(&lock);
    sleep(2);
    condition1 = 1;
    pthread_cond_signal(&key);
    pthread_mutex_unlock(&lock);

    sleep(2);

    pthread_mutex_lock(&lock);
    sleep(2);
    condition2 = 1;
    pthread_cond_signal(&key);
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

void* thread_2(void* args) {
    printf("Hello Im thread 2. My TID: %lu\n", pthread_self());
    
    pthread_mutex_lock(&lock);
    while(!condition1) {
        pthread_cond_wait(&key, &lock);
    }
    printf("Thread_2 running ...\n");
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

void* thread_3(void* args) {
    printf("Hello Im thread 3. My TID: %lu\n", pthread_self());
   
    pthread_mutex_lock(&lock);
    while(!condition2) {
        pthread_cond_wait(&key, &lock);
    }
    printf("Thread_3 running ...\n");
    pthread_mutex_unlock(&lock);
  
    return NULL;
}