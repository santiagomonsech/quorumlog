# quorumlog

A distributed append-only log built up in stages, in plain C. Starting as a single process appending fixed-size entries to a local file, ending as a 3-node cluster with leader election and a real on-disk storage engine.

## Why this name

By the last milestone, writes will only be durable once a **quorum** of nodes agrees on them, via a Raft-style leader election.

## Why I built this

This is an integration project that I'm building with the objective of learning how storage systems store and retrieve raw data, but also how nodes that replicate data reach consensus and stay consistent. The point isn't to end up with a production database or a scalable distributed system, but to build by hand every piece that a real distributed storage system depends on: durable local writes, a replication protocol, a consensus algorithm and a real storage engine underneath it.

## Milestones

- [x] **Milestone 1 — Persistent node:** C library for a basic append-only log of fixed-size, checksummed entries in a structured binary file (`0.1.0`).
  - [ ] Single C process that generates simulated readings (random numbers) and appends them through the library (planned for `0.2.0`).
- [ ] **Milestone 2 — Node that communicates:** two processes over TCP, primary writes and streams to a secondary that persists (primary-secondary replication). *(current)*
- [ ] **Milestone 3 — Cluster that elects a leader:** 3 nodes, Raft-style leader
  election.
- [ ] **Milestone 4 — Real storage engine:** swap the Milestone 1 append-only log for
  the B-tree + WAL from the separate `database` project; range queries by
  time become possible.
- [ ] **Milestone 5 — Observable system:** metrics (latency, throughput, errors) exposed over HTTP; optional containerization with the `container` project's runtime.

## Entry format

The log stores fixed-size entries with the following format:

```c
struct {
  uint64_t timestamp;
  uint64_t value;
  uint32_t checksum;
}
```

This gets translated into big-endian byte data in the following format:

`[timestamp (8 bytes) | value (8 bytes) | checksum (4 bytes)]` => 20 bytes total.

The reason for this decision was to limit the scope of the storage engine: since entries have a fixed size, variable-size entries are left out of the project, which is not intended to be a general-purpose storage system.

## Guarantees (Milestone 1)

Quorumlog allows appending one entry at a time, and it guarantees the following:
- Uses `fsync` for durability once all bytes of the entry have been written.
- When opening the log, the system will truncate the file to a multiple of 20 bytes.
- When retrieving an entry, it checks consistency by computing the CRC32 over the timestamp and value bytes and comparing it with the stored checksum; on a mismatch, the function returns `CRC_MISMATCH`.
- On a write operation, a partial write to disk will result in rolling back the write and leaving the file as it was before the write attempt.

## Requirements

To allow reproducibility and avoid dependency issues, I'm using Nix with flakes, so the only requirement is to have Nix installed.
For dependency resolution:

```bash 
nix develop
```

## How to run
Once the development environment is ready:

```bash
make           # build
make test      # build + run tests
make asan      # build with AddressSanitizer/UBSan
```

## Reference

- Bryant & O'Hallaron — *Computer Systems: A Programmer's Perspective*, 3e
- Kleppmann — *Designing Data-Intensive Applications*
- Ongaro & Ousterhout — *In Search of an Understandable Consensus Algorithm (Raft)*
