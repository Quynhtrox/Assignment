
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#define handle_error(msg) \
    do { perror("msg"); exit(EXIT_FAILURE);} while(0);

volatile int shared_value = 0;

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
    pthread_exit(NULL);
}

int main() {
    printf("Receiver's PID = %d\n", getpid());

    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    // Start pending-checker thread
    // pthread_t tid;
    // pthread_create(&tid, NULL, pending_checker, NULL);
    // pthread_detach(tid);

    sleep(10);

    // Block SIGUSR1
    sigset_t block_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &block_set, NULL) < 0)
        handle_error("sigprocmask() - block\n");
    printf("Blocking SIGUSR1 now.\n");

    pthread_t tid;
    pthread_create(&tid, NULL, pending_checker, NULL);
    pthread_detach(tid);

    // Simulate critical work (while signal is blocked)
    printf("Doing critical work...\n");
    for (int i = 0; i < 20; i++) {
        shared_value++;
        sleep(1);
    }

    // Restore previous mask = unblock SIGUSR1
    if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) < 0)
        handle_error("sigprocmask() - unblock\n");
    printf("Unblocking SIGUSR1 now.\n");

    // Wait to receive signal
    while (1) {
        pause();
    }

    return 0;
}
