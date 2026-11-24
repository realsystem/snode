# SNODE design log (working draft)

Status: interview in progress. Not a final spec. Last updated 2026-11-24.
Terms and IDs: see `CLAUDE.md`. Sources: see `docs/03-research-notes.md` (S-numbers).

## Verified facts (from owner)
- Site: cold desert (location not recorded here). Soil freezes. Visits about every 3 weeks.
- HA on a Raspberry Pi 5 in the trailer, which is in the middle of a 330 x 600 ft area. Starlink. Wi-Fi in the trailer.
- Ecowitt GW3000 gateway with WH51 soil sensors, used separately from the SNODEs and for comparison.
- 3 groves (front, back, side) x 200 trees = 600 trees.
- Grid: 1 m between trees in a row, 2 m between rows. Each grove is 10 rows x 20 trees.
- One probe per tree, one depth, 6 in (15 cm) max. Trees are newly planted.
- Drip irrigation: one emitter per tree. Probes go at the same offset from the emitter at every tree.
- Burrowing rodents present. Conduit must be an option.
- Report interval: every 12 h, at about 05:00 and 17:00.
- Row/tree numbering as in CLAUDE.md: accepted.
- No masts. SNODE antennas must be short, small and close to invisible.
- Two power variants to be designed: **solar only (no battery)** and **battery only (no solar)**.
- Variant A storm gaps are acceptable: the site has enough sun. The operating limits must still be documented for a possible commercial version.
- Receiver antenna on the trailer roof (about 5 m) is acceptable. The trailer is standard fiberglass.
- SNODE enclosure: a box on a stake.
- Comparison sensors: 4x WH51 on one GW3000 (16 channels max). Keep these numbers.
- Every few rows must be drivable by car or tractor.
- Little snow at this site now, but the product must be designed for snow accumulation.
- Trenching and burial are the installer's job; the design gives instructions.
- Drip lines: surface at this site, but the design must work with surface or buried lines.
- Drive lanes must fit a Toyota Tundra.
- Snow provision: minimal.
- Rabbits present. Wooden stake and plastic box are fine; a conduit from the box to the ground is required.
- Splices: sealed direct-bury caps like those used for 24 VAC irrigation wiring.
- Drive lanes about 3.5 m wide: accepted.
- Installation: **no soldering, no bench prep.** The installer goes to the field with a spool and a bag of connectors bought at a home-improvement store.
- Riser: 1/2 in standard PVC conduit.
- Firmware: ESPHome is acceptable.
- Product architecture: 2-wire digital bus probes (custom PCB). Analog v1.2 probes are for the POC only.
- Connection cost must be close to zero. Standard irrigation caps may be a user option, but the design must use something simpler.
- Drive lanes: provide calculated variants; the user chooses.
- Cable rated to -20 °C is accepted for buried runs.
- Standard connection: piercing clamp built into the probe (accepted).
- Drive-lane variant for this site: **V1** (one lane per grove).
- Cable: 16/2 landscape wire is standard; 18/2 irrigation wire is a user option (backup).
- Housings: 3D-printed in house for the POC and small batches; molded housings later (sources noted in D21).
- Order of work: finish the design first, then the POC. No shopping list yet.
- Temperature compensation: **T1** (one DS18B20 per SNODE). Soil temperature is reported to HA together with soil moisture.
- Irrigation controller: standard OpenSprinkler (8 zones) at first; with a zone expander later.
- HA automation is handled in separate threads; this repo keeps a hand-off note (`docs/06-ha-note.md`). Focus here: hardware.
- Accuracy target: ±5% after site calibration, judged mainly on trends.
- Probe polarity: bridge rectifier (polarity-free clamp) accepted.
- Probe depth: bottom of the sensing zone at 6 in (15 cm) (confirmed).
- Node enclosure: 3D-printed in house for the POC.
- POC tools on hand: µA meter, enclosed 3D printer, soldering station, chest freezer.
- Bus cable: keep 16/2 landscape wire; bury it to the US irrigation-wire standard (6 in cover for ≤ 30 V). Confirmed.
- Both power variants (A solar, B battery) go through the POC.
- Receiver: in a box in all cases; must support indoor and outdoor mounting, with optional solar power. Owner asked why a receiver is needed at all (answered in D27; decided by POC test).
- Close all remaining open items before the POC.
- HA decides irrigation (probably OpenSprinkler). SNODEs only report.
- Must work through winter (owner: "winter is most important").
- Lowest cost. Custom PCB allowed after the POC. Existing ESP8266 boards and "Arduino" moisture sensors are for the POC only.

## Architecture common to both variants

```
 [RTC RV-3028-C7]--alarm-->[load switch]--> MCU + probes + mux + DS18B20 (powered only while awake)
        ^ always on (45 nA)        ^
        |                          |
   energy store: battery (variant B) or supercapacitor (variant A)
```
- The node is **fully unpowered between readings**. Only the RTC stays on. This removes the ESP32 board's sleep current (43 µA), which was 92% of the earlier load.
- The RTC alarm switches power on at 05:00 and 17:00. The MCU reads the probes, sends one ESP-NOW packet, sets the next alarm and clears the alarm, which cuts its own power.
- The receiver's reply carries HA's time; the node corrects the RTC if it is off by more than 2 s. This replaces the drift workaround in the old D8.

## Proposed decisions (all UNVERIFIED until POC)
| # | Decision | Choice | Reason | Sources |
|---|---|---|---|---|
| D1 | Probe | Capacitive v1.2 board, checked for the 3 known faults, electronics end potted, tip at 15 cm max, top about 5 cm below grade | Cheapest; no corrosion; board is about 99 mm long, so it fits the 15 cm depth limit. Custom digital probe (A3) is the cost-down path after the POC. | S1–S4, S8 |
| D2 | Probes per SNODE | 20 (2 rows x 10 trees). CD74HC4067 16:1 mux plus 4 spare ESP32-C3 ADC pins (pins UNVERIFIED) | Each grove tiles exactly into 10 clusters; 30 SNODEs for the forest | S9 |
| D3 | Wiring | Star, one 18/3 direct-burial irrigation wire per probe (see D16; replaces Cat5e) | Same cost as Cat5e; takes standard irrigation splices; smaller OD for the riser | S32, S33 |
| D4 | Radio | ESP-NOW star. SNODE: small antenna (u.FL rod or FPC) on the enclosure, about 0.3 m above ground. Receiver: the only tall antenna in the system, on the trailer roof, about 5 m up. Fallbacks: ESP-NOW long-range mode, then a sub-GHz radio. | Owner: no masts in the field. A high antenna at one end keeps most of the Fresnel zone clear (see radio section). | S6, S7, S11, S22, S27 |
| D5 | MCU | Seeed XIAO ESP32C3 (ships with an external u.FL antenna). Board sleep current no longer matters because the board is unpowered between readings. | Needs a connector for a small external antenna; the SuperMini has only a PCB antenna | S12, S26 |
| D6 | Power | Two variants, A (solar only) and B (battery only). See sections below. **Recommended: B.** | B has no dependence on sun or snow, fewer parts, and nothing visible above the enclosure | S14, S15, S21, S24–S25 |
| D7 | Frozen soil | One DS18B20 per SNODE at probe depth; HA marks moisture invalid and blocks irrigation when soil temperature is 0.5 °C or below | Capacitive sensors read frozen soil as dry | S13, S20 |
| D8 | Report times | 05:00 and 17:00, set by the RTC alarm (RV-3028-C7, ±1 ppm at 25 °C) | Owner confirmed the times. In December, sunset is about 16:40, so both readings happen in the dark then. | S23b |
| D9 | Emitter offset | Same offset and same side at every tree; distance set by a POC wetting test (starting point 10 cm) | New trees have a small wetted zone | — |
| D10 | Delivery | Wait for the ESP-NOW acknowledgement, retry up to 3 times, keep unsent readings and resend them next wake. HA alerts "offline" after 25 h with no report. | With 2 reports a day, one lost packet means a 12 h gap | — |
| D11 | Power gating | RV-3028-C7 RTC (45 nA, alarm output) driving a load switch. TPL5110 rejected: its longest interval is 2 h, so it would need 6 wakes per reading and gives no clock time. | Removes sleep current; gives exact report times | S24, S25 |

Options considered and not chosen:
- Resistive probes: the electrodes corrode (S2). Rejected for burial.
- Bare electrode on ESP32 touch pins (A2): cents per probe, but cable capacitance and drift over metres are untested. Possible POC experiment.
- I2C capacitive probes (Adafruit STEMMA $6.00 at 100+): too expensive; I2C isn't designed for long field cables.
- Zigbee/Thread mesh: routers cannot sleep (S11).
- Shielded 22/3 direct-burial cable ($3.49/ft): too expensive.
- 16 probes per node: 15 nodes per grove instead of 10; more expensive.
- 2 m mast per SNODE: rejected by owner (visual clutter).
- LiFePO4 + solar: superseded by the owner's two variants (no hybrid).

## Energy per day (both variants, 20 probes, 2 reports/day)
Assumptions are UNVERIFIED until measured in the POC.
| Item | Calculation | mAh/day |
|---|---|---|
| RTC always on | 45 nA x 24 h | 0.001 |
| Load switch off-state leakage (assumed max 1 µA) | 1 µA x 24 h | 0.024 |
| MCU boot + run | 25 mA x 2.0 s x 2 | 0.028 |
| Probes | 20 x 5 mA x 1.0 s x 2 | 0.056 |
| ESP-NOW transmit | 120 mA x 0.2 s x 2 | 0.013 |
| **Total** | | **0.12** |
| **With 2x margin** | | **0.24** |
Compare: 2.24 mAh/day before power gating. The probes are now the largest item.

## Variant B — battery only (recommended)
- 3x Energizer L91 lithium AA in series (rated -40 to 60 °C) → LDO → 3.3 V for the node while awake. The RTC runs straight from the pack.
- Usable capacity: 3000 mAh x 80% (cold, cutoff) = 2400 mAh. Divided by 0.24 mAh/day = 10,000 days on paper.
- In practice the cells' shelf life sets the limit (more than 20 years demonstrated at 21 °C, S15). **Plan a swap every 5 years** (UNVERIFIED until a winter of field data).
- Winter: no charging, so no cold-charge problem. Capacity falls in the cold, but the margin is about 50x.
- Battery voltage is reported every wake (`sensor.battery_snode_<id>`). HA alerts below a threshold set from the POC discharge curve.

## Variant A — solar only (no battery)
- Storage: one 2.7 V supercapacitor, **10 F**. Usable window 2.7 V → 1.0 V through a boost converter to 3.3 V. The RTC runs straight from the capacitor, down to 1.1 V.
- Supercapacitors are rated -40 to 65 °C at 2.7 V and accept charge in the cold.
- Sizing: drawing from the capacitor at an average of about 1.9 V through an 85% boost roughly doubles the 0.24 mAh/day load, to about 0.49 mAh/day. Leakage at about 1 µA/F (typical) adds 10 µA = 0.24 mAh/day. Total about 0.73 mAh/day.
  - Stored: 10 F x 1.7 V = 17 C = 4.7 mAh. 4.7 / 0.73 = **about 6 days with no sun**.
  - A bigger capacitor doesn't help much: large 100 F cells list 140–260 µA leakage, which exceeds the whole load.
- Panel: very small (well under 1 W). Daily energy need is about 2 mWh, plus about 13 mWh to refill after a storm. Size it to shed snow (steep tilt) and to be inconspicuous, not for energy.
- Charge limit: a shunt or charger that holds the capacitor at 2.7 V or below.
- **Failure mode:** after more than about 6 days without sun (snow on the panel, storms), the node goes dark and misses reports. When the sun returns it restarts on its own: no valid RTC time → it sends a "hello", gets the time from the receiver, and resumes. Winter gaps are likely. Owner to accept or reject (round 7).
- Supercapacitor life: about 10 years projected at 2.7 V and 25 °C (Maxwell 100 F datasheet; UNVERIFIED for the chosen 10 F part).

## Operating limits (for a commercial datasheet)
Each limit is set by the weakest part. All UNVERIFIED until tested; "rated" values are manufacturer ratings.
| Limit | Variant A (solar only) | Variant B (battery only) | Set by | Sources |
|---|---|---|---|---|
| Ambient temperature, operating | -40 to +65 °C (rated parts); design point -27 °C | -40 to +60 °C (rated parts); design point -27 °C | A: supercapacitor upper limit. B: L91 upper limit. Probe lower limit unknown (POC cold test). | S15, S20, S25, S26b, S28 |
| Days without sun | About 6 at the design load, then the node pauses and restarts on its own | Not applicable | 10 F capacitor and leakage | S28 |
| Battery life | Not applicable | Plan 5 years (energy alone would last decades) | Cell shelf life and cold behavior | S15 |
| Reports per day | 2 (design). More reports shorten the no-sun autonomy roughly in proportion. | 2 (design). Up to about 48/day still gives years. | Energy budget | — |
| Probes per SNODE | 20 | 20 | Mux 16 + 4 ADC pins | S9 |
| Probe cable length | 6 m max until tested | 6 m max until tested | Analog signal over cable (POC test) | — |
| Probe depth | 15 cm max | 15 cm max | Probe board length, about 99 mm | S3 |
| Radio link | 105 m design, node antenna 0.3 m, receiver antenna 5 m | Same | Walk test pending | S6, S22 |
| Valid moisture | Only when soil is above 0.5 °C | Same | Capacitive measurement in frozen soil | S13 |
| Snow depth | Box height − 0.1 m (0.2 m at the standard 0.3 m height; longer stake for snowy sites) | Same | Box and antenna must stay above snow | — |
| Field splices | -40 to +105 °C operating; 18–10 AWG | Same | 3M DBR/Y-6 rating | S33 |
| Field cable temperature | Accepted as the -20 °C class for buried cable; landscape wire low limit not published | Same | Cable rating | S36, S41 |
| Bus run length | About 10.5 m per row run by design; 20 m radius/weight is the reference limit for a simple 1-Wire master (bench test) | Same | Bus electrical limits | S42 |
| Probe connection | Piercing clamp; corrosion life to be proven by soak test | Same | Contact corrosion | S40 |
| Vehicle lanes | Wider gap needed (about 3.5 m for a full-size pickup); never in a cluster gap | Same | Vehicle width | S34 |
| SNODEs per receiver | 30 tested by design (60 packets/day); higher counts need a test | Same | Receiver load | — |

## Variant comparison
| | A: solar only | B: battery only |
|---|---|---|
| Winter readings | Gaps after about 6 days of no sun or snow-covered panel | No gaps from energy |
| Visible parts | Small panel at each SNODE | Enclosure only |
| Consumables | None | 3x AA lithium about every 5 years |
| Extra parts | Panel, supercapacitor, boost converter, charge limiter | Battery holder, LDO |
| Main risk | Snow, dust, panel damage | Cell leakage or cold behavior worse than datasheet |

## Radio path (worst case, no field masts)
- Plot 100.6 x 182.9 m, trailer at the center. Farthest point ≈ 104 m. Design link 105 m. λ at 2.44 GHz ≈ 0.123 m. First Fresnel radius r(x) = sqrt(λ·x·(D−x)/D); 60% of it must be clear.
- Node antenna 0.3 m, receiver antenna 5 m (on the trailer roof):
| Distance from node | Line height | 60% Fresnel radius | Lower edge of the clear zone |
|---|---|---|---|
| 10 m | 0.75 m | 0.63 m | 0.12 m (clears bare ground) |
| 20 m | 1.20 m | 0.85 m | 0.35 m (young trees intrude) |
| 52.5 m (mid) | 2.65 m | 1.08 m | 1.57 m (clear) |
- So only the first about 20 m near the node is partly blocked, by the node's own young trees. UNVERIFIED: walk test with the POC node at 0.3 m from the farthest corner.
- Reference point: Ecowitt rates the WH51 (a sub-GHz sensor at ground level) for 100 m in open field. If ESP-NOW fails the walk test, research a sub-GHz radio next.

## Cluster comparison (grove 10 rows x 20 trees, cited parts only)
Cable per probe = horizontal Manhattan distance + 0.65 m (0.15 m depth + 0.5 m slack).
| Option | Nodes/grove | Avg cable | Total/grove (old node parts) |
|---|---|---|---|
| 16/node | 15 | 3.45 m | $951.21 |
| 20/node | 10 | 4.15 m | $927.92 |

## Cost so far (cited parts only, variant B, bus architecture)
| Item | Unit | Per grove (10 SNODEs) | Forest (30 SNODEs) |
|---|---|---|---|
| XIAO ESP32C3 with antenna | £3.93 (Farnell UK, ex VAT) | £39.30 | £117.90 |
| Hammond 1554QA2GY enclosure | £40.42 (RS UK, ex VAT) | £404.20 | £1,212.60 |
| RV-3028-C7 RTC | $3.74 (DigiKey) | $37.40 | $112.20 |
| DS18B20 probe | $1.90 | $19.00 | $57.00 |
| 3x L91 AA | $6.87 | $68.70 | $206.10 |
| 16/2 landscape wire, 500 ft at $140 | about 23 m (75 ft) per SNODE incl. 10% slack | 750 ft → 2 spools = $280.00 | 2,264 ft → 5 spools = $700.00 |
| Probe connection (integrated clamp) | about $0 extra (UNVERIFIED) | $0 | $0 |
| Bus probes (custom PCB, D19) | estimate $0.45–1.30 each (UNVERIFIED) | about $90–260 for 200 | about $270–780 for 600 |
| **Subtotal (without probes)** | | **$405.10 + £443.50** | **$1,075.30 + £1,330.50** |
- Compared with the analog star: cable drops from $2,125 to $700 for the forest, and the mux ($6.50 per node) is gone. The probe cost moves into the custom probe PCB.
- Not yet priced: bus probes, bus driver at the node, load switch, LDO, battery holder, riser conduit and fittings, duct seal, vent, wooden stake, receiver and roof antenna. The enclosure is now the largest priced item; source a cheaper box.

## Ecowitt WH51 comparison (for the test plan)
- WH51: capacitive FDR, 1x AA, reports every 70 s to the GW3000, rated 100 m in open field. The related WH51L is rated -10 to 50 °C, so expect no comparison data in deep cold (UNVERIFIED for the WH51).
- Owner has 4x WH51 and one GW3000 (16 channels max); keep these numbers.
- Plan: one WH51 in each grove (3), about 10 cm from one SNODE probe, same emitter offset and side, same depth where its probe allows. The 4th WH51 goes at a second tree in the front grove, to show tree-to-tree spread.
- Compare trends and wet-up/dry-down timing, not absolute %. Calibrate both with the same air-dry and saturated points of site soil.

## POC to start now (with parts on hand)
1. ESP8266 + two existing sensors + two pots of site soil (dry, wet). Log every 10 min for a week to check drift (S5: one ADC, 0–1 V bare chip).
2. RTC + load switch: measure the off-state current of the whole node with a µA meter. Target: 1 µA or less.
3. Walk test: node antenna at 0.3 m, receiver antenna as high as possible at the trailer, from the farthest corner.

## Design sections remaining (from the brief)
1. Requirements list
2. Sensing element comparison and calibration (partly done)
3. Node/radio options (draft done)
4. Power (two variants drafted)
5. Cabling layout (draft done)
6. Enclosure and burial (draft done)
7. Firmware and HA integration (partly: D8, D10, D11)
8. BOM with part numbers (partial)
9. Test plan (partly: WH51 comparison, POC steps)
10. Open questions (below)

## Enclosure, mounting and burial (draft, all UNVERIFIED)

### Layout rules (drive lanes)
- Owner: drive lanes must fit a Toyota Tundra. A Tundra is 80.2 in (2.04 m) wide **without mirrors**, which is wider than the 2 m row spacing. **So a drive lane needs a wider gap than the grid gives.** Proposed: about 3.5 m between tree rows at each lane. That allows mirrors (width not yet sourced; measure the truck mirror to mirror) plus about 0.5 m each side for young canopy.
- A cluster pairs two rows across a 2 m **non-drive** gap. The SNODE stake and all its cables stay inside that gap.
- Drive lanes are only ever between clusters. No cable crosses a lane, so no conduit is needed for vehicle loads.
- Each probe goes on the side of its tree facing the cluster's internal gap, away from wheels.
- Grove width: 18 m with no lanes; each 3.5 m lane adds 1.5 m. Tree count stays at 200.

#### Drive-lane variants (grove of 10 rows x 20 trees; the user chooses)
Rows numbered 1–10; clusters are row pairs (1-2, 3-4, 5-6, 7-8, 9-10); lanes can only be between pairs (gaps 2|3, 4|5, 6|7, 8|9). Grove length (along rows) is 19 m in all variants. "Walk" = farthest distance from any tree row to a drivable line (a lane or the grove edge).
| Variant | Lanes at gaps | Grove width | Area | Land use vs V0 | Max walk |
|---|---|---|---|---|---|
| V0 | none | 18.0 m | 342 m² | — | 8 m |
| V1 | 6\|7 (or 4\|5) | 19.5 m | 370.5 m² | +8% | 4 m |
| V2 | 4\|5 and 6\|7 | 21.0 m | 399 m² | +17% | 2 m |
| V4 | all four | 24.0 m | 456 m² | +33% | 0 m (every row is next to a lane or the edge) |
- Node and cable count are the same in all variants (10 SNODEs per grove), because buses never cross a lane.
- 3 lanes is not listed: it gives the same 2 m max walk as V2 with more land.
- **This site: V1**, one lane at gap 6|7 (grove 19.5 m x 19 m). The mirror position (4|5) works the same.

### D12 Enclosure
- UV-stabilized polycarbonate box, IP68 class. Reference part: Hammond 1554QA2GY, 140 x 140 x 90 mm, £40.42 (RS UK, single unit). A cheaper equivalent is to be sourced; it is the largest per-node item found so far.
- Mounted on a **wooden stake** (owner: rabbits don't damage a wooden stake or a plastic box).
- Antenna **inside** the box: the XIAO's flexible antenna stuck under the lid. Nothing visible outside. Polycarbonate should pass 2.4 GHz with little loss (UNVERIFIED; walk test).
- One membrane vent (e.g., GORE Protective Vent) to equalize pressure and reduce condensation.

### D13 Cable riser (required)
- Owner: a conduit riser from the box into the ground is required (rabbits), size **1/2 in standard PVC**.
- 1/2 in Schedule 40 PVC: average ID 0.578 in → 0.262 in² → 40% fill = 0.105 in².
- Fill check by architecture:
| Architecture | Cables in the riser | Area | Fill | Fits? |
|---|---|---|---|---|
| Analog star (20 x 18/3 at 0.184 in + DS18B20) | 21 | ≈ 0.56 in² | ≈ 213% | **No** |
| Digital bus, 2 row cables of 18/4 at 0.2 in | 2 | ≈ 0.063 in² | ≈ 24% | Yes |
| **Digital bus, 2 row cables of flat 16/2 landscape wire (0.155 x 0.296 in)** | 2 | ≈ 0.092 in² | ≈ 35% | **Yes (chosen)** |
- **So a 1/2 in riser only works with a bus architecture (D17), which the owner accepted.**
- Seal: duct-seal compound inside the terminal adapter at the top. The buried bottom end stays open for drainage.

### D14 Stake and snow
- Owner: keep it minimal. **One standard box height: bottom of box 0.3 m above grade.** For snowy sites the installer cuts a longer stake and riser. Limit: snow depth ≤ box height − 0.1 m.
- Variant A panel: above the box on the same stake, tilted 60° or more, so snow slides off.

### D15 Burial (installer instructions)
- Trench along the center of the cluster's internal gap, 15–20 cm deep. Short branch trenches of about 1 m to each tree.
- Optional conduit on the main trench in rodent areas; the branches stay direct-burial.
- Leave about 0.3 m of slack at each probe as a loop, for frost heave and to allow the probe to be pulled out for service.
- Locate the drip line (surface or buried) before inserting a probe; keep the standard offset from the emitter.

### D16 Bus cable and probe connection
- **Cable comparison (owner request):**
| | 16/2 landscape lighting wire | 18/2 irrigation (sprinkler) wire |
|---|---|---|
| Shape | Flat, 0.155 x 0.296 in | Round jacket, about 0.190 in OD |
| Price | $140 / 500 ft = $0.28/ft (Home Depot) | $186.27 / 1,000 ft = $0.19/ft (Home Depot); $46.77 / 250 ft = $0.19/ft |
| Forest (2,264 ft) | 5 x 500 ft = **$700** | 3 x 1,000 ft = **$559** |
| Riser fill (2 cables, 1/2 in) | 35% | 22% |
| Built-in piercing clamp | Works: made for flat cable, no prep | The outer jacket must be slit or stripped first, which breaks the "no prep" rule (UNVERIFIED for clamp designs that pierce through the jacket) |
| Temperature | 60 °C max; low limit not published | -20 to 75 °C (Southwire family) |
- **Decision (owner confirmed): 16/2 landscape wire is the standard.** 18/2 irrigation wire is the backup user option (about 20% cheaper; needs caps or a jacket-stripping step).
- **Bus cable: 16/2 low-voltage landscape lighting wire.** Flat 2-conductor, direct-burial rated, sold at home-improvement stores: 500 ft for $140 ($0.28/ft, Home Depot). Flat profile 0.155 x 0.296 in, 60 °C max (Carol 02309 data). Minimum temperature not published (UNVERIFIED; the owner accepts a -20 °C class rating for buried cable).
- **Probe connection: piercing clamp built into the probe housing.** This is the method landscape-light fixtures use: pins pierce the insulation of the flat cable without cutting, stripping or splicing, so a branch can be made at any point on the run.
  - Install: lay the cable along the row, press each probe's clamp onto it at its tree, insert the probe. No separate connector is bought, so the connection cost is the two pins inside the probe (cents; UNVERIFIED until the housing is designed).
  - Known risk: reviews of retail piercing connectors report pin corrosion and breakage after about a year. Mitigations: grease-filled clamp cavity, stainless or tin-plated pins, and a bus that is **unpowered more than 99.9% of the time** (powered only for seconds at 05:00 and 17:00), which limits electrolysis (UNVERIFIED; long-term soak test required).
- **User options (not required):** retail landscape piercing connectors (about $0.68 each, 22-pack for $14.98) or 3M DBR/Y-6 caps (about $2.88 each) with a probe that has a pigtail.
- No soldering, no bench prep: spool + probes.

### D17 Probe architecture for the product (owner accepted: 2-wire digital bus)
- Each probe has a small microcontroller that measures locally and talks on the shared pair, which also carries power.
- One 16/2 run along each row of the cluster; each node has 2 runs (one per row). Bus topology: linear along the row, never a star (1-Wire guidance says an unswitched star is not recommended).
- Candidate protocol: 1-Wire style (data and ground on 2 wires, power from the data line). Reference: Maxim/ADI tutorial 148 defines network **radius** (wire distance to the farthest device) and **weight** (total wire). A simple master is quoted at about 20 m radius and 20 m weight on Cat5; an advanced master is quoted up to 500 m equivalent size.
  - Per row run: node to row about 1.5 m + 9 m along the row ≈ 10.5 m radius and weight. That is within the simple-master figures, but on flat landscape wire rather than Cat5 (UNVERIFIED; bench test).
  - A bonus: the DS18B20 soil temperature sensor is itself a 1-Wire device and can sit on the same bus.
- Probe power: the probe needs a few mA for about a second to measure. Draw from the line during a "power phase", then talk. Protocol and electrical design to be researched next (UNVERIFIED).
- POC: v1.2 analog probes with soldered wiring, to learn sensor behavior.

### D19 Minimum bus probe (cost estimate, UNVERIFIED until quoted)
Concept: the probe blade is the PCB. Copper electrode traces on the blade are the capacitive sensor, measured by the MCU with an RC charge-time method. A diode + capacitor take power from the bus. The MCU talks a 1-Wire style protocol. The electronics end is potted. A plastic housing holds two piercing pins for the 16/2 cable.
| Item | Basis | Per probe (600 qty) |
|---|---|---|
| MCU: Puya PY32F002A (Cortex-M0+, 1.7–5.5 V, -40 to 85 °C) | LCSC, ESSOP-10, $0.0927 at 500+ | $0.09 |
| Alternative MCU: WCH CH32V003J4M6 (SOP-8) | LCSC, $0.1238 at 500+ | ($0.12) |
| SMT assembly | JLCPCB $0.0016 per joint x about 24 joints | $0.04 |
| Assembly setup, one side | JLCPCB $25 per order / 600 | $0.04 |
| Extended-part feeder fees | JLCPCB $1.50 per part type x about 3 types / 600 | $0.01 |
| Passives (diode, capacitors, resistors, transistor) | not yet quoted | estimate $0.02–0.05 |
| PCB blade, about 100 x 20 mm, 2-layer | not yet quoted (JLCPCB from $2 for 5 prototype boards) | estimate $0.10–0.30 |
| Potting of the electronics end | not yet quoted | estimate $0.05–0.15 |
| Housing with 2 piercing pins | 3D-printed (no tooling) or injection-molded (tooling cost, cents per part) | estimate $0.10–0.60 |
| **Total** | | **estimate about $0.45–1.30; target ≤ $1.50** |
- For comparison, the v1.2 analog board is $1.70 and still needs a cable to the node.
- The MCU and assembly are cited; the PCB, passives, potting and housing need real quotes. The housing method depends on volume (round 13).
- Risks to test: capacitance drift with temperature (compensate with the MCU or the node's DS18B20), the accuracy of RC timing compared with the v1.2 board, and bus power during measurement.

### D20 Temperature compensation (owner chose T1)
- **Why it matters:** a capacitive probe reads permittivity, not water directly, and permittivity changes with temperature even when water content doesn't.
  - Free water: about 88 at 0 °C, about 80 at 20 °C (Campbell Scientific).
  - Bound water: the opposite effect; warming releases water bound to soil particles, which raises permittivity. The net effect depends on the soil.
  - In a 19-soil study, a 40 °C change caused up to ±0.028 m³/m³ apparent water-content change.
  - The sensor electronics drift too.
- **Effect here:** the 05:00 reading (cold) and the 17:00 reading (warm) at 15 cm see different soil temperatures, so uncorrected data shows a fake daily swing.
| Option | How | Extra cost | Accuracy | Notes |
|---|---|---|---|---|
| **T1 (chosen)** | One DS18B20 per SNODE (already in the design for the frozen flag) corrects all 20 probes | $0 | Assumes the same soil temperature across the 18 x 2 m cluster | Sun, shade and wet spots near emitters may differ (UNVERIFIED) |
| T2 | The probe MCU's **internal temperature sensor** (the PY32F002A has one on an ADC channel) corrects each probe; the node's DS18B20 is the reference used to remove each chip's offset | $0 | Per probe; the uncalibrated offset varies chip to chip, so calibrate against the DS18B20 | Also gives a per-probe frozen flag. The electronics end sits in the soil, so chip temperature ≈ soil temperature (UNVERIFIED) |
| T3 | An NTC thermistor on each probe PCB | a few cents + 1 pin | Better than T2 | Only if the POC shows T2 is not good enough |
- Correction coefficients are soil-specific and are measured in the POC (pot test at 2 temperatures). They are stored in `config/calibration.yaml`, never in firmware.

### D21 Housing manufacturing path
- **POC and small batches: in-house 3D printing.** ASA for anything exposed to sun (better UV resistance; PETG yellows and turns brittle outdoors); PETG is acceptable for fully buried parts (UNVERIFIED). Seal layer lines on printed parts.
- **Later, molded housings (note for the future):**
  - Protolabs: aluminum molds, mold cost from $1,495; runs from about 10 to 10,000+ parts; no minimum order.
  - RapidDirect: low-volume injection molding, 100–10,000 parts; aluminum molds suit 100–5,000 parts, soft steel 5,000–25,000.
  - Get quotes once the housing design is frozen after the POC.

### D22 Bus electrical design (draft, UNVERIFIED until bench test T4)
- **Protocol: probes emulate DS18B20 thermometers** (1-Wire family 0x28). Each probe answers with its own 64-bit address and puts its raw moisture count in the temperature register.
  - Gain: ESPHome reads them with its standard `one_wire` + `dallas_temp` components, by address. No custom ESPHome component for the probes.
  - Emulation is proven on other MCUs: the OneWireHub library emulates DS18B20 devices, up to 32 per hub. Porting to the PY32F002A is not yet confirmed.
  - Scaling: the DS18B20 register is 16 bits in 1/16 °C steps. The raw count is scaled to fit, and HA converts it with `config/calibration.yaml`.
  - The probe's 64-bit address is printed on its label and mapped to a tree ID at install (`config/probes.yaml`, planned).
- **Physical layer (2 wires: DATA and GND):**
  - The node pulls the line up through a low-value resistor (about 100–220 Ω instead of 1-Wire's usual 4.7 kΩ), so the line can also supply the 20 probes while they measure.
  - Each probe has a diode + reservoir capacitor (rides through the short low pulses of the protocol) and pulls the line low with a small MOSFET, not with an MCU pin.
  - Sequence each wake: (1) power phase, about 200 ms: line held high, all probes measure at once and store the result; (2) read phase: the node reads each probe's register by address.
  - **Master: 2x DS2482-100** (one per row bus), I2C-to-1-Wire bridges supported by ESPHome's `ds248x` component. Each has a strong pull-up through a low-impedance path, and a PCTLZ output that drives an external MOSFET for a stronger pull-up. That MOSFET is the power phase. Operating range 2.9–5.5 V, -40 to 85 °C. List price from $1.18 (1k). A DS2484 also works, but it has no address pin, so two of them can't share one I2C bus.
  - Two separate channels (one per row) instead of one network with two branches, because 1-Wire guidance warns against star layouts.
  - ESPHome's GPIO bit-bang 1-Wire has reported reliability problems; the DS2482 avoids it.
- **Protection:** a TVS diode and series resistor on each bus at the node (long buried cable).
- **Polarity:** bridge rectifier at each probe (owner accepted). The probe pull-down sits on the DC side of the bridge, so the line low level is about one Schottky drop + MOSFET drop. It must stay below the 1-Wire low threshold (bench test T4).

### D23 Node hardware (draft)
- **Variant B power tree:**
  - 3x L91 pack (4.5 V nominal).
  - Always-on rail: TI TPS7A02 LDO (25 nA quiescent, 1.5–6.0 V in) → 3.0 V for the RTC only.
  - Switched rail: TI TPS22917**L** load switch (1–5.5 V, active-low enable, 10 nA typical / 100 nA max shutdown current at -40 to 85 °C), enabled by the RTC's open-drain INT output.
  - From the switched rail through a Schottky diode into the XIAO's 5V pin (Seeed requires a diode when feeding external power there) → XIAO onboard LDO → 3.3 V.
- **Off-state budget:** RTC 45 nA + TPS7A02 25 nA + TPS22917 ≤ 100 nA ≈ 0.17 µA max, within the 1 µA POC target.
- **Margins:**
  - A fresh pack is close to the TPS22917's 5.5 V maximum (absolute max 6 V). Check fresh-pack voltage in the POC.
  - At end of life the pack, minus the diode drop, falls below the 3.0 V the ESP32-C3 needs. The node stops before the cells are fully used; the energy margin covers this.
- **Variant A parts:**
  - 10 F 2.7 V supercapacitor (e.g., Eaton HV1030-2R7106-R, $1.97 at 50; or Tecate TPL-10-10X30F, -40 to 85 °C). The RTC runs directly from it (1.1–5.5 V).
  - Switched side: TI TPS61099 boost converter (0.7–5.5 V in, 1 µA quiescent, true disconnect in shutdown) → 5 V into the XIAO 5V pin. A bulk capacitor on its output covers the ESP32 radio peaks (sizing in the bench test). TI's TPS61094 (60 nA, built-in supercap management) is the alternative to evaluate.
  - Panel and charge limiter: to be selected after the bench energy measurement.
- **Node I/O (XIAO ESP32C3):** I2C to the RTC; 2 bus channels (row A, row B); 1-Wire pin for the node DS18B20; battery-voltage divider on the switched rail (no drain when off); RTC INT readback.

### D24 Probe hardware (draft)
- **Blade:** 2-layer FR4 PCB, about 100 x 20 mm. Electrode traces under solder mask (insulated, no electrode corrosion), with a ground guard.
- **Measurement:** RC charge time. A GPIO charges the electrode through a resistor; one of the PY32F002A's 2 comparators detects the threshold; a timer captures the time. Charge time rises with soil permittivity.
- **Bus interface:** diode + reservoir capacitor, a pull-down MOSFET, and a sense divider. Optional bridge rectifier for polarity.
- **Electronics end:** potted. Two piercing pins in the clamp head contact the 16/2 conductors. A grease-filled cavity; a 3D-printed ASA head for the POC.
- **Depth:** owner said 6 in. Design: electrode about 10 cm long, sensing zone about 5–15 cm, tip at 15 cm. To confirm in round 16.

### D25 Probe mechanics and install (draft)
- Sensing zone about 5–15 cm; bottom of the sensing zone at 6 in (15 cm).
- **Bus cable depth: at least 6 in of cover** (US irrigation practice for ≤ 30 V: Paige installation spec, NEC Table 300.5, municipal codes). The bus runs at 3.3–5 V.
- The problem: the cable lies at 6 in, the same depth as the bottom of the sensing zone. A clamp at the top of the probe is about 15 cm above the cable.
- **Decided (owner, round 18): option C, probe lead.** The main cable lies flat at 6 in cover along the row. Each probe has its own short factory lead with the piercing clamp molded on its end. In the trench, the clamp goes onto the main cable.

```
 grade ────[probe head, potted]──────────────────────────
              |    \
   sensing    |     \  probe lead (factory-fitted, ~30 cm)
   zone       |      \
   5–15 cm    |       \
 6 in ─ ─ ─ ─ ┘ ─ ─ ─ ─[clamp]═══ main cable, flat, 6 in cover ═══
```
  - Install, per tree: trench along the row to about 7 in, lay the main cable on the trench floor, push the probe in at its emitter offset, press the lead's clamp onto the main cable, backfill. Same clamp for every probe; no soldering, no cutting.
  - Why C is better than A and B:
    - The cable runs straight down the row, so the probe position no longer depends on where the cable is (probe at its fixed emitter offset).
    - The probe stays short (electronics in the head, as in D24).
    - A failed probe can be replaced without touching the main cable: dig at the clamp, pull it off, fit a new probe.
  - Cost: about 40 cm of 2-core lead per probe (600 x 0.4 m = 240 m) plus fitting the clamp at the factory. Price at POC (no cited price yet). The main cable stays at 5 spools.
  - Risks (UNVERIFIED, POC): the clamp is underground and sealed in gel or potting compound, so test T5 soaks it in wet soil. The lead runs near the sensing zone, so it must leave the head away from the blade face; any fixed offset it adds is removed by calibration (T6).
- **Probe position and cable side — decided (owner, round 20): P1 + K2.** On this site the emitter sits almost at the trunk, so the probe goes 10 cm from the emitter on the cable side (perpendicular to the row). Lead 40 cm (draft until the site check of the drip lateral).
- Background for that decision:
  - Guidance found: drip-irrigated trees: sensor at the canopy drip line, 12–18 in from an emitter (Monnit; established trees). Vineyard extension: too far from the emitter misses the wetting pattern and leads to over-watering; too close reads wetter than the root zone. There is no single number for young trees (S71, S72).
  - New trees: roots are still in the planting hole, and the wetted bulb under one emitter is small, so the probe should sit inside both.
  - Offset options:
    - P1: 10 cm from the emitter, toward the trunk (recommended for years 1–3; inside the root ball and the wetted bulb; fast response to each irrigation).
    - P2: 20–30 cm from the emitter (edge of the wetted bulb; fits established trees; risks missing water in coarse soil).
  - Cable side options:
    - K1: main cable on the tree line, under the drip lateral (shortest lead; but the trench cuts through the root ball and runs next to the drip tubing).
    - K2: main cable 20–25 cm off the tree line, on the side away from the emitters (recommended: the trench misses roots and tubing; lead reaches across).
  - Lead length with P1 + K2: about 20–25 cm across + about 15 cm down + slack = **40 cm** draft (was 30 cm). The slack also lets the probe move about 20 cm outward later, as the tree grows, without touching the main cable.
- Rejected: A (rise loop: extra cable, cable near the surface at every tree); B (foot clamp: longer probe, and the probe has to sit on the cable line).

### D26 Node enclosure, POC (3D-printed ASA)
- ASA, walls ≥ 2.5 mm, gasketed lid (printed TPU gasket or an O-ring groove). Seal the print (layer lines) and run a soak test before field use.
- Inside: 3x AA holder (variant B) or supercapacitor board (variant A); XIAO with its flexible antenna under the lid; DS2482 board.
- Bottom: one socket for the 1/2 in PVC riser (solvent-weld or threaded adapter); one membrane vent.
- Back: 2 screw tabs for the wooden stake; box bottom 0.3 m above grade.

### D27 Receiver: why, and whether we need one (draft)
- **Why it was in the design:** SNODEs send by ESP-NOW instead of joining Wi-Fi. Measured on an ESP32 (MicroPython): boot + send an ESP-NOW message = 119 ms, 7.7 mC. Boot + send MQTT over Wi-Fi with a static IP = 774 ms, 108.6 mC; with DHCP, 1,626 ms. So Wi-Fi costs about 14x more charge per report.
- **What that means here (UNVERIFIED, arithmetic):** the extra is (108.6 − 7.7) mC x 2 reports ≈ 0.056 mAh/day.
  - Variant B: negligible (energy lasts decades either way).
  - Variant A: no-sun autonomy drops from about 6 to about 5.5 days.
  - **So energy is not the main reason.**
- **The real trade-off is range and setup:**
| | N1: no receiver, nodes join Wi-Fi directly | N2: ESP-NOW receiver |
|---|---|---|
| Extra hardware | None (but the trailer Wi-Fi must reach 105 m to antennas 0.3 m above ground, through young trees) | One small box, roof-mounted |
| Link | Association, DHCP/static IP and API connection: several packets each way, seconds awake; longer on a weak link | One short packet + acknowledgement; long-range mode available |
| Router | 30 more clients; credentials on every node | Fixed 2.4 GHz channel required |
| ESPHome | Native API works with sleeping devices (current community advice) | ESP-NOW Packet Transport |
| Commercial | Depends on the customer's Wi-Fi coverage | Independent of Wi-Fi coverage |
- **Proposal:** test N1 first in the POC walk test (T9), because it needs no extra hardware. Keep N2 as the fallback for this site and as the default for commercial sites. Both run from the same node firmware with a build option.
- If N2 is used: the same box as a SNODE; indoor or outdoor. Modes:
  - R-USB: always on.
  - R-solar: windowed listening around 05:00/17:00, about 25 mAh/day with margin, which needs a LiFePO4 18650 + 0 °C cut-off charger (a supercapacitor is too small).
- Cold start (variant A after a long dark period): the node wakes hourly and retries until it gets the time (N2 window, or N1 via HA/SNTP).

### D28 Variant A charger (closes an open item)
- TI BQ25570 energy-harvesting charger between the panel and the supercapacitor:
  - Continuous harvesting from 100 mV input; cold start from ≥ 600 mV, so an empty capacitor is not a problem.
  - Programmable MPPT; 488 nA typical quiescent current.
  - Supports supercapacitors (TI reference design TIDA-00242: solar harvester with a supercap).
  - Its overvoltage setting caps the capacitor at ≤ 2.6 V.
- TPS61099 stays as the 5 V boost for the XIAO. The panel is chosen to suit the BQ25570's input range (bench step).

### D29 RTC support in ESPHome (closes an open item)
- ESPHome has built-in time platforms for RTC chips such as the DS1307 and RX8025T, but none found for the RV-3028.
- An ESP-IDF driver for the RV-3028-C7 exists, and ESPHome documents how to write a time-platform component.
- **Plan:** a small ESPHome external component for the RV-3028: read time, write time from HA/receiver, set the next alarm, clear the alarm flag (which cuts power). Keep the RV-3028 for its 45 nA.

### D18 Firmware### D18 Firmware
- ESPHome on both the nodes and the receiver (owner accepted).
- ESPHome has an ESP-NOW component and an ESP-NOW platform for its Packet Transport component, which sends sensor states from one node to another without a server. Nodes send their readings to the receiver; the receiver exposes them to HA through the native API.
- Boot flow under RTC power gating: power on → read sensors → send → wait for the acknowledgement → set the next RTC alarm → clear the alarm (cuts power). RTC driver support in ESPHome for the RV-3028 is not confirmed; may need a custom component (UNVERIFIED).
- Scale check needed: 30 nodes x 22 values = 660 entities through one receiver (UNVERIFIED).
- Wi-Fi mode (N1): static IP (DHCP doubled the awake time in the measurement above); ESPHome native API.
- Bus read rule: probes idle in MCU stop mode between bus events and wake on the 1-Wire reset pulse, so the line does not have to feed 20 running MCUs during the read phase. Wake time must fit 1-Wire presence timing (bench test T4, UNVERIFIED).
- **Nodes send raw values only** (raw counts, probe temperature, node soil temperature, battery voltage, RSSI). HA converts raw to % using `config/calibration.yaml`, so calibration lives in one place.
- **HA alerts:**
| Alert | Rule (proposed) |
|---|---|
| Dry | Zone median below the dry threshold for 2 readings in a row |
| Wet | Zone median above the wet threshold for 2 readings in a row |
| Frozen | Soil temperature ≤ 0.5 °C → moisture marked invalid; irrigation blocked for that zone |
| Probe fault | No answer from a probe, or raw value outside the calibrated range, for 2 readings |
| Node offline | No report for 25 h |
| Battery / storage low | Variant B: pack voltage below the POC-derived threshold. Variant A: capacitor voltage at wake below threshold |
| Weak radio | RSSI below the threshold set in the walk test, 3 readings in a row |
- **Irrigation interlock:** HA must not open an OpenSprinkler zone when that zone is frozen or its data is older than 25 h.

## Receiver
### D30 Network setups the receiver must support (owner: support any ISP, router and LAN)
- The problem: an ESP32 doing ESP-NOW and Wi-Fi on one radio must use the router's channel. In ESPHome the ESP-NOW channel can't be set when Wi-Fi is enabled (S62, S68). Many routers pick their channel automatically and change it.
- Owner's site: Starlink Mini (built-in Wi-Fi 5 router, one Ethernet port, S73); no spare Ethernet port. No manual channel setting found in Starlink docs (UNVERIFIED). So the owner's setup must work over Wi-Fi with a channel that can change.

#### D31 Channel follow (new; makes a Wi-Fi receiver work with any router)
- The receiver joins Wi-Fi on whatever channel the router uses. Nodes find it:
  1. The node saves the last channel that worked (ESP32-C3 flash, written only when it changes; or the RV-3028 user EEPROM).
  2. At each report, the node sends on the saved channel and waits for the receiver's acknowledgement (up to 3 tries).
  3. With no acknowledgement, it tries channels 1–11 (US) once each. When one is acknowledged, it saves it.
- This is a known method: nodes iterate through all channels, sending to the hub until one is acknowledged (ThingPulse; Espressif forum, S74).
- Cost (UNVERIFIED, POC T3): a full scan is about 11 short sends, a fraction of a second, and happens only after the router changes channel.
- ESPHome: a node with no Wi-Fi can change its ESP-NOW channel (`espnow.set_channel`, S68). Scan logic is a short script/lambda (bench test T3).
- Fallback **two-chip receiver**: one ESP32 does ESP-NOW on a fixed channel and passes packets over a short UART link to a second ESP32 on Wi-Fi (community pattern, S68). Works with any router; costs one extra board.

| Example | Who it suits | How | Router change |
|---|---|---|---|
| **E1: Wi-Fi receiver + channel follow (default; owner's site)** | Any router, any ISP, no free port | Receiver box in Wi-Fi range (indoor, or on the trailer roof powered by USB). Router keeps automatic channels. | None |
| E2: Ethernet receiver | A free LAN port or switch | Olimex ESP32-POE / WT32-ETH01 on the LAN; ESP-NOW on a fixed channel; PoE allows a roof box on one cable. | None |
| E3: Wi-Fi receiver, fixed channel | Routers with a channel setting | Router: 2.4 GHz channel 1, 6 or 11 at 20 MHz. Channel follow still runs, so a missed setting does no harm. | Fix channel |
| E4: dedicated farm access point | New installs; weak field coverage; direct-Wi-Fi nodes (N1) | Any router in AP mode, mounted high, wired or meshed to the main router; fixed channel. | New AP |
| E5: solar receiver away from the trailer | Large sites | Outdoor box, LiFePO4 18650 with a 0 °C charge cut-off (D27); Wi-Fi uplink + channel follow. | None |
| E6: UNODE as the receiver (optional) | A UNODE near the trailer | UNODE runs the receiver function as an extra feature; SNODE never depends on it. | As its uplink |
| E7: two-chip receiver | Routers that change channel often | As above (fallback). | None |
- Direct Wi-Fi (N1) needs no receiver and no channel handling at all; it is tested first (T9).

### Notes
- The trailer is fiberglass, which passes 2.4 GHz with little loss (UNVERIFIED; check in the walk test). The receiver ESP32 can stay inside, with a short coax to the roof antenna.

### D32 Spare boards in the POC (owner has several ESP8266 and ESP32-CAM)
- NodeMCU (ESP8266): walk test and drift logger (T11, T1); ESP-NOW field sender (T12). ESPHome supports ESP-NOW on the ESP8266, but reports received RSSI as 0 there (S75), so it can't be the measuring receiver.
- ESP32-CAM (ESP32): ESP-NOW receiver on the farm Wi-Fi for T12 (measures RSSI). Optional:
  - External-antenna test. The board has a PCB antenna and a u.FL socket, selected by a 0 Ω jumper (S76). Moving the jumper is a small solder job, so it's the owner's choice. It would show how much a u.FL antenna inside the box adds, which is closer to the final node.
  - Camera: not needed for SNODE. A between-visit photo (snow on a node box, trailer roof) belongs to the UNODE project.
- Results from these boards are indicative only. The final node is a XIAO ESP32-C3 with a u.FL antenna.

### D33 Node design v0.1 (owner chose this step, round 21)
- Full design: `docs/08-node-design.md` (block diagram, netlist, pin map, energy per wake, packet format v1, limits).
- Changes from D22/D23:
  - **Power phase from a node-switched P-FET (BUS_PWR)**, not from the DS2482 PCTLZ strong pull-up. ESPHome's ds248x docs don't say whether it drives the strong pull-up, so the power phase is kept outside the 1-Wire library. Probes measure during that phase and answer DS18B20 reads with the stored value.
  - **HOLD line:** the XIAO keeps its own power on (N-FET on the load-switch ON pin), so it can clear the alarm flag and still finish. Watchdog cuts power after 25 s.
  - **RTC rail at 3.3 V** (TPS7A0233), so the RTC and the I2C pull-ups share a level (variant B).
  - **Receiver sets the schedule:** its reply carries UTC time and the next wake (05:00/17:00 local, from HA's time zone). Nodes work in UTC, and no time zone is stored in this repo.
  - **Generic nodes:** the packet carries 20 slots; the slot → tree mapping lives in `config/probes.yaml`.
- Firmware: `firmware/node/`, `firmware/components/rv3028/`, POC receiver `firmware/receiver/`. Not compiled yet.
- Energy estimate: about 26 µAh per wake, so about 0.11 mAh/day with margin (inside the 0.24 budget). UNVERIFIED (T2).

## Open questions (round 22)
1. Bench POC parts: may I write the bench parts list now (XIAO, RV-3028 breakout, 2x DS2482-100 breakouts, about 20 DS18B20 as probe stand-ins, MOSFETs, L91 pack)? This needs cited prices (BOM rule).
2. Next design step: (b) probe PCB + PY32 bus firmware (recommended next: it is the largest unknown), or (c) the final receiver?
3. ESP32-CAM type and USB adapter: still open from round 21.

Closed in round 21: next step = node design (D33).

HA side: see `docs/06-ha-note.md`. Spec: `docs/04-spec.md`. POC: `docs/05-poc-plan.md`.
