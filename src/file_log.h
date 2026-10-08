#ifndef FILE_LOG_H
#define FILE_LOG_H

#include "entry.h"

typedef void (*sighandler_t)(int);

sighandler_t set_signal_ignore(int signal_id);
void undo_signal_ignore(int signal_id, sighandler_t signal_handler);
int open_log(const char* filename);
int close_log();
int log_to_file(log_entry *entry);

#endif // FILE_LOG_H