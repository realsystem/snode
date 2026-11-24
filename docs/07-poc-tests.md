# SNODE POC tests (short checklist)

Full plan with setups and reasoning: `05-poc-plan.md`. This page is what to do, what to record, and what counts as a pass.
Record results in `01-interview-log.md` (dated) and update `02-design-log.md`.

## Site visit — Dec 11 (T11, parts on hand)

| # | Do | Record | Why |
|---|---|---|---|
| 1 | Dig one 7 in trench, about 1 m long, along a tree row | Rocks or hardpan? Time taken; tool used | Is the trench install (D25 option C) practical? |
| 2 | Take 2 soil samples from 0–15 cm: about 1 L each, in sealed bags | Grove and tree ID | Bench tests T1 and T6 need site soil |
| 3 | Soil temperature at 6 in (any probe thermometer), morning and afternoon | °C and time | First real check of the frozen-soil threshold |
| 4 | Early Wi-Fi check: phone Wi-Fi analyzer, or the ESP8266 walk-test firmware on a power bank, held 0.3 m above ground at the farthest corner of each grove | RSSI (dBm) per grove; ESP8266 connect time if used | Early view of direct Wi-Fi (N1) before T9 |
| 5 | Photo of the back of the Starlink router; Starlink app → router settings | Router generation; free LAN port?; any 2.4 GHz channel setting? | Network setup (D30 E1/E3) |
| 6 | Measure the emitter-to-probe offset you want; note the cable side of the row | cm; side | Probe lead length |

Pass: items 1–6 recorded. No hardware is left on site.

## Bench (before the field cluster)

| Test | What | Pass |
|---|---|---|
| T1 | Old sensor drift in site soil, 1 week | Drift < 2% of span/week |
| T2 | Node off-current; wake times | ≤ 1 µA; 05:00/17:00 ± 2 s for 7 days |
| T3 | Node → (receiver) → HA data path, both variants; Ethernet receiver and Wi-Fi receiver | ≥ 99% of 14 reports; correct entity names; offline alert at 25 h |
| T4 | Bus: 2 x 10.5 m runs, 20 probes on leads with clamps, 1,000 reads | ≥ 99.9% reads |
| T5 | Clamp soak in wet soil (runs for months) | < 1 Ω change in 3 months |
| T6 | Calibration: dry/wet at 5 °C and 25 °C | Within ±2% after correction |
| T7 | Freezer to -20 °C | Frozen flag at ≤ 0.5 °C; node reports from cold |
| T8 | Side by side with a WH51, 3 wet/dry cycles | Peaks within 1 reading |

## Field (at least 3 weeks)

| Test | What | Pass |
|---|---|---|
| T9 | Walk test from the farthest corner, node antenna at 0.3 m: direct Wi-Fi (N1), then ESP-NOW (N2) | N1 is used if it delivers ≥ 98% of reports with a median awake time < 2 s; otherwise N2 |
| T10 | 2 nodes (variants A and B), 3 weeks, 2 reports a day | ≥ 98% delivered; alerts work; battery trend OK |
