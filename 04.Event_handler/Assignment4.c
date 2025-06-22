#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>

#define MAX_BUFFER_SIZE 256

char event_queue[MAX_BUFFER_SIZE];
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

void *bottom_half_thread(void *args) {
    while (1)
    {
        pthread_mutex_lock(&queue_lock);
        pthread_cond_wait(&queue_cond, &queue_lock);

        printf("Bottom half handling data: %s\n", event_queue);

        pthread_mutex_unlock(&queue_lock);
    }

    return NULL;
}

void handle_top_half(int fd) {
    char buffer[MAX_BUFFER_SIZE];
    int len = read(fd, buffer, sizeof(buffer) - 1);
    if (len > 0) {
        buffer[len] = '\0';

        pthread_mutex_lock(&queue_lock);
        snprintf(event_queue, sizeof(event_queue), "%s", buffer);
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&queue_lock);

        printf("Top half received data: %s", buffer);
    }
}

int main() {
    pthread_t bh_thread;
    pthread_create(&bh_thread, NULL, bottom_half_thread, NULL);

    int fd = epoll_create1(0);
    struct epoll_event ev, events[1];
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

    printf("Waiting event ...\n");
    while (1) {
        int n = epoll_wait(fd, events, 1, -1);
        if (epoll_wait(fd, events, 1, -1)) {
            if (events[0].data.fd == STDIN_FILENO)
                handle_top_half(STDIN_FILENO);
        }
    }
    close(fd);
    return 0;
 }