#ifndef HANDLE_CLIENT_H
#define HANDLE_CLIENT_H

#include "shared_data.h"

void *handle_client(void *arg);
void handle_sigint(int sig);

#endif //HANDLE_CLIENT_H