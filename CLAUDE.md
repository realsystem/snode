# CLAUDE.md — Farm monitoring projects (SNODE, UNODE)

This file holds the shared terms, IDs and working rules for every project in this
repo. Read it before working on any of them. If a term here conflicts with a
project doc, this file wins until it is updated.

## 0. This repo is the project memory

Chat history and attachments may not be available in a future session. This repo
is the only reliable memory.

- At the start of a session, read in order: this file, `docs/00-brief.md`,
  `docs/01-interview-log.md`, `docs/02-design-log.md`, `docs/03-research-notes.md`.
- After every interview round or decision, update `docs/01-interview-log.md`
  (questions and answers) and `docs/02-design-log.md` (decisions, numbers,
  open questions). Add new sources to `docs/03-research-notes.md`.
- Commit with a short message that says what changed and why.

## 1. Site context (VERIFIED, from owner)

- Location: not recorded in this repo (owner's request). Cold desert. Soil freezes in winter.
- Never write the site's town, state or coordinates in any file or commit in this repo.
- Visits about every 3 weeks. Nobody on site otherwise.
- Home Assistant (HA) on a Raspberry Pi 5 in the trailer. Starlink internet,
  Wi-Fi in the trailer. Victron devices report to HA over Bluetooth.
- Ecowitt GW3000 gateway (16 soil channels) with WH51 sensors, used alongside the DIY nodes.
- The trailer is in the middle of the 330 x 600 ft monitored area.

## 2. Terminology

| Term | Meaning | Status |
|---|---|---|
| **forest** | The whole plot. Everything in this repo lives inside one forest. | VERIFIED |
| **grove** | One block of 200 trees. Three groves: `front`, `back`, `side`. | VERIFIED |
| **grid** | Tree spacing: 1 m between trees in a row, 2 m between rows. | VERIFIED |
| **row** | A straight line of trees at 1 m spacing inside one grove. Each grove is 10 rows x 20 trees (about 19 m x 18 m). | VERIFIED |
| **tree** | One tree position on the grid. Each tree has one probe and one drip emitter. | VERIFIED |
| **probe** | One soil-moisture sensor at one tree, at one depth (15 cm max), at a fixed offset from that tree's emitter. | VERIFIED |
| **bus** | The 2-wire cable along a row that powers and connects that row's probes to the SNODE. | VERIFIED (architecture) |
| **drive lane** | A wider gap between clusters for vehicles; never inside a cluster. | PROPOSED |
| **node** | A field device that reports to HA. Two types: SNODE and UNODE. | VERIFIED |
| **SNODE** | Soil node: powered only at report times by an RTC; two variants, solar only or battery only (design log D6) that reads the probes of a group of trees and reports to HA. Monitoring only. | VERIFIED |
| **UNODE** | General monitoring node (ESP32-class, own battery and solar), designed in a separate project. | VERIFIED |
| **cluster** | The set of trees wired to one SNODE. Default: 2 adjacent rows x 10 trees = 20 trees. | PROPOSED |
| **receiver** | Always-powered ESP32 in the trailer that receives SNODE radio packets and passes them to HA. | PROPOSED |
| **zone** | An irrigation valve zone (OpenSprinkler). HA maps trees to zones. | PROPOSED |

## 3. IDs and naming

### Grove codes
| Grove | Code |
|---|---|
| front | `F` |
| back | `B` |
| side | `S` |

### Tree ID
`<grove><row 2 digits>-<tree 2 digits>`, for example `F03-17` = front grove, row 3, tree 17.

- Rows are numbered from 01, starting at the row nearest the trailer. (VERIFIED)
- Trees are numbered from 01, starting at the end of the row nearest the
  main path or access side. (VERIFIED)

### Node ID
- SNODE: `snode_<grove>_<2 digits>`, for example `snode_f_01`
- UNODE: `unode_<location or purpose>_<2 digits>`, for example `unode_trailer_01`

### HA entities (lowercase, underscores only)
- Moisture: `sensor.soil_moisture_f03_17`
- Raw reading (before calibration): `sensor.soil_raw_f03_17`
- Soil temperature (one per SNODE): `sensor.soil_temp_snode_f_01`
- Battery: `sensor.battery_snode_f_01`
- Frozen-soil flag: `binary_sensor.soil_frozen_snode_f_01`
- Online status: `binary_sensor.online_snode_f_01`

### Calibration
All calibration values (dry/wet raw counts per probe) live in one file:
`config/calibration.yaml`, keyed by tree ID. Never hard-code them in firmware.

## 4. Working rules for Claude in this repo

- Use only verified data. If a fact is missing, ask. Don't assume.
- Mark every engineering claim **VERIFIED** (from owner or a cited source) or
  **UNVERIFIED** (needs a test or the owner's confirmation).
- Cite sources: manufacturer datasheets and manuals first, then tested
  comparisons. Give links. Record them in `docs/03-research-notes.md`.
- Ask short batches of questions (max 4), show options with trade-offs, then
  recommend one and say why.
- SNODEs are monitoring only. HA makes all irrigation decisions.
- Winter operation is the top priority.
- Prices go in the BOM only with a cited source and date.
- Plain, concise language. No marketing words.

## 5. Shared design constraints

- Must run unattended for at least 3 weeks between visits, through winter.
  Report interval: every 12 h, at 05:00 and 17:00 (VERIFIED).
  Design minimum temperature: -27 °C (-17 °F). Record low at the nearest NWS station, used as a proxy. UNVERIFIED for the site.
- Capacitive moisture readings are invalid in frozen soil. HA must block
  irrigation and flag "frozen" when soil temperature is 0.5 °C or below.
- LiFePO4 cells must not be charged below 0 °C. Chargers must have a cold cut-off.
- Burrowing rodents are present. Field cables must allow optional conduit.
- Rabbits are present: cables between the box and the ground must always be in a conduit riser.
- Drive lanes must fit a full-size pickup (Toyota Tundra), about 3.5 m between tree rows; no cable may cross a drive lane.
- Field installation needs no soldering and no bench prep: a spool of wire and the probes only.
  Connection cost per probe must be close to zero (built-in clamp); splice caps are a user option, not the design.
- Box-to-ground riser: 1/2 in standard PVC conduit.
- Firmware: ESPHome (nodes and receiver).
- Every design must state its operating limits (temperature, days without sun,
  battery life, link distance, cable length), because the design may become a commercial product.
- No masts in the field. Node antennas must be short and nearly invisible.
  Only the receiver at the trailer may have a tall antenna.

## 6. Repo layout

| Path | Contents |
|---|---|
| `CLAUDE.md` | This file: terms, IDs, rules |
| `docs/00-brief.md` | Owner's original design brief (verbatim) |
| `docs/01-interview-log.md` | Every question round and the owner's answers, dated |
| `docs/02-design-log.md` | Current decisions, power budget, costs, open questions |
| `docs/03-research-notes.md` | Findings with source links |
| `config/calibration.yaml` | Per-probe calibration values (empty until POC) |
| UNODE | Separate project. Link here when available. |
