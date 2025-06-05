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
        
        exit(0);
    }
    else {
        printf("I'm parent process, PID: %d\n", getpid());
        while(1);
    }   

    return 0;
}