# Arquitectura — quorumlog

Documento vivo. Se actualiza a medida que se toman decisiones.

---

## Visión del sistema

Sistema distribuido construido en 5 hitos, integrando Track A (embedded/systems)
y Track B (distributed). Arranca como un solo proceso que persiste un
append-only log local (Hito 1), gana comunicación por red y replicación
primario-secundario (Hito 2), gana consenso vía leader election Raft-style
en un cluster de 3 nodos (Hito 3), reemplaza el log de juguete por un storage
engine real — B-tree + WAL del proyecto `database` (Hito 4), y termina con
observabilidad básica y contenedorización opcional (Hito 5).

El nombre resume el arco: empieza siendo un log, termina siendo
quorum-governed.

---

## Diagrama de componentes (planeado — Hito 1)

```
┌──────────────────┐
│ sensor simulado   │ ← número random generado en código, sin hardware
└─────────┬─────────┘
          │ lectura
          ▼
┌──────────────────┐
│ log writer        │ ← formatea la entry, hace append al archivo binario
└─────────┬─────────┘
          │ write()/fsync()
          ▼
┌──────────────────┐
│ archivo binario   │ ← append-only log, formato de entry TBD
└──────────────────┘
```

## Diagrama de arco completo (referencia, no implementar aún)

```
Hito 1        Hito 2                Hito 3                  Hito 4                Hito 5
[1 proceso] → [primario/secundario] → [3 nodos + elección   → [B-tree + WAL real] → [métricas +
 log local      sobre TCP               de líder, Raft]         (proyecto database)   HTTP endpoint,
                                                                                        container opcional]
```

---

## Interfaces clave

_Pendiente — se completa cuando exista la primera firma de función real (Hito 1)._

---

## Decisiones de diseño

_Ninguna todavía. Formato para cuando se tome la primera:_

### [FECHA] — [Título de la decisión]

**Alternativas consideradas:**
- [opción A]: [pros / contras]
- [opción B]: [pros / contras]

**Decisión:** [opción elegida]
**Razón:** [por qué]

---

## Invariantes

_Pendiente hasta que exista una primera implementación real (Hito 1)._

---

## Pendientes / preguntas abiertas

- [ ] Formato binario del entry (Hito 1)
- [ ] Política de fsync (Hito 1)
- [ ] Protocolo de mensajes sobre TCP (Hito 2, no adelantar)
- [ ] Simplificaciones de Raft a tomar para el leader election de juguete (Hito 3, no adelantar)
