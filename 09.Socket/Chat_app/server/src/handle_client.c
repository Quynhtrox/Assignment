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

void trim_newline(char *text);
void broadcast_message(const char *message, client_t *sender);
void hash_password(const char *password, char *outout);
int username_exists(const char *username);
void handle_sigint(int sig);

void *handle_client(void *arg) {
    client_t *cli = (client_t *)arg;
    int len;
    char buffer[BUFFER_SIZE];
    char password[64], hash[65];
    char option[16];

    // Choose login/sign_up mode
    SSL_write(cli->ssl, "Choose: log_in / sign_up\n", 26);
    len = SSL_read(cli->ssl, option, sizeof(option) - 1);
    if (len <= 0) goto cleanup;
    option[len] = '\0'; trim_newline(option);

    // Enter username
    SSL_write(cli->ssl, "Username: ", 10);
    len = SSL_read(cli->ssl, cli->username, sizeof(cli->username) - 1);
    if (len <= 0) goto cleanup;
    cli->username[len] = '\0'; trim_newline(cli->username);

    // Enter password 
    SSL_write(cli->ssl, "Password: ", 10);
    len = SSL_read(cli->ssl, password, sizeof(password) - 1);
    if (len <= 0) goto cleanup;
    password[len] = '\0'; trim_newline(password);
    hash_password(password, hash);

    if (strcmp(option, "sign_up") == 0) {
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
            SSL_write(cli->ssl, "sign_up successful\n", 20);
        } else {
            pthread_mutex_unlock(&storage_mutex);
            SSL_write(cli->ssl, "sign_up failed\n", 16);
            goto cleanup;
        }
    } else if (strcmp(option, "log_in") == 0) {
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
            SSL_write(cli->ssl, "log_in failed\n", 13);
            goto cleanup;
        }
        SSL_write(cli->ssl, "log_in successful\n", 17);
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

void trim_newline(char *text) {
    text[strcspn(text, "\r\n")] = '\0';
}

/* Send messenger to all client */
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

/* Encrypt password with SHA-256, export in hex format */
void hash_password(const char *password, char *output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)password, strlen(password), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
}

/* Check if username exists in storage_account.txt file */
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

/* Handle SIGINT */
void handle_sigint(int sig) {
    log_system("Server shutting down");
    printf("Signal %d: SIGINT\n", sig);
    exit(0);
}