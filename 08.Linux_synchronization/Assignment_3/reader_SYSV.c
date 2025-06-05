#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SIZE        1024
#define FILENAME    "troc"

int main() {
    key_t key = ftok(FILENAME, 65);

    int shmid = shmget(key, SIZE, 0666);
    
    char* str = (char*) shmat(shmid, NULL, 0);

    printf("Reader read: %s\n", str);
    shmdt(str);

    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}