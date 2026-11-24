# SNODE bus protocol v1 (powered bus O2) — draft

Shared by the node (`firmware/components/snode_bus/`) and the probes (`firmware/probe/`).
Design log D35, D38, D40. Everything here is UNVERIFIED until bench test T4b.

## 1. Physical layer
| Item | Value |
|---|---|
| Wires | 2 (16/2 landscape wire), probes polarity-free (bridge in each probe) |
| Node → probes | Voltage steps. Mark (idle, logic 1) = pack voltage (3.6–5.4 V); space (logic 0) = about 0.7 V lower (one silicon diode in series) |
| Probes → node | Current steps. Logic 0 = probe sinks about +10 mA (R_SINK ≈ 330 Ω); logic 1 = no extra current |
| Node receive threshold | About 5 mA above idle (22 Ω shunt, about 110 mV) |
| Probe receive | Probe compares its rectified line voltage with its own held supply (matched dividers): line above supply = mark, below = space |
| Framing | UART 9600 baud, 8 data bits, no parity, 1 stop bit, LSB first; half duplex |
| Power | The bus is powered for the whole session (about 0.5–1 s twice a day); probes measure at power-up |

## 2. Timing
| Item | Value |
|---|---|
| Probe boot + measurement after power-up | ≤ 100 ms; the node waits 150 ms before the first frame |
| Probe reply start | 2–5 ms after the stop bit of the node's frame |
| Node guard time | Node ignores receive for 1 ms after its own frame (edge currents) |
| Node reply timeout | 25 ms; up to 2 retries |
| Read of 10 probes | About 25 ms each, about 0.25 s per bus; both buses run in parallel |
| Probe sleep | Between frames, stop mode; wakes on a line edge. Non-addressed probes go back to sleep after the node's frame |

## 3. Frames
All multi-byte fields little-endian. CRC = Dallas/Maxim CRC-8 (0x8C reflected), the same function as `firmware/probe/src/crc8.c`.

### Node → probe
`A5 | CMD | ADDR (4) | ARG (0–1) | CRC` — CRC over CMD..ARG (not the sync byte).

| CMD | Name | ADDR | ARG | Reply |
|---|---|---|---|---|
| 0x01 | READ | Probe serial | — | READ reply |
| 0x02 | DISCOVER | Prefix | Bit count n (0–32) | Every probe whose lowest n serial bits equal the prefix sends one byte 0x00 |
| 0x03 | MEASURE | 0xFFFFFFFF (all) | — | None; probes measure again |

### Probe → node (READ reply, 15 bytes)
`5A | 81 | SERIAL (4) | RAW (2) | SPREAD (2) | CHIP_T (2) | TIMEOUTS (1) | STATUS (1) | CRC`
- RAW: mean charge-time count; 0x7FFF = measurement failed.
- SPREAD: max − min of the kept samples. CHIP_T: chip temperature °C x 100 (INT16_MIN = none).
- TIMEOUTS: samples that timed out. STATUS: bit 0 = measured at this power-up; other bits reserved (0).
- CRC over bytes 81..STATUS.

### DISCOVER (install)
- Depth-first search over the 32 serial bits, lowest bit first. The node asks "is any probe's serial ending in these n bits there?"
  Several probes answering at once is fine: their currents add, and the node only needs "any byte arrived".
- At n = 32 the prefix is a full serial: one probe found.
- Time: about 1–2 s per probe; 10 probes per bus in about 10–20 s (both buses in parallel).

## 4. Serials
- 32-bit probe serial, written into flash when the probe is programmed and printed on its label (09-probe-design.md).
- 0 and 0xFFFFFFFF are not valid serials.
- Slot order on a bus = position along the row; set in the node file and in `config/probes.yaml`.

## 5. Versioning
- This is v1. A future version keeps the sync bytes and the READ command and adds new CMD values only.
