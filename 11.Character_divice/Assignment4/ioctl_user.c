#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include "Q_ioctl.h"

int main() {
    int fd = open("/dev/trocdev0", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    write(fd, "Hello world", 12);

    int size;
    ioctl(fd, Q_GET_BUFFER_SIZE, &size);
    printf("Buffer size = %d bytes\n", size);

    ioctl(fd, Q_CLEAR_BUFFER, NULL);
    printf("Buffer cleared\n");

    ioctl(fd, Q_GET_BUFFER_SIZE, &size);
    printf("Buffer size after clear = %d bytes\n", size);

    close(fd);
    return 0;
}