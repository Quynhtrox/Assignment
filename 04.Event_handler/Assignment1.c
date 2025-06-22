#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

pid_t pid;

void sigusr1_handler(int sig);
void sigusr2_handler(int sig);

int main() {
    pid = getpid();

    printf("PID: %d\n", pid);
    printf("Send SIGUSR1: kill -10 %d\n", pid);
    printf("Send SIGUSR2: kill -12 %d\n", pid);

    struct sigaction su1, su2;

    su1.sa_handler = sigusr1_handler;
    su2.sa_handler = sigusr2_handler;

    sigemptyset(&su1.sa_mask);
    sigemptyset(&su2.sa_mask);

    su1.sa_flags = 0;
    su2.sa_flags = 0;

    sigaction(SIGUSR1, &su1, NULL);
    sigaction(SIGUSR2, &su2, NULL);

    while(1){
        printf("working ...\n");
        sleep(5);
    }

    return 0;
}

void sigusr1_handler(int sig) {
    printf("Hello from SIGUSR1\n");
}

void sigusr2_handler(int sig) {
    printf("I'm SIGUSR2. I will terminate program ...\n");
    exit(0);
}