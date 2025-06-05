#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREAD  5

int a = 0;
pthread_spinlock_t lock;

void* thread_handler(void* arg);


int main() {
    pthread_t thread[MAX_THREAD];
    pthread_spin_init(&lock, 0);
    
    printf("Main TID: %lu\n", pthread_self());



    for (int i = 0; i < MAX_THREAD; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&thread[i], NULL, thread_handler, arg);
    }

    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(thread[i], NULL);
    }
    

    printf("%d\n", a);

    return 0;
}

void* thread_handler(void* arg) {

    int thread_id = *(int*)arg;
    free(arg);
    
    printf("Hello i'm thread %d. my TID: %lu\n",thread_id, pthread_self());
    for (int i = 0; i < 50000; i++) {
        pthread_spin_lock(&lock);
        a++;
        printf("Thread %d add 1 to 'a' | a = %d\n", thread_id, a);
        pthread_spin_unlock(&lock);
    }
    pthread_exit(NULL);
}