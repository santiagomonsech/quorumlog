#ifndef ENTRY_H
#define ENTRY_H

#include <stdint.h>

#define ENTRY_SIZE 20

typedef struct log_entry
{
    uint64_t timestamp;
    uint64_t value;
    uint32_t checksum;
} log_entry;

static const uint8_t PAYLOAD_SIZE = 16;
static const uint8_t VALUE_OFFSET = 8;
static const uint8_t CHECKSUM_OFFSET = 16;

uint32_t calculate_checksum(const uint8_t *buffer);
int encode_entry(log_entry *entry, uint8_t *buffer);
log_entry *decode_entry(log_entry *entry, uint8_t *buffer);

#endif // ENTRY_H