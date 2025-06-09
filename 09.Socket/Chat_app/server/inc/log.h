#ifndef LOG_H
#define LOG_H

#include"shared_data.h"

void log_system(const char *message);
void log_message(const char *username, const char *message);
void save_to_history(const char *message);

#endif //LOG_H