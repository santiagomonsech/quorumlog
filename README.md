# quorumlog

A distributed KV/log system built up in stages, in C — starting as a single
process appending to a local file, ending as a 3-node cluster with leader
election and a real on-disk storage engine.

---

## Why this name

The system starts as a plain append-only **log** on one node. By the last
milestone, writes are only durable once a **quorum** of nodes agrees on them,
via a Raft-style leader election. `quorumlog` names that arc: log first,
quorum-governed by the end. Nothing about the project is domain-flavored
(no sensors-as-a-product framing) — the "sensor" is just a stand-in data
source so there's always something to append.

---

## Why I built this

This is the integrator project for two tracks I'm building in parallel:
**embedded/systems** (C, storage, networking, low-level I/O) and
**distributed systems** (replication, consensus, consistency). Each milestone
here only unlocks once the matching theory has been covered on both sides —
the point isn't to end up with a production database, it's to have built,
by hand, every piece a real distributed storage system depends on: durable
local writes, a replication protocol, a consensus algorithm, and a real
storage engine underneath it.

---

## Milestones

- [ ] **Hito 1 — Nodo que persiste:** single C process, simulated sensor
  (random number, no hardware) appended to a structured binary file as a
  basic append-only log. *(current)*
- [ ] **Hito 2 — Nodo que comunica:** two processes over TCP, primary
  writes and streams to a secondary that persists — primary/secondary
  replication.
- [ ] **Hito 3 — Cluster que elige líder:** 3 nodes, Raft-style leader
  election — when the leader dies, the others elect a new one.
- [ ] **Hito 4 — Storage engine real:** swap the Hito 1 append-only log for
  the B-tree + WAL from the separate `database` project — range queries by
  time become possible.
- [ ] **Hito 5 — Sistema observable:** metrics (latency, throughput,
  errors) exposed over HTTP; optional containerization with the `container`
  project's runtime.

Full detail and prerequisites per milestone: `~/learning/roadmap/proyecto-integrador.md`.

---

## Estructura

```
quorumlog/
├── README.md            ← este archivo
├── CHANGELOG.md
├── CONTEXT.md           ← vocabulario del dominio (gitignored, no en el repo de quorumlog)
├── CLAUDE.md            ← instrucciones para Claude Code (gitignored)
├── src/                 ← código fuente
├── tests/               ← tests
├── notes/               ← notas de sesión (gitignored)
├── mistakes.md          ← errores conceptuales (@reviewer)
└── docs/
    └── architecture.md  ← diseño del sistema, decisiones tomadas
```

Note: unlike the mini-projects in the learning repo, `quorumlog` has its own
git history — `CLAUDE.md`, `CONTEXT.md`, and `notes/` are gitignored *within
this repo* since they're learning/AI scaffolding, not part of the deliverable.

---

## Cómo correr

No hay código todavía — arranca en Hito 1.

```bash
make           # build
make test      # build + run tests
make asan      # build con AddressSanitizer/UBSan
```

**Requirements:** gcc

---

## Comandos disponibles en este proyecto

| Comando | Descripción |
|---------|-------------|
| `/check` | Compila, corre tests, verifica con sanitizers |
| `/explore` | Explora el SO relacionado con el tema activo |
| `/next` | Próximo paso concreto del hito activo |
| `/recap` | Escribe notas de la sesión actual en notes/ |

## Agentes útiles para este proyecto

- `@debugger` — cuando algo explota, gdb/valgrind guiado
- `@reviewer` — para revisar el código en cualquier momento
- `@quiz` — para testear comprensión después de un hito
- `@explainer` — cuando un concepto no está claro

---

## Reference

- Bryant & O'Hallaron — *Computer Systems: A Programmer's Perspective*, 3e
- Kleppmann — *Designing Data-Intensive Applications*
- Ongaro & Ousterhout — *In Search of an Understandable Consensus Algorithm (Raft)*
