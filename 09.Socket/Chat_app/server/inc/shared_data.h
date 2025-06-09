#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <openssl/ssl.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 2048

extern pthread_mutex_t clients_mutex;
extern pthread_mutex_t log_mutex;
extern pthread_mutex_t storage_mutex;

typedef struct {
    SSL *ssl;
    int socket;
    char username[32];
} client_t;

extern client_t *clients[MAX_CLIENTS];

extern int PORT;

#endif //SHARED_DATA_H