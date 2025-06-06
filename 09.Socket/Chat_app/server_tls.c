#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/sha.h>

#define PORT 4433
#define MAX_CLIENTS 10
#define BUFFER_SIZE 2048

typedef struct {
    SSL *ssl;
    int socket;
    char username[32];
} client_t;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t storage_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_system(const char *message) {
    pthread_mutex_lock(&log_mutex);
    FILE *log = fopen("chat.log", "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "[%Y-%m-%d %H:%M:%S]", tm_info);
        fprintf(log, "%s [SYSTEM]: %s\n", time_str, message);
        fclose(log);
    }
    pthread_mutex_unlock(&log_mutex);
}

void log_message(const char *username, const char *message) {
    pthread_mutex_lock(&log_mutex);
    FILE *log = fopen("chat.log", "a");
    if (log) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "[%Y-%m-%d %H:%M:%S]", tm_info);
        fprintf(log, "%s %s: %s\n", time_str, username, message);
        fclose(log);
    }
    pthread_mutex_unlock(&log_mutex);
}

void trim_newline(char *text) {
    text[strcspn(text, "\r\n")] = '\0';
}

void save_to_history(const char *message) {
    FILE *fp = fopen("chat_history.txt", "a");
    if (fp) {
        fprintf(fp, "%s\n", message);
        fclose(fp);
    }
}

void broadcast_message(const char *message, client_t *sender) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i] != sender) {
            SSL_write(clients[i]->ssl, message, strlen(message));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    save_to_history(message);
    const char *msg_content = strchr(message, ':');
    if (msg_content && msg_content[1] != '\0') {
        log_message(sender->username, msg_content + 2);
    }
}

void hash_password(const char *password, char *output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)password, strlen(password), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
}

int username_exists(const char *username) {
    pthread_mutex_lock(&storage_mutex);
    FILE *fp = fopen("storage_account.txt", "r");
    if (!fp) {
        pthread_mutex_unlock(&storage_mutex);
        return 0;
    }
    char line[128], stored_user[64];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%63[^:]", stored_user);
        if (strcmp(username, stored_user) == 0) {
            fclose(fp);
            pthread_mutex_unlock(&storage_mutex);
            return 1;
        }
    }
    fclose(fp);
    pthread_mutex_unlock(&storage_mutex);
    return 0;
}

void *handle_client(void *arg) {
    client_t *cli = (client_t *)arg;
    int len;
    char buffer[BUFFER_SIZE];
    char password[64], hash[65];
    char option[16];

    SSL_write(cli->ssl, "Choose: login / register\n", 26);
    len = SSL_read(cli->ssl, option, sizeof(option) - 1);
    if (len <= 0) goto cleanup;
    option[len] = '\0'; trim_newline(option);

    SSL_write(cli->ssl, "Username: ", 10);
    len = SSL_read(cli->ssl, cli->username, sizeof(cli->username) - 1);
    if (len <= 0) goto cleanup;
    cli->username[len] = '\0'; trim_newline(cli->username);

    SSL_write(cli->ssl, "Password: ", 10);
    len = SSL_read(cli->ssl, password, sizeof(password) - 1);
    if (len <= 0) goto cleanup;
    password[len] = '\0'; trim_newline(password);
    hash_password(password, hash);

    if (strcmp(option, "register") == 0) {
        if (username_exists(cli->username)) {
            SSL_write(cli->ssl, "Username already exists\n", 24);
            goto cleanup;
        }
        pthread_mutex_lock(&storage_mutex);
        FILE *fp = fopen("storage_account.txt", "a");
        if (fp) {
            fprintf(fp, "%s:%s\n", cli->username, hash);
            fclose(fp);
            pthread_mutex_unlock(&storage_mutex);
            SSL_write(cli->ssl, "Register successful\n", 20);
        } else {
            pthread_mutex_unlock(&storage_mutex);
            SSL_write(cli->ssl, "Register failed\n", 16);
            goto cleanup;
        }
    } else if (strcmp(option, "login") == 0) {
        pthread_mutex_lock(&storage_mutex);
        FILE *fp = fopen("storage_account.txt", "r");
        int found = 0;
        if (fp) {
            char line[128], stored_user[64], stored_hash[65];
            while (fgets(line, sizeof(line), fp)) {
                sscanf(line, "%63[^:]:%64s", stored_user, stored_hash);
                if (strcmp(cli->username, stored_user) == 0 && strcmp(hash, stored_hash) == 0) {
                    found = 1;
                    break;
                }
            }
            fclose(fp);
        }
        pthread_mutex_unlock(&storage_mutex);
        if (!found) {
            SSL_write(cli->ssl, "Login failed\n", 13);
            goto cleanup;
        }
        SSL_write(cli->ssl, "Login successful\n", 17);
    } else {
        SSL_write(cli->ssl, "Invalid option\n", 15);
        goto cleanup;
    }

    char welcome[64];
    snprintf(welcome, sizeof(welcome), "%s joined the chat\n", cli->username);
    broadcast_message(welcome, cli);

    while ((len = SSL_read(cli->ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[len] = '\0'; trim_newline(buffer);
        if (strlen(buffer) == 0) continue;
        char message[BUFFER_SIZE + 64];
        snprintf(message, sizeof(message), "%s: %s\n", cli->username, buffer);
        broadcast_message(message, cli);
    }

cleanup:
    if (strlen(cli->username) > 0) {
        char bye[64];
        snprintf(bye, sizeof(bye), "%s has left the chat\n", cli->username);
        broadcast_message(bye, cli);
    }
    close(cli->socket);
    SSL_shutdown(cli->ssl);
    SSL_free(cli->ssl);
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == cli) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    free(cli);
    pthread_exit(NULL);
}

void handle_sigint(int sig) {
    log_system("Server shutting down");
    exit(0);
}

int main() {
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
    printf("[+] Server listening on port %d (HTTPS)\n", PORT);
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
