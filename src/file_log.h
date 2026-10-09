#ifndef FILE_LOG_H
#define FILE_LOG_H

#include "entry.h"

typedef void (*sighandler_t)(int);

sighandler_t set_signal_ignore(int);
void undo_signal_ignore(int, sighandler_t);
int open_log(const char*);
int restore_log(const char*);
int close_log();
int log_to_file(log_entry *entry);
int read_log_entry(int fd, log_entry* entry);

#endif // FILE_LOG_H