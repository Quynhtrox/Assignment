#include <stdio.h>
#include <stdlib.h>     //exit()
#include <signal.h>
#include <unistd.h>     //sleep()

#define handle_error(msg) \
    do { perror("msg"); exit(EXIT_FAILURE);} while(0);

void handle_sig(int sig) {
    printf("Received signal: %d\n", sig);
}

void check_signal_mask() {
    sigset_t curr_mask;
    sigemptyset(&curr_mask);
    sigprocmask(0, NULL, &curr_mask);

    for (int i = 1; i < NSIG; i++) {
        if (sigismember(&curr_mask, i)) {
            printf("Signal %d is blocked\n", i);
        }
    }
}

void check_signal_var(sigset_t* set) {
    for (int i = 1; i < NSIG; i++) {
        if (sigismember(set, i)) {
            printf("Signal %d is in the set\n", i);
        }
    }
}

int main() {
    printf("PID: %d\n", getpid());
    sigset_t block_set, old_set;

    struct sigaction sa;
    sa.sa_handler = handle_sig;
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

    if (sigprocmask(SIG_BLOCK, &block_set, &old_set) < 0)
        handle_error("sigprocmask() - block\n");

    printf("\nCheck block_set.\n");
    check_signal_var(&block_set);

    printf("Blocked SIGINT, SIGTERM, and SIGQUIT for 10 seconds.\n");

    printf("\nCheck mash curent.\n");
    check_signal_mask();
    sleep(10);

    if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) < 0)
        handle_error("sigprocmask() - unblock\n");

    printf("SIGINT, SIGQUIT, SIGTERM is unblocked.\n");
    check_signal_mask();

    while(1) {
        printf("working ...\n");
        sleep(5);
    }
    
    return 0;
}