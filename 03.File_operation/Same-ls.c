#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

const char* get_username(uid_t uid);
void format_time(time_t rawtime, char *buffer, size_t size);
const char* get_file_type(mode_t mode);
void display_file_info(const char *directory, const char *filename);
void list_directory(const char *path);

int main(int argc, char *argv[]) {
    const char *path = (argc > 1) ? argv[1] : ".";

    printf("Inspecting directory: %s\n", path);
    printf("====================================\n");

    list_directory(path);

    return 0;
}

// Function convert UID to username
const char* get_username(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "Unknown";
}

// Function to convert time format to string
void format_time(time_t rawtime, char *buffer, size_t size) {
    struct tm *timeinfo = localtime(&rawtime);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

// Function to check file type
const char* get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return "File";
    if (S_ISDIR(mode)) return "Directory";
    return "Other";
}

// Function to process and print file information
void display_file_info(const char *directory, const char *filename) {
    char fullpath[4096];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, filename);

    struct stat file_stat;
    if (stat(fullpath, &file_stat) == -1) {
        perror("stat");
        return;
    }

    char mtime[64], ctime[64];
    format_time(file_stat.st_mtime, mtime, sizeof(mtime));
    format_time(file_stat.st_ctime, ctime, sizeof(ctime));

    printf("Name       : %s\n", filename);
    printf("Type       : %s\n", get_file_type(file_stat.st_mode));
    printf("Owner      : %s (UID: %d)\n", get_username(file_stat.st_uid), file_stat.st_uid);
    printf("Modified   : %s\n", mtime);
    printf("Meta-Change: %s\n", ctime);
    printf("------------------------------------\n");
}

// Function to loop through directory and display information
void list_directory(const char *path) {
    DIR *directory = opendir(path);
    if (!directory) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        // Clear '.' and '..'
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        display_file_info(path, entry->d_name);
    }

    closedir(directory);
}


