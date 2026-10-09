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

ssize_t _get_log_size(int fd)
{
    struct stat sb;

    if (fstat(fd, &sb) != 0)
    {
        perror("fstat");
        return -1;
    }
    return sb.st_size;
}

int _remove_corrupted_entries(int fd)
{
    ssize_t file_size = _get_log_size(fd);
    if (file_size == -1)
    {
        return -1;
    }
    ssize_t diff = (file_size % ENTRY_SIZE);
    if (diff != 0)
    {
        if (ftruncate(fd, file_size - diff) == -1)
        {
            perror("Error truncating the file");
            return -1;
        }
    }
    return 0;
}

int _apply_correction(const char *filename)
{
    int fd_rec;
    if ((fd_rec = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
    {
        perror("Failed to open log file for restoration");
        return -1;
    }
    if (_remove_corrupted_entries(fd_rec) < 0)
    {
        close(fd_rec);
        return -1;
    }
    close(fd_rec);
    return 0;
}

int open_log(const char *filename)
{
    if (_apply_correction(filename) < 0)
    {
        perror("Error fixing the file");
        return -1;
    }
    if ((log_fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) == -1)
    {
        perror("Failed to open log file");
        return -1;
    }
    return log_fd;
}

int restore_log(const char *filename)
{
    if (_apply_correction(filename) < 0)
    {
        perror("Error fixing the file");
        return -1;
    }
    int fd;
    if ((fd = open(filename, O_RDONLY)) == -1)
    {
        perror("Failed to open log file for reading");
        return -1;
    }
    return fd;
}

sighandler_t set_signal_ignore(int signal_id)
{
    return signal(signal_id, SIG_IGN);
}

void undo_signal_ignore(int signal_id, sighandler_t signal_handler)
{
    signal(signal_id, signal_handler);
}

int close_log()
{
    if (log_fd == -1)
    {
        perror("Log file is already closed");
        return -1;
    }
    if (close(log_fd) == -1)
    {
        perror("Failed to close log file");
        return -1;
    }
    log_fd = -1;
    return 0;
}

ssize_t _rollback_last_entry()
{
    if (log_fd == -1)
    {
        perror("Log file is not open");
        return -1;
    }
    _remove_corrupted_entries(log_fd);
    printf("Last entry was rollbacked\n");
    return 0;
}

int _write_entry(const uint8_t buffer[], const size_t entry_size)
{
    size_t bytes_left = entry_size;
    ssize_t bytes_written = 0;

    uint8_t *buffer_ptr = (uint8_t *)buffer;

    while (bytes_left > 0)
    {
        if ((bytes_written = write(log_fd, buffer_ptr, bytes_left)) <= 0)
        {
            if (errno == EINTR)
            {
                bytes_written = 0;
            }
            else
            {
                perror("Error writing to log");
                _rollback_last_entry();
                return -1;
            }
        }
        buffer_ptr += bytes_written;
        bytes_left -= bytes_written;
    }
    if (fsync(log_fd) < 0)
    {
        perror("Error writing to log");
        _rollback_last_entry();
        return -1;
    }
    return 0;
}

int _read_entry(int fd, uint8_t buffer[], size_t buffer_size)
{
    size_t bytes_left = buffer_size;
    ssize_t bytes_read = 0;

    while (bytes_left > 0)
    {
        if ((bytes_read = read(fd, buffer, bytes_left)) < 0)
        {
            if (errno == EINTR)
            {
                bytes_read = 0;
            }
            else
            {
                perror("Error reading from log");
                return -1;
            }
        }
        bytes_left -= bytes_read;
        buffer += bytes_read;
    }
    return 0;
}

int log_to_file(log_entry *entry)
{
    if (log_fd == -1)
    {
        perror("Log file is not open");
        return -1;
    }
    uint8_t buffer[ENTRY_SIZE];
    encode_entry(entry, buffer);
    if (_write_entry(buffer, ENTRY_SIZE) < 0)
    {
        return -1;
    }
    return 0;
}

int read_log_entry(int fd, log_entry *entry)
{
    if (fd == -1)
    {
        perror("Log file is not ready for reading");
        return -1;
    }
    uint8_t buffer[ENTRY_SIZE];
    if (_read_entry(fd, buffer, ENTRY_SIZE) < 0)
    {
        return -1;
    }
    decode_entry(entry, buffer);
    uint32_t checksum = calculate_checksum(buffer);
    return !(checksum == entry->checksum);
}