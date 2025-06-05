#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main() {
    const char* name = "/troc";
    const int SIZE = 4096;

    int shm_fd;
    shm_fd = shm_open(name, O_RDONLY, 0666);

    void* ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    printf("Reader received: %s", (char*)ptr);

    munmap(ptr, SIZE);
    close(shm_fd);
    shm_unlink(name);

    return 0;
}