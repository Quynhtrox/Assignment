#include <string.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <openssl/err.h>
#include <openssl/sha.h>

#include "shared_data.h"
#include "log.h"
#include "handle_client.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    PORT = atoi(argv[1]);

    signal(SIGINT, handle_sigint);

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) exit(EXIT_FAILURE);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
  
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
  
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
  
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
  
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d (HTTPS)\n", PORT);
    log_system("Server started");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &len);
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_sock);
        if (SSL_accept(ssl) <= 0) {
            close(client_sock);
            SSL_free(ssl);
            continue;
        }

        client_t *cli = malloc(sizeof(client_t));
        if (!cli) {
            perror("Malloc failed");
            close(client_sock);
            SSL_free(ssl);
            continue;
        }
      
        cli->socket = client_sock;
        cli->ssl = ssl;
        memset(cli->username, 0, sizeof(cli->username));

        pthread_mutex_lock(&clients_mutex);
        int added = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i]) {
                clients[i] = cli;
                added = 1;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        if (!added) {
            SSL_write(ssl, "Server full\n", 12);
            close(client_sock);
            SSL_shutdown(ssl);
            SSL_free(ssl);
            free(cli);
            continue;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, (void *)cli) != 0) {
            perror("Thread creation failed");
            continue;
        }
        pthread_detach(tid);
    }

    close(server_fd);
    SSL_CTX_free(ctx);
    return 0;
}
