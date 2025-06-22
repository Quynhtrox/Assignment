#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

int main() {
    fd_set rfds;
    struct timeval tv;
    int retval;
    char buffer[256];
    ssize_t bytesRead;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);  // fd = 0 is stdin

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    printf("Waiting for keyboard input ...\n");

    retval = select(1, &rfds, NULL, NULL, &tv);

    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is already to read!\n");

        if (FD_ISSET(0, &rfds)) {
            bytesRead = read(0, buffer, sizeof(buffer) - 1);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                printf("Received data: %s\n", buffer);
            } else if (bytesRead == 0) {
                printf("End of input received\n");
            } else {
                perror("read()");
            }
        }
    else
        printf("Don't receive data in 10s.\n");

    return 0;
}