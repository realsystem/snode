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

## Cost so far (cited parts only, variant B)
| Item | Unit | Per grove (10 SNODEs) | Forest (30 SNODEs) |
|---|---|---|---|
| XIAO ESP32C3 with antenna | £3.93 (Farnell UK, ex VAT) | £39.30 | £117.90 |
| Hammond 1554QA2GY enclosure | £40.42 (RS UK, ex VAT) | £404.20 | £1,212.60 |
| RV-3028-C7 RTC | $3.74 (DigiKey) | $37.40 | $112.20 |
| CD74HC4067 mux breakout | $6.50 | $65.00 | $195.00 |
| DS18B20 probe | $1.90 | $19.00 | $57.00 |
| 3x L91 AA | $6.87 | $68.70 | $206.10 |
| Probes | $1.70 | $340.00 | $1,020.00 |
| 18/3 irrigation wire, 500 ft spools at $0.25/ft | — | 2,723 ft → 6 spools = $750.00 | 8,169 ft → 17 spools = $2,125.00 |
| **Subtotal** | | **$1,280.10 + £443.50** | **$3,715.30 + £1,330.50** |
Not yet priced: load switch, LDO, battery holder, riser conduit and fittings, duct seal, vent, wooden stake, potting, DBR/Y-6 (repairs only), receiver and roof antenna. Variant A swaps the L91 cells and holder for a supercapacitor, boost converter, charge limiter and panel.
The mux breakout price is for prototyping; the bare chip is $1.05. The enclosure is now the biggest per-node item after cable; source a cheaper box.

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
- Grove width: 18 m with no lanes; each 3.5 m lane adds 1.5 m. Tree count stays at 200. The number of lanes per grove is an owner question (round 10).

### D12 Enclosure
- UV-stabilized polycarbonate box, IP68 class. Reference part: Hammond 1554QA2GY, 140 x 140 x 90 mm, £40.42 (RS UK, single unit). A cheaper equivalent is to be sourced; it is the largest per-node item found so far.
- Mounted on a **wooden stake** (owner: rabbits don't damage a wooden stake or a plastic box).
- Antenna **inside** the box: the XIAO's flexible antenna stuck under the lid. Nothing visible outside. Polycarbonate should pass 2.4 GHz with little loss (UNVERIFIED; walk test).
- One membrane vent (e.g., GORE Protective Vent) to equalize pressure and reduce condensation.

### D13 Cable riser (required)
- Owner: rabbits make the cable run between the box and the ground the critical part. **A conduit riser from the box down into the ground is required.**
- 1-1/2 in Schedule 40 PVC conduit (ID 1.56 in), from a terminal adapter and locknut in the box bottom down to trench depth, ending in a sweep elbow in the trench. The riser is strapped to the wooden stake.
- Fill: 21 cables (20 probes of 18/3 irrigation wire at 0.184 in OD, plus the DS18B20 lead) ≈ 0.56 in², which is 29% of the riser's 1.91 in². That is under the 40% rule for 3+ cables. (With Cat5e it would have been at the 40% limit.)
- Seal: duct-seal compound packed around the cables inside the adapter, at the top of the riser. The open buried end lets condensation drain into the soil.
- This replaces the 6 multi-hole glands in the previous draft: one hole in the box instead of six.

### D14 Stake and snow
- Owner: keep it minimal. **One standard box height: bottom of box 0.3 m above grade.** For snowy sites the installer cuts a longer stake and riser. Limit: snow depth ≤ box height − 0.1 m.
- Variant A panel: above the box on the same stake, tilted 60° or more, so snow slides off.

### D15 Burial (installer instructions)
- Trench along the center of the cluster's internal gap, 15–20 cm deep. Short branch trenches of about 1 m to each tree.
- Optional conduit on the main trench in rodent areas; the branches stay direct-burial.
- Leave about 0.3 m of slack at each probe as a loop, for frost heave and to allow the probe to be pulled out for service.
- Locate the drip line (surface or buried) before inserting a probe; keep the standard offset from the emitter.

### D16 Probe cable and splices
- **Cable changed from Cat5e to 18/3 irrigation (sprinkler) wire.** It is UL listed for direct burial, 0.184 in OD, and about the same price per foot ($0.25/ft vs $0.24/ft for Cat5e).
- Reason: the owner wants the same sealed splices used for 24 VAC irrigation wiring. Those (3M DBR/Y-6, direct bury, gel-filled, -40 to 105 °C) take 18–10 AWG only. Cat5e's 24 AWG is below their range, so it would have needed telecom gel connectors (UY2, 26–19 AWG), which are moisture-resistant but not direct-burial kits.
- **Probe assembly:** the v1.2 board's connector is removed and the 18/3 wire is soldered straight to the board. The joint and the electronics end are potted, with adhesive-lined heat-shrink over the cable exit. Probe assemblies are built on the bench in standard cable lengths. So there are no splices at the tree in a new install.
- **Field repair:** cut out the bad probe assembly and splice a new one in with 3x DBR/Y-6 (18 AWG to 18 AWG, within range).
- Potting compound must be rated to -40 °C (part to be selected).

## Receiver
- The trailer is fiberglass, which passes 2.4 GHz with little loss (UNVERIFIED; check in the walk test). The receiver ESP32 can stay inside, with a short coax to the roof antenna.

## Open questions (round 10)
1. Drive lanes: a Tundra (80.2 in without mirrors) doesn't fit a 2 m gap. Accept about 3.5 m lanes (please measure the truck mirror to mirror)? How many lanes per grove?
2. Accept 18/3 irrigation wire with bench-built, potted probe assemblies (no splices at the tree, DBR splices only for repairs)?
3. Riser: 1-1/2 in Schedule 40 PVC, rigid, strapped to the wooden stake. OK?
4. Next section is firmware and HA: is ESPHome acceptable for the receiver and nodes, or do you prefer custom firmware?
