#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// ham stdlib dung de lam gi

// Function handle SIGINT
void handle_sigint() {
    printf("\nReceived SIGINT (Ctrl+C).\n");
}

int main() {
    struct sigaction sa;

    // Attach signal processing function
    sa.sa_handler = handle_sigint; // chu y con tro ham --> thuc hanh bai tap ve con tro ham 

    // Clear flags and mask
    // Khong clear thi sa_mask cos gia tri gi
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; //thuc hanh doi voi flags: sa_siginfo va sa_

    // Set action on receive when receiving SIGINT
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }
    //tim hieu ve current trong ham sigaction
    // tai sao phai backup lai trong sigaction
    printf("Program running. Press Ctrl+C to trigger SIGINT.\n");

    while (1) {
        printf("Working...\n");
        sleep(2);
    }
    return 0;
}