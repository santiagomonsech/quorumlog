#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
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

sighandler_t set_signal_ignore(int signal_id){
    return signal(signal_id, SIG_IGN);
}

void undo_signal_ignore(int signal_id, sighandler_t signal_handler){
    signal(signal_id, signal_handler);
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

ssize_t _rollback_last_entry(){
    if(log_fd == -1){
        perror("Log file is not open");
        return -1;
    }
    ssize_t offset = lseek(log_fd, 0, SEEK_END);
    if(offset == -1){
        perror("An error ocurred while trying to rollback the log file");
        return -1;
    }
    ssize_t diff = (offset % ENTRY_SIZE);
    if(diff != 0){
        if(ftruncate(log_fd, offset - diff) == -1){
            perror("Error truncating the file");
            return -1;
        }
    }
    printf("Last entry was rollbacked\n");
    return 0;
}

int _write_entry(const uint8_t buffer[], const size_t entry_size){
    size_t bytes_left = entry_size;
    ssize_t bytes_written = 0;

    uint8_t * buffer_ptr = (uint8_t *)buffer;

    while(bytes_left > 0){
        if((bytes_written = write(log_fd, buffer_ptr, bytes_left)) <= 0){
            if(errno == EINTR){
                bytes_written = 0;
            }else{
                perror("Error writing to log");
                _rollback_last_entry();
                return -1;
            }
        }
        buffer_ptr += bytes_written;
        bytes_left -= bytes_written;
    }
    if(fsync(log_fd) < 0){
        perror("Error writing to log");
        _rollback_last_entry();
        return -1;
    }
    return 0;
}

int log_to_file(log_entry *entry) {
    if(log_fd == -1){
        perror("Log file is not open");
        return -1;
    }
    uint8_t buffer[ENTRY_SIZE];
    encode_entry(entry, buffer);
    if(_write_entry(buffer, ENTRY_SIZE) < 0){
        return -1;
    }
    return 0;
}
