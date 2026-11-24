# SNODE POC plan (draft)

Goal: prove every UNVERIFIED design assumption on one prototype before building 30 nodes and 600 probes.
Order: bench first (no site visit needed), then one field cluster at the site.
All thresholds are PROPOSED until the owner accepts them.

## Phase S — site survey (next visit)
- T11: see `docs/07-poc-tests.md`.

## Phase 0 — sensor baseline (parts on hand)
- Setup: ESP8266 + two existing "Arduino" moisture sensors in two pots of site soil (one dry, one wet). Log every 10 min.
- Duration: 1 week.
- Pass: drift < 2% of the dry-to-wet raw span per week at constant moisture.
- Learn: is the v1.2 board type usable as the POC sensor front-end? (Check it for the 3 known faults first.)

## Phase 1 — node power and data path (bench)
- Short checklist for each visit: `docs/07-poc-tests.md`.
- Setup: two nodes, one per power variant: **B** (3x L91) and **A** (10 F supercap + TPS61099 + small panel). Both use XIAO ESP32C3 + RV-3028-C7 + load switch and ESPHome. A receiver (XIAO in the ASA box) on the fixed Wi-Fi channel, connected to HA.
- Tests:
  - T2: off-state current of the whole node ≤ 1 µA (µA meter). Wakes at 05:00 and 17:00 ±2 s over 7 days.
  - T3: readings arrive in HA with CLAUDE.md entity names; acknowledgement and retry work; unsent readings are resent; "offline" alert fires after 25 h with the node unplugged.
  - Measure the real energy per wake (current x time) and update the power budget.
  - Variant A only: run from the supercap with the panel covered; record days until the node pauses (target ≈ 6). Uncover; check it restarts, sends "hello" and gets the time.
  - Receiver: R-USB always on, and R-solar windowed listening (5 min around each report); measure receiver energy per day.
- Pass: off-current ≤ 1 µA; ≥ 99% of 14 scheduled reports delivered on the bench per node; variant A autonomy ≥ 5 days; cold-start recovery works.

## Phase 2 — bus and clamp (bench)
- Setup: 2 runs of 16/2 landscape wire, 10.5 m each, laid flat with one probe lead + clamp per probe as in the field (D25, option C). 20 bus-probe stand-ins (PY32F002A dev boards or first probe PCBs) on 3D-printed ASA clamp housings, spaced 1 m apart.
- Tests:
  - T4: 1,000 read cycles of all 20 probes; check the power phase holds up during measurement.
  - T5 (starts here, runs for months): 10 clamps on cable in a bucket of wet site soil. Bus powered on the real schedule (seconds, twice a day). Measure contact resistance monthly.
- Pass: ≥ 99.9% successful probe reads; clamp contact resistance change < 1 Ω over 3 months.

## Phase 3 — calibration and cold (bench)
- Setup: probes in site soil in pots; one WH51 in the same pot.
- Tests:
  - T6: raw counts at air-dry and saturated site soil; repeat at about 5 °C and 25 °C to get temperature coefficients. Compare T2 (MCU internal sensor) against the DS18B20.
  - T7: chest freezer to -20 °C with soil pots, a node and cells: frozen flag appears; node wakes and reports from cold; cell voltage under load.
  - T8: trend agreement with the WH51 through 3 wet-up/dry-down cycles.
- Pass: the same moisture at 5 °C and 25 °C reads within ±2% after correction; frozen flag at ≤ 0.5 °C; WH51 and probe peaks within 1 reading of each other.

## Phase 4 — field cluster (site, at least 3 weeks)
- Setup: two SNODEs (one variant A, one variant B) at the grove corner farthest from the trailer, each with up to 20 probes on one cluster (fewer is fine for the POC); receiver inside the trailer with the roof antenna; one WH51 about 10 cm from one probe at the same emitter offset.
- Tests:
  - T9: walk test before install, node antenna at 0.3 m, from the farthest corner: (a) N1 direct Wi-Fi to the trailer router: connect time, success rate, RSSI; (b) N2 ESP-NOW to the receiver: packet loss, RSSI. Pass for N1: ≥ 98% successful reports and median awake time < 2 s; otherwise use N2.
  - T10: run for at least 3 weeks, 2 reports a day.
- Pass: ≥ 98% of reports delivered; all alerts fire correctly when tested by hand; readings follow each irrigation event; battery voltage trend consistent with the 5-year plan.

## Outputs of the POC
- Final energy per wake and battery-life figure.
- Calibration and temperature coefficients for site soil, in `config/calibration.yaml`.
- Clamp and housing design frozen for molding quotes (D21).
- Radio margin and receiver antenna choice.
- Updated operating limits for the spec (`04-spec.md`).

## Bench POC parts list (prices checked 2026-11-24; sources in 03-research-notes.md)
Parts already on hand (NodeMCU, ESP32-CAM, multimeter, breadboard) are not listed. Quantities cover 2 bench nodes and 1 receiver.

### Phase 1 — node power, clock and radio (needed for any bus choice)
| Item | Qty | Unit price | Source | Line total |
|---|---|---|---|---|
| Seeed XIAO ESP32C3 (2 nodes + 1 receiver) | 3 | $4.99 (10+ price) | S83 | $14.97 |
| RV-3028-C7 breakout (Pimoroni PIM449) | 2 | £12.90 incl. VAT | S84 | £25.80 |
| TI TPS22917LDBVR load switch | 5 | $0.257 (5+) | S85 | $1.29 |
| TI TPS7A0233PDBVR LDO | 5 | not found (LCSC C2887324 listed, no price) | S85 | — |
| Energizer L91 AA | 6 | $2.29 | BOM (04-spec) | $13.74 |
| SOT-23-6 / SOT-23-5 adapter boards, MOSFETs, Schottky diodes, resistors, capacitors | — | not priced (generic stock) | — | — |

### Phase 2 — bus and probes
| Item | Qty | Unit price | Source | Line total |
|---|---|---|---|---|
| PY32F003 Startkit (official dev board; same core as the PY32F002A) | 1 | $16.94 | S86 | $16.94 |
| PY32F002AF15P6TU (TSSOP20) for hand-built probe prototypes | 10 | $0.1638 (5+) | S86 | $1.64 |
| DAPLink programmer (Universal-Solder 26730) | 1 | €4.73 (DigiKey) | S87 | €4.73 |
| DS18B20, TO-92 (probe stand-ins for the first bus test) | 21 | €1.14 (10–49) | S88 | €23.94 |
| DS18B20 waterproof (node soil sensor) | 2 | $9.95 | S88 | $19.90 |
| Only with bus O1: DS2482S-100 (SOIC-8) + SOIC-8 adapters | 4 | $3.05 (1 pc) | S80 | $12.20 |
| Only with bus O2: comparator (LMV331 class), shunt resistors | 2 | not priced yet | — | — |
| 16/2 landscape wire (bench runs, 2 x 10.5 m) | from the field spool | — | 04-spec BOM | — |

Cited subtotal (Phase 1 + 2, without O1/O2 lines): $69.19 + £25.80 + €28.67.
