#ifndef FILE_LOG_H
#define FILE_LOG_H

#include "entry.h"

int open_log(const char* filename);
int close_log();
int log_to_file(log_entry *entry);

#endif // FILE_LOG_H