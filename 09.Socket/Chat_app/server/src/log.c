#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "shared_data.h"
#include "log.h"

/* Save log activity server into chat.log */
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

/* Save log activity user into chat.log */
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

/* Save messenger to chat_history.txt */
void save_to_history(const char *message) {
    FILE *fp = fopen("chat_history.txt", "a");
    if (fp) {
        fprintf(fp, "%s\n", message);
        fclose(fp);
    }
}