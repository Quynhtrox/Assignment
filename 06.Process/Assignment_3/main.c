#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello world! My PID: %d\n", getpid());

    for (int i = 0; i <= 50; i++) {
        printf("Running ... \n");
        sleep(2);
    }

    printf("Good bye\n");
    return 0;
}
