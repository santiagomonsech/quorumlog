#include <string.h>
#include <zlib.h>
#include "bytes.h"
#include "entry.h"
#include <stdio.h>

uint32_t _calculate_checksum(const uint8_t *buffer){
    uint64_t checksum = crc32(0L, buffer, PAYLOAD_SIZE);
    return (uint32_t)checksum;
}

int encode_entry(log_entry *entry, uint8_t *buffer){
    put_u64_be(buffer, entry->timestamp);
    put_u64_be(buffer + VALUE_OFFSET, entry->value);
    uint32_t checksum = _calculate_checksum(buffer);
    put_u32_be(buffer + CHECKSUM_OFFSET, checksum);
    return 0;
}

log_entry* decode_entry(log_entry *entry, uint8_t *buffer){
    entry->timestamp = get_u64_be(buffer);
    entry->value = get_u64_be(buffer + VALUE_OFFSET);
    entry->checksum = get_u32_be(buffer + CHECKSUM_OFFSET);
    return entry;
}