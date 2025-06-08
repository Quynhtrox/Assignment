#include <stdio.h>
#include <stdlib.h>     // eixt()
#include <signal.h>
#include <unistd.h>     //sleep()
#include <string.h>     //strsignal()

#define handle_error(msg) \
    do { perror("msg"); exit(EXIT_FAILURE);} while(0);

void pending_signals() {
    sigset_t pending;
    sigpending(&pending);

    printf("Pending signals:\n");
    for (int sig = 1; sig < NSIG; sig++) {
        if (sigismember(&pending, sig)) {
            printf("Signal %2d: %s\n", sig, strsignal(sig));
        }
    }
}

void handle_sig(int sig) {
    printf("Received signal: %d\n", sig);
}

int main() {
    sigset_t block_set;

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    int signals[] = {SIGINT, SIGTERM, SIGQUIT};
    for (int i = 0; i < sizeof(signals)/sizeof(signals[0]); i++) {
        sigaction(signals[i], &sa, NULL);
    }

    sigemptyset(&block_set);

    for (int i = 0; i < sizeof(signals)/sizeof(signals[0]); i++) {
        sigaddset(&block_set, signals[i]);
    }


    if (sigprocmask(SIG_BLOCK, &block_set, NULL) < 0)
        handle_error("sigprocmask() - block\n");

    printf("Blocked SIGINT, SIGTERM, and SIGQUIT for 30 seconds.\n");
    sleep(30);

    pending_signals();

    if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) < 0)
        handle_error("sigprocmask() - unblock\n");

    printf("SIGINT, SIGQUIT, SIGTERM is unblocked.\n");

    while(1) {
        printf("working ...\n");
        sleep(5);
    }
    
    return 0;
}