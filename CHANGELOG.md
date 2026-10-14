# Changelog

## [Unreleased]

## [0.1.0] - 2026-10-14
### Added

- Binary log entry format: 20 bytes per entry: timestamp (u64), value (u64) and `CRC32` (u32). All fields are big-endian.
- `open_log()` opens the log for appending, creating it with mode 0600. Before opening the file, it truncates the file to the latest multiple of 20. Still not checking for data integrity.
- `log_to_file` appends an entry to the end of the file, short writes and interruptions are retried, and to ensure durability the `fsync` syscall is called once the whole entry has been written.
- If an entry fails to be written, the log will be truncated to the last multiple of 20.
- `restore_log` opens the log file in read-only mode and returns the file descriptor for reading entries. It also truncates the log file if the size is not a multiple of 20.
- `read_log_entry` reads and verifies an entry, it returns 0 on success, `CRC_MISMATCH` if the stored `CRC32` does not match the one computed over the timestamp and value fields, `EOF_LOG` if the end of the log was reached, and `-1` on I/O error (with `errno` set).
- `close_log` only closes the log when using it in write mode.
