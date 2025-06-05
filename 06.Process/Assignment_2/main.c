#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
    }

    if (pid == 0) {
        printf("I'm child process. My PID: %d\n", getpid());
        printf("My parent's PID: %d\n", getppid());

        exit(1);
    }
    else {
        printf("I'm parent process. My PID: %d\n", getpid());

        sleep(10);
        wait(NULL);
    }
    return 0;
}