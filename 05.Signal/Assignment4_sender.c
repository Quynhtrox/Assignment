#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <receiver_pid>\n", argv[0]);
        return 1;
    }

    pid_t target_pid = atoi(argv[1]);

    printf("[Sender] Sending SIGUSR1 to PID %d\n", target_pid);
    kill(target_pid, SIGUSR1);

    return 0;
}
