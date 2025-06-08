#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint() {
    printf("\nReceived SIGINT (Ctrl+C).\n");
}

int main() {
    printf("PID: %d\n", getpid());

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Program running. Press Ctrl+C to trigger SIGINT.\n");

    while (1) {
        printf("Working...\n");
        sleep(2);
    }
    return 0;
}