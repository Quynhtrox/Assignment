#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 0;

void* thr_reader(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&rwlock);
        printf("Reader %d read: %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    return NULL;
}

void* thr_writer(void* arg) {
    for (int i = 1; i <= 5; i++) {
        pthread_rwlock_wrlock(&rwlock);
        shared_data = i * 10;
        printf("Writer wrote: %d\n", shared_data);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thr_r1, thr_r2, thr_w;
    int id1 = 1, id2 = 2;

    pthread_create(&thr_r1, NULL, thr_reader, &id1);
    pthread_create(&thr_r2, NULL, thr_reader, &id2);
    pthread_create(&thr_w, NULL, thr_writer, NULL);

    pthread_join(thr_r1, NULL);
    pthread_join(thr_r2, NULL);
    pthread_join(thr_w, NULL);

    pthread_rwlock_destroy(&rwlock);
    return 0;
}
