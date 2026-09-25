#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>
#include <stdint.h>

static const uint8_t BUFFER_OFFSET = 56;

uint8_t put_u64_be(uint8_t *, uint64_t);
uint8_t put_u32_be(uint8_t *, uint32_t);
uint64_t get_u64_be(const uint8_t *);
uint32_t get_u32_be(const uint8_t *);

#endif /* LOGGER_H */
