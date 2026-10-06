#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../src/file_log.h"
#include "../src/entry.h"

static int log_fd = -1;

int open_log(const char *filename){
    if((log_fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) == -1){
        perror("Failed to open log file");
        return -1;
    }
    return log_fd;
}

int close_log(){
    if(log_fd == -1){
        perror("Log file is already closed");
        return -1;
    }
    if(close(log_fd) == -1){
        perror("Failed to close log file");
        return -1;
    }
    log_fd = -1;
    return 0;
}

int log_to_file(log_entry *entry) {
    if(log_fd == -1){
        perror("Log file is not open");
        return -1;
    }
    uint8_t buffer[ENTRY_SIZE];
    encode_entry(entry, buffer);
    ssize_t result = write(log_fd, buffer, ENTRY_SIZE);
    if(result == -1){
        perror("Failed to write to log file");
    }
    if(fsync(log_fd) == -1){
        perror("Failed to sync log file");
    }
    return 0;
}
