#include "shared_data.h"

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t storage_mutex = PTHREAD_MUTEX_INITIALIZER;

client_t *clients[MAX_CLIENTS];

int PORT;