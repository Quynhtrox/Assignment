#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

volatile sig_atomic_t shared_value = 0;

void handle_sigusr1(int signo) {
    printf("Received SIGUSR1, shared_value = %d\n", shared_value);
}

// Thread to scan pending signals
void *pending_checker(void *arg) {
    while (1) {
        sigset_t pending;
        sigpending(&pending);

        if (sigismember(&pending, SIGUSR1)) {
            printf("SIGUSR1 is pending...\n");
        } else {
            printf("No pending SIGUSR1\n");
        }

        sleep(1);
    }
    return NULL;
}

int main() {
    printf("Receiver's PID = %d\n", getpid());

    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    sleep(10);

    // Block SIGUSR1
    sigset_t block_set, prev_mask;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &block_set, &prev_mask);

    shared_value++;
    // Start pending-checker thread
    pthread_t tid;
    pthread_create(&tid, NULL, pending_checker, NULL);
    pthread_detach(tid);
    
    // Simulate critical work (while signal is blocked)
    for (int i = 0; i < 10; i++) {
        printf("Doing critical work...\n");
        sleep(2);
    }

    // Restore previous mask = unblock SIGUSR1
    printf("Unblocking SIGINT now.\n");
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);

    // Wait to receive signal
    while (1) {
        pause();
    }

    return 0;
}
