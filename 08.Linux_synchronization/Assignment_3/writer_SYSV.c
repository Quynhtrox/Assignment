#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SIZE        1024
#define FILENAME    "troc"

int main() {
    key_t key = ftok(FILENAME, 65);
    int shmid = shmget(key, SIZE, 0666 | IPC_CREAT);

    char* str = (char*) shmat(shmid, NULL,0);

    strcpy(str, "Hello from SYSTEMV-V writer");
    printf("Writer wrote: %s\n", str);

    shmdt (str);
    
    return 0;
}