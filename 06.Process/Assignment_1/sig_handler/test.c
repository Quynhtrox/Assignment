#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void parent_process(void);
void child_process_1(void);
void child_process_2(void);
void sigchld_handler(int);

int main() {
	int pid1 = fork();
	if(pid1 == 0) {
		child_process_1();
	} else {
		int pid2 = fork();
		if(pid2 == 0) {
			child_process_2();
		} else {
			parent_process();
		}
	}
	return 0;
}

void parent_process(void) {
	unsigned int count;
	signal(SIGCHLD, sigchld_handler);
	while(1) {
		printf("This is from parent. PID = %d, count = %d\n", getpid(), count++);
		#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void parent_process(void);
void child_process_1(void);
void child_process_2(void);
void sigchld_handler(int);

int main() {
	int pid1 = fork();
	if(pid1 == 0) {
		child_process_1();
	} else {
		int pid2 = fork();
		if(pid2 == 0) {
			child_process_2();
		} else {
			parent_process();
		}
	}
	return 0;
}

void parent_process(void) {
	unsigned int count;
	signal(SIGCHLD, sigchld_handler);
	while(1) {
		printf("This is from parent. PID = %d, count = %d\n", getpid(), count++);
		if(count > 20) {
			int status = 0;
			int tmp = waitpid(-1, &status, WNOHANG);
			if(tmp > 0) {
				printf("my child %d ended, done cleaning up\n", tmp);
				if(WIFEXITED(status)) {
					printf("It died in peace with code %d. Lomen\n", WEXITSTATUS(status));
				} else if(WIFSIGNALED(status)) {
					printf("It was terminated/killed by SIGNAL %d\n", WTERMSIG(status));
				} else if(WIFSTOPPED(status)) {
					printf("It was stopped by SIGNAL %d\n", WSTOPSIG(status));
				} else if(WIFCONTINUED(status)) {
					printf("Oh now it's continued\n");
				} else {
					printf("I dont know wtf is going on here\n");
				}
			}
		}
		sleep(2);

	}
}

void child_process_1(void) {
	unsigned int count;
	while(1) {
		int ppid = getppid();
		printf("I'm child process 1, PID = %d,  my parent process %d, count = %d\n", getpid(), ppid, count++);
		if(count == 5) {
			printf("I (1) now wil exit and become a zombie till mah pa kill me\n");
			exit(0);
		}
		sleep(2);
	}

}

void child_process_2(void) {
	unsigned int count;
	while(1) {
		int ppid = getppid();
		printf("I'm child process 2, PID = %d,  my parent process %d, count = %d\n", getpid(), ppid, count++);
		if(count == 10) {
			printf("I (2) now wil exit and become a zombie till mah pa kill me\n");
			exit(0);
		}
		sleep(2);
	}
}

void sigchld_handler(int sig) {
	if(sig == SIGCHLD) {
		printf("received signal no %d\n", sig);
		int status = 0;
		int tmp = waitpid(-1, &status, WNOHANG);
		if(tmp > 0) {
			printf("my child %d ended, done cleaning up\n", tmp);
			if(WIFEXITED(status)) {
				printf("It died in peace with code %d. Lomen\n", WEXITSTATUS(status));
			} else if(WIFSIGNALED(status)) {
				printf("It was terminated/killed by SIGNAL %d\n", WTERMSIG(status));
			} else if(WIFSTOPPED(status)) {
				printf("It was stopped by SIGNAL %d\n", WSTOPSIG(status));
			} else if(WIFCONTINUED(status)) {
				printf("Oh now it's continued\n");
			} else {
				printf("I dont know wtf is going on here\n");
			}
		}
	}	
}
		sleep(2);

	}
}

void child_process_1(void) {
	unsigned int count;
	while(1) {
		int ppid = getppid();
		printf("I'm child process 1, PID = %d,  my parent process %d, count = %d\n", getpid(), ppid, count++);
		if(count == 5) {
			printf("I (1) now wil exit and become a zombie till mah pa kill me\n");
			exit(0);
		}
		sleep(2);
	}

}

void child_process_2(void) {
	unsigned int count;
	while(1) {
		int ppid = getppid();
		printf("I'm child process 2, PID = %d,  my parent process %d, count = %d\n", getpid(), ppid, count++);
		if(count == 10) {
			printf("I (2) now wil exit and become a zombie till mah pa kill me\n");
			exit(0);
		}
		sleep(2);
	}
}

void sigchld_handler(int sig) {
	if(sig == SIGCHLD) {
		printf("received signal no %d\n", sig);
		int status = 0;
		int tmp = waitpid(-1, &status, WNOHANG);
		if(tmp > 0) {
			printf("my child %d ended, done cleaning up\n", tmp);
			if(WIFEXITED(status)) {
				printf("It died in peace with code %d. Lomen\n", WEXITSTATUS(status));
			} else if(WIFSIGNALED(status)) {
				printf("It was terminated/killed by SIGNAL %d\n", WTERMSIG(status));
			} else if(WIFSTOPPED(status)) {
				printf("It was stopped by SIGNAL %d\n", WSTOPSIG(status));
			} else if(WIFCONTINUED(status)) {
				printf("Oh now it's continued\n");
			} else {
				printf("I dont know wtf is going on here\n");
			}
		}
	}	
}