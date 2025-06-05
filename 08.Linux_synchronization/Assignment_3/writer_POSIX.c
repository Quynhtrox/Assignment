#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

int main() {
    const char* name = "/troc";
    const int SIZE = 4096;

    int shm_fd;

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);

    void* ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sprintf(ptr, "Hello from POSIX writer process\n");

    munmap(ptr, SIZE);
    close(shm_fd);
    return 0;
}
