#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <zlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <signal.h>
#include "../src/bytes.h"
#include "../src/entry.h"
#include "../src/file_log.h"

void roundtrip_test(uint64_t value)
{
    uint8_t buffer[8];
    put_u64_be(buffer, value);
    uint64_t retrieved_value = get_u64_be(buffer);
    assert(value == retrieved_value);
}

void roundtrip_entry_test()
{
    uint8_t buffer[ENTRY_SIZE];
    time_t now = time(NULL);
    uint64_t value = rand();
    log_entry entry = {.timestamp = now, .value = value};
    encode_entry(&entry, buffer);
    log_entry decoded;
    decode_entry(&decoded, buffer);
    assert(decoded.checksum == _calculate_checksum(buffer));
    assert(decoded.timestamp == entry.timestamp);
    assert(decoded.value == entry.value);
}

void log_to_file_test()
{
    log_entry entry = {.timestamp = time(NULL), .value = rand()};
    open_log("./log_file");
    assert(log_to_file(&entry) == 0);
    close_log();
}

void wrong_entry_will_be_discarded_test()
{
    sighandler_t prev_handler = set_signal_ignore(SIGXFSZ);
    log_entry entry = {.timestamp = time(NULL), .value = rand()};
    unlink("./log_file_limit");
    open_log("./log_file_limit");
   
    
    struct rlimit lim, prev;
    getrlimit(RLIMIT_FSIZE, &prev);
    getrlimit(RLIMIT_FSIZE, &lim);
    lim.rlim_cur = 50;
    if(setrlimit(RLIMIT_FSIZE, &lim) == -1) {
        perror("Failed to set file size limit");
    }

    assert(log_to_file(&entry) == 0);
    assert(log_to_file(&entry) == 0);
    assert(log_to_file(&entry) == -1);

    if(setrlimit(RLIMIT_FSIZE, &prev) == -1) {
        perror("Failed to restore file size limit");
    }
    undo_signal_ignore(SIGXFSZ, prev_handler);
    close_log();
}

int main(void)
{
    srand(time(NULL));
    roundtrip_test(0);
    roundtrip_test(1000);
    roundtrip_test(UINT64_MAX);
    roundtrip_entry_test();
    log_to_file_test();
    wrong_entry_will_be_discarded_test();
    return 0;
}
