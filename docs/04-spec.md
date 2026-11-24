# SNODE spec summary (draft)

Status: design draft, 2026-11-24. Everything not marked VERIFIED needs the POC.
Details and reasoning: `02-design-log.md` (D-numbers). Sources: `03-research-notes.md` (S-numbers).

## 1. Requirements

| # | Requirement | Value | Status |
|---|---|---|---|
| R1 | Trees | 600 (3 groves x 200), one probe per tree | VERIFIED |
| R2 | Probe depth | 15 cm max, one depth, fixed offset from the tree's drip emitter | VERIFIED |
| R3 | Report schedule | 05:00 and 17:00 | VERIFIED |
| R4 | Unattended operation | At least 3 weeks; through winter | VERIFIED |
| R5 | Design low temperature | -27 °C (proxy station record) | UNVERIFIED for site |
| R6 | Radio range | 105 m worst case, no field masts; one tall antenna at the trailer only | VERIFIED (constraints) |
| R7 | Power | Two variants: A solar only (no battery), B battery only (no solar) | VERIFIED |
| R8 | Installation | No soldering, no bench prep; spool + probes; built-in clamp; ~zero connection cost | VERIFIED |
| R9 | Box-to-ground riser | 1/2 in PVC (rabbits) | VERIFIED |
| R10 | Drive lanes | Fit a Toyota Tundra (~3.5 m); no cable crosses a lane; site uses V1 | VERIFIED |
| R11 | Function | Monitoring only; HA controls irrigation (OpenSprinkler) | VERIFIED |
| R12 | Firmware | ESPHome | VERIFIED |
| R13 | Frozen soil | Readings marked invalid and irrigation blocked at ≤ 0.5 °C soil temperature | PROPOSED |
| R14 | Accuracy | ±5% moisture after site calibration, judged mainly on trends | VERIFIED |
| R16 | Irrigation controller | OpenSprinkler, 8 zones now; expander (+16 zones) later | VERIFIED |
| R17 | Bus burial | 6 in minimum cover (US irrigation practice) | VERIFIED (interpretation to confirm) |
| R18 | POC scope | Both power variants | VERIFIED |
| R19 | Receiver | Boxed; indoor or outdoor; optional solar | VERIFIED |
| R15 | Commercial readiness | Operating limits stated for every variant | VERIFIED |

## 2. Decisions

| # | Decision | Choice | Reason | Source |
|---|---|---|---|---|
| D1/D17 | Probe | Custom 2-wire bus probe: PCB blade as capacitive electrode, PY32F002A MCU, potted electronics. POC uses v1.2 analog boards. | Fits a 1/2 in riser; ~1 m cable/tree; near-zero connection cost | S1, S44, S42 |
| D2 | Probes per SNODE | 20 (2 rows x 10 trees) | Grove tiles into 10 clusters; 30 nodes | — |
| D3/D16 | Bus cable | 16/2 flat landscape wire (standard); 18/2 irrigation wire as user option | Clamp works on flat cable without prep | S41, S43 |
| D16 | Connection | Piercing clamp built into the probe housing | No separate connector | S40 |
| D4 | Radio | ESP-NOW star to one receiver; antenna inside the node box; receiver antenna on the trailer roof ~5 m | No masts; high end clears most of the Fresnel zone | S6, S22 |
| D5 | Node MCU | Seeed XIAO ESP32C3 | Small, includes an antenna, ESPHome support | S26, S39 |
| D6 | Power | Variant B recommended: 3x L91 AA, plan 5-year swap. Variant A: 10 F supercap + small panel, about 6 days without sun | Load about 0.24 mAh/day with margin | S15, S28 |
| D11 | Power gating | RV-3028-C7 RTC alarm drives a load switch; node is unpowered between readings | Removes sleep current; exact report times | S25 |
| D7/D20 | Temperature | T1: one DS18B20 per node corrects all its probes; soil temperature is reported to HA with the moisture values | Owner choice; $0 extra | S13, S47 |
| D22 | Bus protocol | Probes emulate DS18B20 (1-Wire); low-value pull-up powers the probes; ESPHome `one_wire` + `dallas_temp` read them | No custom ESPHome code for probes | S51, S52 |
| D23 | Node power | TPS7A02 always-on LDO for the RTC; TPS22917L load switch driven by the RTC; diode into the XIAO 5V pin. Variant A: 10 F supercap + TPS61099 boost | Off-state ≈ 0.17 µA max | S53, S54, S55, S59, S60 |
| D22 | Bus master | 2x DS2482-100 (one per row), PCTLZ-driven MOSFET for the power phase | ESPHome `ds248x` support; avoids bit-bang | S58 |
| D24 | Probe polarity | Bridge rectifier; clamp fits either way | Owner accepted | — |
| D24/D25 | Probe depth | Sensing zone about 5–15 cm, tip at 15 cm (6 in) | Owner: 6 in | — |
| D26 | Node enclosure (POC) | 3D-printed ASA, gasketed, riser socket, vent | Owner choice | S50 |
| D25 | Bus burial | 16/2 landscape wire at ≥ 6 in cover (US irrigation practice, ≤ 30 V); each probe joins it with its own factory lead and a molded clamp (option C) | Owner: US irrigation standard; owner chose C | S61 |
| D27 | Radio path | N1 direct Wi-Fi (test first) or N2 ESP-NOW receiver (fallback; commercial default) | Energy difference small; range/setup decides | S62, S63, S64 |
| D28 | Variant A charger | TI BQ25570 (cold start ≥ 600 mV, MPPT, 488 nA) | Supercap-friendly harvester | S65 |
| D30 | Receiver uplink | One firmware: Wi-Fi with channel follow (default), Ethernet, or fixed channel; examples E1–E7; UNODE optional | Owner: any ISP/router/LAN; owner site has no spare port | S62, S67–S69, S73 |
| D25b | Probe position | P1: 10 cm from the emitter, on the cable side; K2: cable 20–25 cm off the tree line; lead 40 cm (draft) | Owner choice | S71, S72 |
| D32 | POC spare boards | NodeMCU: walk test, drift logger, ESP-NOW sender; ESP32-CAM: ESP-NOW receiver | ESP8266 can't measure ESP-NOW RSSI | S75, S76 |
| D33 | Node design v0.1 | `08-node-design.md`: RTC-gated power with HOLD and watchdog, node-switched power phase, receiver-set schedule, packet v1 | Owner chose this step | S25, S52–S55, S58, S75 |
| D34 | Probe design v0.1 | `09-probe-design.md`: RC timing electrode, ratiometric comparator, 64-sample trimmed mean, one PCB for both bus options | Owner chose this step | S78–S82 |
| D35 | Bus electrical | O2 powered bus, M-Bus principle at about 5 V, discrete parts | Owner chose O2; 1-Wire pull-up can't power 10 awake probes | S78, S79 |
| D36 | Ready M-Bus chips | Reviewed, not used: 36 V bus, +$1,000–1,500, TSS721A -25 °C | Cost, burial voltage class, cold | S96, S97 |
| D37 | Probe MCU | Both kept: ATtiny202/402 (US, bench) and PY32F002A (LCSC); production choice after bench tests | Owner: noted | S81, S94 |
| D38 | O2 bus parameters | Mark = pack voltage, space ≈ 0.7 V lower, 9600 baud, +10 mA reply; energy unchanged (25 µAh/wake worst case) | Owner: OK if power estimate unchanged | S93, S94 |
| D39 | Node design v0.2 | Powered-bus driver per row (Q_MK + diode, shunt, TLV7031), UART per bus, `snode_bus` component, install discovery | Owner chose this step | S93 |
| D40 | Bus protocol v1 | `10-bus-protocol.md`: 9600 8N1, READ / DISCOVER / MEASURE, CRC-8 | Shared by node and probe | S79 |
| D31 | Channel follow | Node saves the last good channel; on a missed acknowledgement it scans channels 1–11 | Works with routers that change channel | S74 |
| D29 | RTC in ESPHome | Small external component for the RV-3028 | No built-in platform | S66 |
| D12 | Enclosure | UV-stabilized polycarbonate, IP68 class, on a wooden stake, box bottom 0.3 m above grade | Owner: wood and plastic are rabbit-safe | S29 |
| D13 | Riser | 1/2 in Schedule 40 PVC, 2 cables (35% fill), duct seal at the top | Rabbits | S35, S38 |
| D18 | Firmware/HA | ESPHome + ESP-NOW Packet Transport; raw values sent, calibration in HA from `config/calibration.yaml` | Calibration in one place | S39 |
| D21 | Housing manufacture | ASA 3D print now; aluminum-mold injection molding later | Owner choice | S49, S50 |

## 3. Operating limits (both variants unless stated)

| Limit | Value | Status |
|---|---|---|
| Temperature, electronics and cells | -40 to +60 °C (B) / -40 to +65 °C (A), parts ratings | Rated |
| Field cable | -20 °C class accepted for buried runs | Accepted |
| Days without sun | A: about 6 at design load; B: not applicable | UNVERIFIED |
| Battery life | B: plan 5 years | UNVERIFIED |
| Probes per node | 20 | Design |
| Bus run | About 10.5 m per row run; 20 m radius/weight reference limit | UNVERIFIED |
| Radio link | 105 m, node antenna 0.3 m, receiver antenna 5 m | UNVERIFIED |
| Snow depth | Box height − 0.1 m (0.2 m at standard height) | Design |
| Valid moisture | Soil > 0.5 °C | Design |
| SNODEs per receiver | 30 | UNVERIFIED |

## 4. Open items

1. Probe lead length final (40 cm draft; drip lateral position at next-visit check).
2. First compile of the POC firmware on the owner's PC.
3. Production probe MCU — after bench tests (D37).
4. Probe MCU comparator count (ATtiny202 has one) — round 26.
3. Probe lead + molded clamp price — POC.
- HA automation (zones, alerts, delivery): handled in the HA threads; see `06-ha-note.md`.
5. Bus protocol and electrical design (power phase, timing, drops per run).
6. Probe PCB, passives, potting and housing quotes.
7. Cheaper enclosure than the Hammond 1554 reference part.
8. Load switch, LDO, battery holder, riser fittings, vent, stake: select and price.
9. Variant A parts: panel, supercapacitor, boost converter, charge limiter: select and price.
10. Receiver roof antenna and coax: select and price.
11. RV-3028 support in ESPHome (may need a custom component).
12. Sharing parts or firmware with UNODE.

## 5. Test list

Full plan with pass/fail criteria: `05-poc-plan.md`.

| # | Test | Proves |
|---|---|---|
| T1 | v1.2 drift in site soil (ESP8266 logger) | Sensor stability; baseline |
| T2 | RTC power gating, off-state current | ≤ 1 µA off; correct wake times |
| T3 | ESPHome ESP-NOW node → receiver → HA; channel follow after a forced router channel change | Data path, entity naming, D31 |
| T4 | Bus bench test, 2 x 10.5 m runs, 20 drops | Bus reliability, power phase |
| T5 | Piercing clamp soak test | Contact life in wet soil |
| T6 | Calibration in site soil (dry/wet, 2 temperatures) | Raw→% mapping; temperature coefficients |
| T7 | Freezer test to -20 °C | Frozen flag; cold start; cell behavior |
| T8 | WH51 side-by-side | Trend agreement |
| T9 | Field walk test from the farthest corner, direct Wi-Fi (N1) and ESP-NOW (N2) | Which radio path; RSSI margin at 0.3 m node antenna |
| T10 | Field cluster run, at least 3 weeks | Delivery rate, alerts, battery trend |
| T12 | Optional ESP-NOW range test: NodeMCU sender, ESP32-CAM receiver on farm Wi-Fi | ESP-NOW RSSI and loss per spot; channel follow (D31) |
| T11 | Site survey (next visit): soil sample, trench test, early Wi-Fi RSSI, router check | Inputs for T1, T6, T9 and D30 |

## 6. BOM (cited parts only, prices seen 2026-11-24)

Variant B, 600 trees, 30 SNODEs, 1 receiver. Probe line counts only the cited MCU and assembly fees.

| Item | Part | Unit price | Source | Qty | Total |
|---|---|---|---|---|---|
| Node MCU | Seeed XIAO ESP32C3 (with antenna) | £3.93 | S26 | 30 | £117.90 |
| Receiver MCU | Seeed XIAO ESP32C3 | £3.93 | S26 | 1 | £3.93 |
| RTC | Micro Crystal RV-3028-C7 | $3.74 | S25 | 30 | $112.20 |
| Soil temperature | DS18B20 waterproof | $1.90 | S20 | 30 | $57.00 |
| Cells | Energizer L91 AA (3 per node) | $2.29 | S21 | 90 | $206.10 |
| Enclosure | Hammond 1554QA2GY | £40.42 | S29 | 30 | £1,212.60 |
| Bus cable | 16/2 landscape wire, 500 ft (2,264 ft with probe leads, option C) | $140.00 | S41 | 5 | $700.00 |
| Probe MCU | Puya PY32F002A | $0.0927 | S44 | 600 | $55.62 |
| Probe assembly | JLCPCB setup + SMT + feeders (≈$0.09/probe) | $0.09 | S46 | 600 | $54.00 |
| Bus master | ADI DS2482-100 (2 per node; 1k list price) | $1.18 | S58 | 60 | $70.80 |
| **Total (cited)** | | | | | **$1,255.72 + £1,334.43** |
| **Per tree (cited)** | | | | | **about $2.09 + £2.22** |

Variant A swap (per node): remove 3x L91 ($6.87), add a 10 F supercap (Eaton HV1030, $1.97 at 50, S59) + TPS61099 boost (price not yet cited) + panel (not yet selected).
The POC node enclosure is 3D-printed (D26); the Hammond box stays as the bought reference.

Not yet in the BOM (no cited price yet): probe PCB, passives, potting and housing; load switch, LDO, battery holder; riser conduit and fittings, duct seal, vent, wooden stake; receiver roof antenna and coax. Variant A replaces the cells with a supercapacitor, boost converter, charge limiter and panel.
