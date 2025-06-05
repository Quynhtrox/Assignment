#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main() {
    pid_t pid;

    while (1) {
        pid = fork();
        if (pid == 0) {

            exit(0);
        }
        else if (pid > 0) {
            printf("Created child: %d\n", pid);
        }
        else {
            printf("Created child max\n");
            break;
        }
    }
    sleep(10);
    return 0;
}