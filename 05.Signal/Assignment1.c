#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Function handle SIGINT
void handle_sigint(int sig) {
    printf("\nReceived SIGINT (Ctrl+C). Exiting gracefully...\n");
}

int main() {
    struct sigaction sa;

    // Attach signal processing funtion
    sa.sa_handler = handle_sigint;

    // Clear flags and mask
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Set action on receive when receiving SIGINT
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