# SNODE ↔ UNODE compatibility note

Status: draft. Owner decision (round 29, design log D45): SNODE and UNODE stay separate projects,
but must stay compatible. UNODE repo: https://github.com/realsystem/unode (its copy of this note:
`docs/snode-compat.md`). This file is the reference copy; change both in the same change set.

## 1. Independence
- SNODE never depends on UNODE (already D30, option E6: a UNODE may host the receiver, as an option only).
- UNODE never requires the SNODE receiver. It may use it when it exists.
- Hardware is not shared: the owner's ESP32-C3 order is for SNODE only; UNODE uses its own boards.

## 2. Shared radio path
- A UNODE may report over ESP-NOW to the SNODE receiver (N2), using the same channel follow (D31).
- UNODE pump node is about 90 m from the trailer; SNODE's worst case is 105 m (R6). Whether it
  reaches is UNVERIFIED: use the T9/T12 walk-test method at the UNODE pump node.
- The receiver has no UNODE handling yet. Forwarding UNODE packets to HA is future receiver work.

## 3. Packet namespace (byte 0 = type, byte 1 = version)

| Type | Direction | Owner | Meaning |
|---|---|---|---|
| 0x01 | node → receiver | SNODE | Report (`08-node-design.md` §6) |
| 0x02 | node → receiver | SNODE | Install list |
| 0x81 | receiver → node | SNODE | Time + next wake reply |
| 0x10–0x1F | node → receiver | UNODE | Reserved |
| 0x90–0x9F | receiver → node | UNODE | Reserved |
| others | — | — | Unassigned; ask before use |

- The POC receiver drops packets that are not type 0x01 or 0x02 (`firmware/receiver/snode-receiver-poc.yaml`):
  VERIFIED by reading the code, not yet by a test. So UNODE packets cannot be mistaken for SNODE data.

## 4. HA and IDs
- Node IDs: `snode_<grove>_<nn>` and `unode_<purpose>_<nn>` (CLAUDE.md §3), e.g. `unode_gen_01`, `unode_pump_01`.
- HA events: SNODE uses `esphome.snode_report` and `esphome.snode_install`. `esphome.unode_report` is reserved for UNODE.
- Entity pattern for UNODE in iteration 2: `sensor.battery_unode_<id>`, `binary_sensor.online_unode_<id>`.
  UNODE iteration 1 is MQTT over Wi-Fi with discovery names; aligning them is a UNODE open item.

## 5. Power variant terms
- Both projects use the same words: **solar only** (variant A: panel + supercapacitor, no battery) and
  **battery only** (variant B: primary cells, no charging).
- SNODE: both variants (D6).
- UNODE: `unode_pump` = solar only (owner, round 29). `unode_gen` = open: it must still monitor the
  generator's starter battery while the generator is off; battery only with timed wakes is under review
  in the UNODE repo.

## 6. Firmware
- Both use ESPHome. External components here (e.g. `components/rv3028/`) may be reused by UNODE.
- A change to the packet type ranges, event names or channel follow updates this file and the UNODE copy together.
