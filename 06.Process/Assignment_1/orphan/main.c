#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main() {
    pid_t pid;
    pid = fork();

    if (pid == 0) {
        printf("I'm child processd, PID: %d\n", getpid());
        printf("My parent's PID: %d\n", getppid());
        sleep(5);
        printf("After 5s, new Parent's PID: %d\n", getppid());
    }
    else {
        printf("I'm parent process, PID: %d\n", getpid());
        sleep(2);
        exit(0);
    }

    return 0;
}