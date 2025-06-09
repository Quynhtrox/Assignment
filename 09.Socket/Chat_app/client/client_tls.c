#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 2048

SSL *ssl = NULL;
int sockfd = -1;
volatile int running = 1;

pthread_t recv_thread, send_thread;

/* Clear '\r\n' */
void trim_newline(char *str) {
    str[strcspn(str, "\r\n")] = '\0';
}

/* Close SSL & socket, clean data */
void cleanup() {
    running = 0;
    if (ssl) SSL_shutdown(ssl);
    if (ssl) SSL_free(ssl);
    if (sockfd != -1) close(sockfd);
    printf("\n[+] Disconnected from server\n");
}

/* Handle SIGINT */
void handle_exit(int sig) {
    cleanup();
    exit(0);
}

/* Receive and handle messenger from server */
void *receive_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    while (running) {
        int len = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (len <= 0) break;
        buffer[len] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }
    running = 0;
    return NULL;
}

/* Send messenger to server */
void *send_handler(void *arg) {
    char buffer[BUFFER_SIZE];
    while (running && fgets(buffer, sizeof(buffer), stdin)) {
        trim_newline(buffer);
        if (strcmp(buffer, "/quit") == 0) {
            running = 0;
            break;
        }
        if (SSL_write(ssl, buffer, strlen(buffer)) <= 0) {
            fprintf(stderr, "[-] Failed to send message\n");
            running = 0;
            break;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server_IP> <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[2]);

    signal(SIGINT, handle_exit);

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (!SSL_CTX_load_verify_locations(ctx, "/home/troc/workplace/Assignments/09.Socket/Chat_app/server/server.crt", NULL)) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(sockfd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];

    // Login
    int len = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (len <= 0) { cleanup(); return 1; }
    buffer[len] = '\0';
    printf("%s", buffer);

    fgets(buffer, sizeof(buffer), stdin);
    trim_newline(buffer);
    SSL_write(ssl, buffer, strlen(buffer));

    len = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (len <= 0) { cleanup(); return 1; }
    buffer[len] = '\0';
    printf("%s", buffer);

    fgets(buffer, sizeof(buffer), stdin);
    trim_newline(buffer);
    SSL_write(ssl, buffer, strlen(buffer));

    // Login result
    len = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (len <= 0) { cleanup(); return 1; }
    buffer[len] = '\0';
    printf("%s", buffer);
    if (strstr(buffer, "failed")) {
        cleanup();
        SSL_CTX_free(ctx);
        return 1;
    }

    // Threads
    pthread_create(&recv_thread, NULL, receive_handler, NULL);
    pthread_create(&send_thread, NULL, send_handler, NULL);

    pthread_join(send_thread, NULL);
    running = 0;
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);

    cleanup();
    SSL_CTX_free(ctx);
    return 0;
}