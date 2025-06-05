#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void child_process1();
void child_process2();
void parent_process();
void sigchild_handler(int sig);

int main() {
    signal(SIGCHLD, sigchild_handler); // Đăng ký xử lý sớm
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 < 0) {
        perror("fork 1 failed");
        exit(1);
    }
    if (pid1 == 0) {
        child_process1();
    }

    // Chỉ cha mới đến được đây
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork 2 failed");
        exit(1);
    }
    if (pid2 == 0) {
        child_process2();
    }

    // Chỉ cha đến được đây nếu cả hai fork() thành công
    parent_process();
    return 0;
}

void child_process1() {
    printf("I'm child process 1st. My PID: %d\n", getpid());

    sleep(5);
    exit(0);
}

void child_process2() {
    printf("I'm child process 2nd. My PID: %d\n", getpid());

    sleep(100);
    exit(0);
}

void parent_process() {
    signal(SIGCHLD, sigchild_handler);
    printf("I'm parent process. My PID: %d\n", getpid());

    while(1) pause();
}

void sigchild_handler(int sig) {
    printf("I'm SigChild\n");
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("SIGCHLD: Child %d exited\n", pid);
    }
}