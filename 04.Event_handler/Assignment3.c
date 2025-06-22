#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_EVENTS          2
#define MAX_BUFFER_SIZE     256
#define handle_error(msg)   \
    do { perror(msg); exit(EXIT_FAILURE);} while(0);

/* Handle stdin event and exit program when received 'exit' from keyboard */
void handle_stdin_event() {
    char buffer[MAX_BUFFER_SIZE];
    int len = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (len > 0) {
        buffer[len] = '\0';
        printf("From keyboard: %s", buffer);
        if (strncmp(buffer, "exit", 4) == 0)
            exit(0);
    }
}

/* Handle pipe event */
void handle_pipe_event(int fd, int pipe_read) {
    char buffer[MAX_BUFFER_SIZE];
    int len = read(pipe_read, buffer, sizeof(buffer) - 1);
    if (len > 0) {
        buffer[len] = '\0';
        printf("From pipe: %s", buffer);
    } else {
        printf("Pipe EOF\n");
        epoll_ctl(fd, EPOLL_CTL_DEL,  pipe_read, NULL);
        close(pipe_read);
    }
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        handle_error("pipe()");
    }

    int fd = epoll_create1(0);
    if (fd == -1) {
        handle_error("epoll_create1()");
    }

    struct epoll_event event, events[MAX_EVENTS];

    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;
    if (epoll_ctl(fd, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1) {
        handle_error("epoll_ctl");
    }

    event.events = EPOLLIN;
    event.data.fd = pipefd[0];
    if (epoll_ctl(fd, EPOLL_CTL_ADD, pipefd[0], &event) == -1) {
        handle_error("epoll_ctl");
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(pipefd[0]);
        sleep(3);
        const char *buf = "Data from pipe!\n";
        write(pipefd[1], buf, strlen(buf));
        close(pipefd[1]);
        exit(0);
    }

    printf("Wating for keyboard input and flower pipe ... (10S)...\n");

    while (1) {
        int nfds = epoll_wait(fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == STDIN_FILENO) {
                handle_stdin_event();  
            } else if (events[i].data.fd == pipefd[0]) {
                handle_pipe_event(fd, pipefd[0]);
            }
        }
    }

    close(fd);
    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}
