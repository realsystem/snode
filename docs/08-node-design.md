# SNODE node design v0.2 (variant B, battery) — draft

Status: draft for the bench POC (Phase 1–2). Every value is UNVERIFIED until bench tests T2, T3 and T4b pass.
v0.2 (round 25): probes on the powered bus O2 (design log D35, D38, D39); protocol in `10-bus-protocol.md`.
Sources: design log D11, D22, D23, D29, D31, D33 and research notes S25, S26, S52–S55, S58, S75.
Firmware: `firmware/node/` (node), `firmware/components/rv3028/` (RTC), `firmware/receiver/snode-receiver-poc.yaml` (POC receiver).

## 1. Block diagram

```
 3x L91 AA ──[Q1 reverse-polarity P-FET]──┬──────────────────────────────┐ PACK (3.6–5.4 V)
                                          │                              │
                                  [U1 TPS7A0233 LDO]             [U2 TPS22917L load switch]
                                          │ VRTC 3.3 V, always on        │ VSW (switched pack)
                                          │                              ├──[R1/R2 100k/68k]──> A1 (VBAT)
                           ┌──────────────┤                              │
                     [U3 RV-3028-C7]   R3 1M pull-up                 [D1 Schottky]
                      INT (open drain) ──┴──> ON (active low) <──┐       │
                           │  I2C 0x52        ^                  │   XIAO 5V pin (+C1 220 µF)
                           │                  │ [SW1 wake button]│       │
                           │                  └── GND            │   [XIAO ESP32C3, u.FL antenna]
                           │                     [Q2 N-FET] <── HOLD (D2)      │ 3V3 out
                           │                                                   │
   I2C (D4 SDA / D5 SCL, 4.7k pull-ups to 3V3) ── U3 RTC
   PACK ──[Q_EN P-FET]── BUS_HI ──┬──[Q_MK_A P-FET ‖ D_SP_A diode]── row A bus + ──> 10 probes ──> row A bus − ──[R_SH_A 22 Ω]── GND
        BUS_EN (D3)              └──[Q_MK_B P-FET ‖ D_SP_B diode]── row B bus + ──> 10 probes ──> row B bus − ──[R_SH_B 22 Ω]── GND
   TX_A (D6) / TX_B (D7) ──> N-FET ──> Q_MK gate (mark = on, space = off: line drops one diode)
   R_SH_A / R_SH_B ──> TLV7031 comparator ──> RX_A (D0) / RX_B (D8)       DS18B20 (soil) ── D10 (1-Wire GPIO)
```

## 2. How a wake works
1. At the alarm time the RV-3028 pulls INT low. INT is wired to the load switch ON pin (active low), so VSW turns on and the XIAO boots.
2. Firmware sets HOLD (D2 high → Q2 pulls ON low). Power now stays on whatever the RTC does.
3. BUS_EN on: both row buses get power, and every probe boots and measures (about 100 ms). The node starts the DS18B20 conversion, waits 150 ms, then reads the 10 probes of each row over its own UART (both rows in parallel, about 0.25 s; protocol v1, `10-bus-protocol.md`). BUS_EN off, then the pack voltage is read.
4. The node sends one ESP-NOW report (packet v1, section 6) to the receiver. It tries 3 times on the saved channel, then channels 1–11 once each (channel follow, D31), and saves the channel that worked.
5. The receiver replies with UTC time and the next wake time. The node corrects its RTC if it is more than 2 s off, then sends any stored unsent reports (up to 4).
6. The node sets the next alarm and clears the alarm flag (INT released), saves its state to flash, then releases HOLD. Power is cut.
- No reply: the node keeps a 12 h rhythm from its last schedule. No valid time (first start or after a battery swap): it retries every hour until a receiver answers.
- Wake button: hold for 2 s. Install mode: the node searches each bus for probe serials (DISCOVER) and sends one install packet per bus (HA event `esphome.snode_install`), then a report every 10 s for 2 minutes (flag bit 2), so readings can be checked in HA while installing.
- Watchdog: power is cut after 25 s, or 3 minutes in install mode, whatever the firmware is doing. If power is still on 3 s after HOLD is released (a fault), the node falls back to ESP32 deep sleep for 1 h.

## 3. Schematic (netlist form)

| Ref | Part | Pins / connection | Notes | Source |
|---|---|---|---|---|
| BT1 | 3x Energizer L91 AA holder | + → Q1, − → GND | 3.6–5.4 V | S15 |
| Q1 | P-MOSFET, logic level, VDS ≥ 12 V, RDS(on) ≤ 100 mΩ | drain = BT1+, source = PACK, gate = GND | Reverse-polarity protection, no quiescent drain | UNVERIFIED part choice |
| U1 | TI TPS7A0233 (3.3 V) | IN = PACK, OUT = VRTC, EN = IN | 25 nA; 1 µF in/out | S53 |
| U2 | TI TPS22917L | VIN = PACK, VOUT = VSW, ON = net ON | Active-low enable; 10 nA typ off | S54 |
| R3 | 1 MΩ | ON → VRTC | Keeps U2 off; 3.3 µA only while awake | — |
| U3 | Micro Crystal RV-3028-C7 | VDD = VRTC, INT → ON, SDA/SCL → I2C, VBACKUP = GND (switchover disabled in firmware) | 0x52, 45 nA | S25 |
| SW1 | Momentary button | ON → GND | Wake / install mode | — |
| Q2 | N-MOSFET (2N7002 class) | drain = ON, source = GND, gate = D2 via 1 kΩ, 100 kΩ gate–GND | HOLD | — |
| R1, R2 | 100 kΩ / 68 kΩ | VSW → A1 → GND | Pack voltage; max 2.19 V at A1; drains only while awake | S55 (ADC1 pin) |
| D1 | Schottky, 1 A, ≤ 0.45 V at 0.5 A | anode = VSW, cathode = XIAO 5V | Seeed requires a diode into the 5V pin | S55 |
| C1 | 220 µF low-ESR + 10 µF | XIAO 5V → GND | Radio peaks | UNVERIFIED size (T2) |
| XIAO | Seeed XIAO ESP32C3 | D0 = RX_A, D1 = VBAT, D2 = HOLD, D3 = BUS_EN, D4 = SDA, D5 = SCL, D6 = TX_A, D7 = TX_B, D8 = RX_B, D10 = 1-Wire | D0/D8 are strapping pins: driven high at boot by the comparators (bus off, no current) | S26, S55 |
| R4, R5 | 4.7 kΩ | SDA, SCL → 3V3 | Unpowered when off: no drain | — |
| Q_EN | P-MOSFET, logic level, RDS(on) ≤ 100 mΩ at VGS −2.5 V | source = PACK, drain = BUS_HI; gate 100 kΩ to PACK, pulled low by an N-FET from BUS_EN (D3) | Powers both buses; off at boot | UNVERIFIED part choice |
| Q_MK_A, Q_MK_B | P-MOSFET as Q_EN | source = BUS_HI, drain = bus +; gate 100 kΩ to BUS_HI, pulled low by an N-FET (2N7002 class) whose gate is TX_A / TX_B (100 kΩ to GND) | Mark = FET on; space = FET off | — |
| D_SP_A, D_SP_B | Silicon diode (1N4148W class) | anode = BUS_HI, cathode = bus + (parallel to Q_MK) | Space level about 0.7 V below mark | — |
| R_SH_A, R_SH_B | 22 Ω, 1% | bus − → GND | Current sense; about 0.25 V at 11 mA | — |
| U4, U5 | TI TLV7031 comparator (push-pull, 1.6–6.5 V) | VCC = 3V3; IN− = shunt via 1 kΩ + 10 nF; IN+ = 110 mV from a 3V3 divider (1 kΩ / 29 kΩ); OUT → RX_A (D0) / RX_B (D8) | Output low while a probe sinks current (UART 0) | S93 |
| D2, D3 | TVS, 6.0 V working | bus + → bus − (each bus) | Long buried cable; pack max 5.4 V | — |
| J1, J2 | 2-pole terminal | bus A, bus B (to 16/2 runs in the riser) | Polarity-free probes (bridge in probe) | — |
| J3 | 3-pole terminal | DS18B20: 3V3, D10 (4.7 kΩ pull-up to 3V3), GND | Node soil-temperature sensor at probe depth | S13 |

Off-state draw: U3 45 nA + U1 25 nA + U2 ≤ 100 nA ≈ 0.17 µA max (unchanged from D23).

## 4. Pin map (XIAO ESP32C3), v0.2
| XIAO | GPIO | Use |
|---|---|---|
| D0 | GPIO2 | RX_A (UART, from comparator A); strapping pin, high at boot |
| D1 / A1 | GPIO3 | VBAT sense (ADC1) |
| D2 | GPIO4 | HOLD (output) |
| D3 | GPIO5 | BUS_EN (output) |
| D4 | GPIO6 | I2C SDA |
| D5 | GPIO7 | I2C SCL |
| D6 | GPIO21 | TX_A (UART) |
| D7 | GPIO20 | TX_B (UART) |
| D8 | GPIO8 | RX_B (UART, from comparator B); strapping pin, high at boot |
| D9 | GPIO9 | Free (BOOT button) |
| D10 | GPIO10 | 1-Wire, node DS18B20 |
- Logger on USB (USB Serial/JTAG), so both hardware UARTs are free for the buses.

## 5. Energy per wake (estimate, replaces the D-log line items)
| Phase | Time | Current | Charge |
|---|---|---|---|
| Boot (no Wi-Fi) | 0.5 s | 25 mA | 3.5 µAh |
| Bus session, 20 probes (worst case: all awake) | 1.0 s | 25 mA + 20 mA | 12.5 µAh |
| Send + wait for reply (radio on) | 0.4 s | 80 mA | 8.9 µAh |
| **Per wake** | **1.9 s** | | **≈ 25 µAh** |
- 2 wakes/day ≈ 0.05 mAh/day + off-state 0.004 mAh/day; with 2x margin about **0.11 mAh/day**, inside the 0.24 mAh/day budget. Measure in T2.
- With the O2 powered bus (design log D38): worst case 25 µAh per wake (all probes awake for a 1 s session), so the budget is unchanged.

## 6. Packet format v1 (node → receiver), little-endian
| Byte | Field |
|---|---|
| 0 | Type 0x01 (report) |
| 1 | Version 1 |
| 2 | Grove code, ASCII `F`, `B` or `S` |
| 3 | Node number (1–99) |
| 4–5 | Sequence number (missing numbers = lost reports) |
| 6–9 | RTC time, UTC epoch (0 = not valid) |
| 10 | Flags: bit 0 time invalid, bit 1 resent from backlog, bit 2 install mode |
| 11 | Channel used |
| 12–13 | Pack voltage, mV |
| 14–15 | Node soil temperature x100 (°C), 0x7FFF = missing |
| 16 | Probe count (20) |
| 17–56 | Raw count per slot, int16; slots A01–A10 then B01–B10; 0x7FFF = missing |

Receiver reply (broadcast): `0x81, grove, node number, UTC now (4 bytes), next wake UTC (4 bytes)`.

Install packet (install mode, one per bus): `0x02, version 1, grove, node number, bus 'A'/'B', count (bit 7 = more than 10 found), serials (4 bytes each, up to 10)`. Receiver → HA event `esphome.snode_install` with node, bus and serials (comma-separated hex).
Receiver → HA: event `esphome.snode_report` with node ID (e.g. `snode_f_01`), seq, flags, channel, battery_mv, soil_temp, raw (comma list, `na` = missing), rssi. Slot → tree ID mapping: `config/probes.yaml`. HA turns raw into % with `config/calibration.yaml` (see `06-ha-note.md`).

## 7. Variant A (solar) changes, not designed in detail yet
- Storage: 10 F supercapacitor charged by a BQ25570 (D28). RTC from the capacitor (1.1–5.5 V).
- The RTC then runs below 3.3 V, so the I2C lines need a level translator (2-channel, e.g. PCA9306 class) between the RTC and the XIAO, and the INT pull-up goes to the capacitor rail.
- VSW feeds a TPS61099 boost to 5 V into the XIAO 5V pin (D23).

## 8. Operating limits (node electronics, draft)
| Limit | Value | Set by |
|---|---|---|
| Temperature | -40 to 85 °C (rated parts); design point -27 °C | XIAO, RV-3028, TLV7031 ratings; L91 -40 to 60 °C |
| Pack voltage | 3.6–5.4 V | Low end: XIAO 3.3 V rail through the diode; high end: TPS22917 5.5 V max |
| Probes per node | 20 (10 per bus) + 1 DS18B20 | Packet format; bus tests |
| Awake time | ≤ 25 s per wake (≤ 3 min install mode) | Watchdog |
| Unsent reports kept | 4 (2 days at 2 reports/day) | Flash backlog size |

## 9. Bench tests for this design
- T2: off-state current ≤ 1 µA; wake on alarm; HOLD and power cut; watchdog cut; button wake.
- T3: report reaches HA through the POC receiver; time reply sets the RTC; channel follow after a forced channel change; backlog resend after the receiver is switched off for 2 wakes.
- T4b: bus reads with 10 hand-built probes per row on 10.5 m of 16/2 cable: line levels and shunt signal on a scope; 1,000 read cycles; discovery finds every probe.

## 10. Open items
- Done in v0.2: powered-bus driver, pin map, `snode_bus` ESPHome component, install-mode discovery.
- Part numbers for Q1, Q3, D1–D3 and C1 (generic specs above); pick at bench-build time with cited datasheets.
- ESPHome support used but not yet compiled: two UARTs on the ESP32-C3 with strapping-pin RX, the `snode_bus` and `rv3028` external components, `espnow` `wait_for_sent` and `set_channel_from_action`, globals with `std::array` restore.
- Channel scan time on the real node: the desk test scanned at 10 s per send (about 170 s for 9 channels). The real node must start from its saved channel, and a full scan (3 tries + channels 1–11) must fit the 25 s watchdog or be split over several wakes. UNVERIFIED (D46, T3).
- Node receive threshold vs idle probe current (all probes asleep vs awake) — T4b.
- Wi-Fi OTA updates for nodes (now: USB only).
