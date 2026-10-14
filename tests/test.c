#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <zlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../src/bytes.h"
#include "../src/entry.h"
#include "../src/file_log.h"

uint64_t calculate_random()
{
    return (rand() % (100 - 0 + 1)) + 0;
}

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
    uint64_t value = calculate_random();
    log_entry entry = {.timestamp = now, .value = value};
    encode_entry(&entry, buffer);
    log_entry decoded;
    decode_entry(&decoded, buffer);
    assert(decoded.checksum == calculate_checksum(buffer));
    assert(decoded.timestamp == entry.timestamp);
    assert(decoded.value == entry.value);
}

void log_to_file_test()
{
    log_entry entry = {.timestamp = time(NULL), .value = calculate_random()};
    open_log("./log_file");
    assert(log_to_file(&entry) == 0);
    close_log();
}

void wrong_entry_will_be_discarded_test()
{
    sighandler_t prev_handler = set_signal_ignore(SIGXFSZ);
    log_entry entry = {.timestamp = time(NULL), .value = calculate_random()};
    unlink("./log_file_limit");
    open_log("./log_file_limit");

    struct rlimit lim, prev;
    getrlimit(RLIMIT_FSIZE, &prev);
    getrlimit(RLIMIT_FSIZE, &lim);
    lim.rlim_cur = 50;
    if (setrlimit(RLIMIT_FSIZE, &lim) == -1)
    {
        perror("Failed to set file size limit");
    }

    assert(log_to_file(&entry) == 0);
    assert(log_to_file(&entry) == 0);
    assert(log_to_file(&entry) == -1);

    if (setrlimit(RLIMIT_FSIZE, &prev) == -1)
    {
        perror("Failed to restore file size limit");
    }
    undo_signal_ignore(SIGXFSZ, prev_handler);
    close_log();
}

void read_entry_test()
{
    log_entry entry;
    int fd = restore_log("log_file");
    printf("Restored log file descriptor: %d\n", fd);
    assert(read_log_entry(fd, &entry) == 0);

    printf("%ld, %ld\n", entry.timestamp, entry.value);
    close(fd);
}

void read_all_entries()
{
    log_entry entry;
    int fd = restore_log("log_file");
    printf("Restored log file descriptor: %d\n", fd);
    int read_status = 0;
    struct stat sb;

    if (fstat(fd, &sb) != 0)
    {
        perror("fstat");
        return;
    }
    printf("Log file size: %ld bytes\n", sb.st_size);

    while (read_status == 0)
    {
        read_status = read_log_entry(fd, &entry);
        if (read_status == EOF_LOG)
        {
            assert(sb.st_size == lseek(fd, 0, SEEK_CUR));
        }
        else
        {
            assert(read_status == 0);
        }
    }

    close(fd);
}

void read_corrupted_entry()
{
    unlink("./log_file_corrupted");
    log_entry entry = {.timestamp = time(NULL), .value = calculate_random()};
    int fd = open_log("log_file_corrupted");
    assert(log_to_file(&entry) == 0);
    close_log();

    int fd_corr = open("log_file_corrupted", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    printf("Corrupted log file descriptor: %d\n", fd_corr);
    if (fd_corr == -1)
    {
        perror("Failed to open corrupted log file");
        return;
    }
    uint8_t buffer[ENTRY_SIZE];
    encode_entry(&entry, buffer);
    buffer[3] ^= 0xFF;

    if (lseek(fd_corr, 0, SEEK_SET) == -1)
    {
        perror("Failed to seek to beginning of file");
        close(fd_corr);
        return;
    }

    fd = restore_log("log_file_corrupted");
    if (write(fd_corr, buffer, sizeof(buffer)) != sizeof(buffer))
    {
        perror("Failed to write corrupted entry");
    }

    log_entry corrupted_entry;
    int status = read_log_entry(fd, &corrupted_entry);
    assert(status == CRC_MISMATCH);

    close(fd);
    close(fd_corr);
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
    read_entry_test();
    read_all_entries();
    read_corrupted_entry();
    return 0;
}
