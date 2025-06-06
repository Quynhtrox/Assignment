#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define handle_error(msg) \
    do { perror("msg"); exit(EXIT_FAILURE);} while(0);

void handle_sigint(int sig) {
    //printf("Received SIGINT!\n");
    printf("Received signal: %d\n", sig);
}

int main() {
    sigset_t block_set, old_set;

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);
    sigaddset(&block_set, SIGTERM);
    sigaddset(&block_set, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &block_set, &old_set) < 0)
        handle_error("sigprocmask() - block\n");

    printf("Blocked SIGINT, SIGTERM, and SIGQUIT for 10 seconds.\n");
    //printf("SIGINT is blocked for 10 seconds. Try press Ctrl+C now ...\n");
    sleep(10);

    if (sigprocmask(SIG_SETMASK, &old_set, NULL) < 0)
        handle_error("sigprocmask() - unblock\n");

    printf("SIGINT is unblocked. Press Ctrl+C now to trigger handler.\n");

    while(1) {
        printf("working ...\n");
        sleep(5);
    }
    
    return 0;
}