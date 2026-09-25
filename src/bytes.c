#include <time.h>
#include <zlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bytes.h"

typedef struct log_entry {
    uint64_t timestamp;
    uint64_t value;
    uint32_t checksum;
} log_entry;


uint8_t put_u64_be(uint8_t *buff, uint64_t val){
    for(int i = 0; i<8; i++){
        buff[i] = (val >> (BUFFER_OFFSET - i * 8)) & 0xFF;
    }
    return 8;
}

uint8_t put_u32_be(uint8_t *buff, uint32_t val){
    for(int i = 0; i<4; i++){
        buff[i] = (val >> (24 - i * 8)) & 0xFF;
    }
    return 4;
}

uint64_t get_u64_be(const uint8_t buff[]){
    uint64_t value = 0;
    for(int i = 0; i < 8; i++){
        value |= ((uint64_t)buff[i] << (BUFFER_OFFSET - i * 8));
    }
    return value;
}

uint32_t get_u32_be(const uint8_t buff[]){
    uint32_t value = 0;
    for(int i=0; i<4; i++){
        value |= (uint32_t)buff[i] << (24 - i * 8);
    }
    return value;
}