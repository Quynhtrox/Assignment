#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX_THREAD  15
#define MAX_PRINTER 3

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int printers[MAX_PRINTER] = {0};
sem_t sem;

void* thread(void* arg);

int main(){
    pthread_t tid[MAX_THREAD];

    sem_init(&sem, 0, MAX_PRINTER);
    

    for (int i = 0; i < MAX_THREAD; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&tid[i], NULL, thread, arg);
        sleep(1);
        free(arg);
    }

    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(tid[i], NULL);
    }

    sem_destroy(&sem);
    pthread_mutex_destroy(&lock);

    return 0;
}

void* thread(void *arg) {
    int thread_id = *(int*)arg;

    printf("Thread %d is waiting for a printer...\n", thread_id);
    sem_wait(&sem);

    int printer_id = -1;

    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_PRINTER; i++) {
        if (printers[i] == 0) {
            printers[i] = 1;
            printer_id = i;
            break;
        }
    }
    pthread_mutex_unlock(&lock);

    printf("Thread %d is using printer %d\n", thread_id, printer_id);
    sleep(rand() % 10 + 1);
    printf("Thread %d has finished using printer %d\n", thread_id, printer_id);

    printers[printer_id] = 0;

    sem_post(&sem);
    pthread_exit(NULL);
}
