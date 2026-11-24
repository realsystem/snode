# SNODE probe design v0.1 — draft

Status: draft. Every value is UNVERIFIED until bench tests T4, T5 and T6 pass.
Builds on design log D17, D19, D22, D24, D25 (option C lead) and D20 (temperature).
Firmware: `firmware/probe/` (portable core host-tested; hardware layer and protocol layer to do).

## 1. What the probe does
At each node wake the bus powers up. The probe boots, measures the soil (RC charge time of a buried
electrode) and its chip temperature, stores the result, and returns it when the node asks.
It has no battery and no clock; it is powered only through the bus, a few seconds twice a day.

## 2. The bus question (design log D35, owner to choose)
The 1-Wire plan (D22) has a power problem found in this round:
- The DS2482's weak pull-up is 1000–1675 Ω, and at 3.3 V it reads a "low" only below 0.9 V (S78).
- While the node talks, every probe MCU on that bus is awake for at least part of each transaction
  (all devices listen to the ROM address). 10 MCUs at about 0.5–1 mA each need 5–10 mA, which a 1–1.7 kΩ
  pull-up can't supply. The probes would run on their reservoir capacitors and brown out on long reads.
- A stronger (lower-value) pull-up makes the probe's low level too high: the polarity bridge adds two diode drops,
  so the line low is about 0.5–0.8 V against a 0.9 V limit.

| | O1: 1-Wire (DS18B20 emulation), as D22 | O2: powered bus, M-Bus style (recommended) |
|---|---|---|
| Idea | Line idles high through a weak pull-up; master and probes pull it to 0 V | Line never drops to 0 V. Node → probe: voltage steps (high / about 1.2 V lower). Probe → node: the probe draws an extra ~10 mA, the node senses the current. M-Bus uses this principle for remotely powered meters (S79) |
| Probe power while talking | From the pull-up and a reservoir capacitor; marginal for 10 probes | Directly from the node's supply; probes always powered |
| Polarity bridge | Its diode drops eat into the 0.9 V low limit | No effect (signals are relative steps) |
| Node hardware | 2x DS2482-100 (about $3 each at 1 pc, S80) | 2 small MOSFETs + 2 diodes + shunt resistor + 1 comparator per bus; no DS2482 |
| Node firmware | Standard ESPHome `ds248x` + `dallas_temp` | Custom ESPHome component (UART-based; we already have one for the RTC) |
| Probe firmware | 1-Wire slave timing in software (µs-level, interrupt-driven) | UART-style frames at 2400 baud: simpler timing |
| Node soil DS18B20 | On a DS2482 bus | On its own GPIO (ESPHome `one_wire` GPIO; one sensor, short cable) |
| Risk | High: bench T4 may fail on power | Medium: new node component, but no power margin problem |
- **Recommendation: O2.** Winter reliability comes first, and O2 removes the power and threshold problems instead of
  working around them. Cost is about the same (DS2482s removed, a few cents of discretes added).
- The probe PCB below supports both: only one resistor value (R_SINK) and the firmware differ.

## 3. Schematic (netlist form)

| Ref | Part | Connection | Notes |
|---|---|---|---|
| J1 | Lead pads (2) | factory-soldered 40 cm 2-core lead → molded clamp on the 16/2 bus | D25 option C |
| BR1 | 2x BAT54S (dual Schottky) as a bridge | J1 → V_LINE+ / GND | Either polarity on the clamp |
| D1 | BAT54 | V_LINE+ → VDD | Isolates the reservoir from the line |
| C1 | 4.7 µF X7R | VDD → GND | Reservoir; rides through line steps |
| C2 | 100 nF | VDD → GND, at the MCU | Decoupling |
| U1 | PY32F002A (ESSOP10) | VDD, GND, pins below | 1.7–5.5 V, -40 to 85 °C, 2 comparators, unique UID, SWD (S81) |
| R_CHG | 1 MΩ 1% | CHG pin → E_SENSE | Electrode charge resistor |
| E_SENSE | Electrode (PCB copper) | → COMP1+ | Sensing pad, front layer, under solder mask |
| R_REF1, R_REF2 | 100 kΩ 1% each | VDD → REF → GND; REF → COMP1− | Ratiometric threshold 0.5 VDD |
| R_LS1, R_LS2 | 100 kΩ / 47 kΩ | V_LINE+ → LINE → GND | Line sense (LINE pin: COMP2+ against a VDD divider for O2, or EXTI for O1) |
| Q1 | N-MOSFET (2N7002 class) | drain = V_LINE+ via R_SINK, source = GND, gate = SINK pin, 100 kΩ gate–GND | O1: R_SINK = 0 Ω (pull line low). O2: R_SINK ≈ 330 Ω (≈ 10 mA current step) |
| TP1–TP4 | Test pads | VDD, GND, SWDIO, SWCLK | Programming before potting; pogo-pin jig |
- Pin assignment on the ESSOP10 package: to set from the PY32F002A datasheet (comparator inputs are fixed pins).

## 4. Measurement
- Electrode C (soil-dependent, about 10–200 pF, UNVERIFIED) charges through R_CHG = 1 MΩ; time to 0.5 VDD = 0.69 RC ≈ 7–140 µs.
  At 8 MHz that is about 55–1100 ticks per sample; at 24 MHz 3x more.
- Ratiometric: the threshold is a fixed fraction of VDD, so the result doesn't depend on bus voltage.
- 64 samples, sorted, 8 dropped at each end, mean of the rest (`measure.c`). Spread and timeout count are kept as a
  fault indicator. More than half timed out = failed (0x7FFF): open electrode, water ingress or broken trace.
- Chip temperature from the MCU's internal sensor (design log D20 option T2, free). Reported alongside raw.
- Fixed offsets (comparator delay, pad and lead capacitance) are removed by the per-probe calibration
  (`config/calibration.yaml`).

## 5. PCB (blade and head)
| Item | Value |
|---|---|
| Board | 2-layer FR4, 1.6 mm, about 100 x 20 mm; 45° pointed tip |
| Sensing zone | Electrode 65 x 10 mm on the front, starting 10 mm from the tip (covers about 5–15 cm depth when the head sits at grade) |
| Ground | Back-layer ground pour under the electrode area; 2 mm ground strips along both front edges |
| Solder mask | Over the whole blade (electrode insulated, no corrosion) |
| Edge sealing | FR4 cut edges take up water: dip the blade in conformal coating or epoxy after test. Required (UNVERIFIED which coating; soak test T5) |
| Head (top 25 mm) | MCU, passives, test pads, lead pads; potted (epoxy or polyurethane) inside the 3D-printed ASA cap (POC) |
| Lead exit | From the top of the head, away from the electrode face, with strain relief inside the potting |
| Label | Probe serial (= 32-bit serial in flash; O1: also the 64-bit ROM code) |
| Panel | JLCPCB-style panel with V-cut or tabs; assembly as in D19 |

## 6. Energy (per probe, per wake)
- Boot + 64 samples: about 20 ms at about 1 mA. Talking: about 50 ms at about 1 mA. Otherwise stop mode.
- About 0.07 mA·s per probe per wake; 20 probes ≈ 0.4 µAh per wake. Small next to the node's 26 µAh (08-node-design.md).

## 7. Cost
Unchanged from D19 (estimate about $0.45–1.30 per probe; MCU $0.0927 at 500+) plus about 6 passives (cents) and the
40 cm lead (price at POC). O2 removes the node's 2 DS2482s (about $6 per node at 1 pc).

## 8. Bench tests
- T4a: PY32 dev board + 1 MΩ + a sense pad on perfboard in pots of site soil: raw count vs moisture, noise (spread).
- T4b: 10 PY32 probes on 10.5 m of 16/2 cable with the chosen bus (O1 or O2): 1,000 read cycles, VDD at each probe on a scope during reads.
- T5: blade + clamp soak test (edge sealing, clamp contacts).
- T6: calibration in site soil at 5 °C and 25 °C; chip temperature against the node DS18B20.

## 9. Open items
- Bus choice O1/O2 (round 23).
- ESSOP10 pin assignment; internal temperature sensor calibration data (datasheet).
- Electrode geometry and R_CHG tuned with T4a data.
- Conformal coating / potting compound choice (T5).
- Programming and labeling jig for 600 probes (pogo pins on TP1–TP4).
