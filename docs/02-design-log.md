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
| D3 | Wiring | Star, one gel-filled direct-burial Cat5e run per probe (3 of 8 conductors used), OD about 0.215 in, fits optional conduit | Cheapest cited direct-burial cable per metre | S17, S18 |
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
| Snow depth | Stake height − 0.1 m (0.2 / 0.5 / 0.9 m for the 0.3 / 0.6 / 1.0 m stakes) | Same | Box and antenna must stay above snow | — |
| Cable entry seal | -40 to +100 °C (multi-hole inserts) | Same | Gland insert rating | S30 |
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
| RV-3028-C7 RTC | $3.74 (DigiKey) | $37.40 | $112.20 |
| CD74HC4067 mux breakout | $6.50 | $65.00 | $195.00 |
| DS18B20 probe | $1.90 | $19.00 | $57.00 |
| 3x L91 AA | $6.87 | $68.70 | $206.10 |
| Probes | $1.70 | $340.00 | $1,020.00 |
| Cat5e, 1000 ft boxes | $241.99 | 3 boxes = $725.97 | 9 boxes = $2,177.91 |
| **Subtotal** | | **$1,256.07 + £39.30** | **$3,768.21 + £117.90** |
Not yet priced: load switch, LDO, battery holder, enclosure, glands/connectors, potting, optional conduit, receiver and roof antenna. Variant A swaps the L91 cells and holder for a supercapacitor, boost converter, charge limiter and panel (none priced yet).
The mux breakout price is for prototyping; the bare chip is $1.05.

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
6. Enclosure and burial (not started)
7. Firmware and HA integration (partly: D8, D10, D11)
8. BOM with part numbers (partial)
9. Test plan (partly: WH51 comparison, POC steps)
10. Open questions (below)

## Enclosure, mounting and burial (draft, all UNVERIFIED)

### Layout rules (drive lanes)
- A cluster pairs two rows across a **non-drive** gap. The SNODE stake and all its cables stay inside that gap.
- Drive lanes may only be the gaps **between** clusters (rows 2–3, 4–5, 6–7, 8–9 in a 10-row grove). So no cable ever crosses a drive lane, and no conduit is needed for vehicle loads.
- Each probe goes on the side of its tree facing the cluster's internal gap, so wheels in a drive lane never run over probes.
- Whether a car or tractor fits in a 2 m gap is an owner question (round 9).

### D12 Enclosure
- UV-stabilized polycarbonate box, IP68 class. Reference part: Hammond 1554QA2GY, 140 x 140 x 90 mm, £40.42 (RS UK, single unit). A cheaper equivalent is to be sourced; this is the largest per-node item found so far.
- Antenna **inside** the box: the XIAO's flexible antenna stuck under the lid. Nothing visible outside. Polycarbonate should pass 2.4 GHz with little loss (UNVERIFIED; walk test).
- All cables enter through the **bottom face only**, so water runs off and doesn't pool at the glands.
- One membrane vent (e.g., GORE Protective Vent) to equalize pressure and reduce condensation in daily temperature swings.

### D13 Cable entry
- 21 cables (20 probes + 1 DS18B20) through 6 M25 glands with 4-hole inserts (LAPP SKINTOP DIX 4 x 5 mm, rated -40 to +100 °C; €1.34–1.36 per insert). Unused holes closed with sealing plugs to keep the IP68 rating.
- Fit check needed: Cat5e is about 5.5 mm and the inserts are sized for 5 mm; the 5–7 mm range of other multi-hole glands may fit better. Test in the POC.
- Considered and deferred: one Cat5e serving 2 adjacent trees with a splice at the first tree. That halves the entries and cuts cable by about a third, but adds 300 field splices. Revisit with the custom bus probe (A3).

### D14 Stake and snow
- Box mounting height = design snow depth + 0.1 m. **Standard stake heights: 0.3 m (little snow), 0.6 m and 1.0 m (snow kits).** This site: 0.3 m.
- Variant A panel: mounted above the box on the same stake, tilted 60° or steeper facing the sun, so snow slides off.
- Stake material: non-metal near the antenna (fiberglass or HDPE preferred). Part to be selected.

### D15 Burial (installer instructions)
- Trench along the center of the cluster's internal gap, 15–20 cm deep (owner can hand-trench). Short branch trenches of about 1 m to each tree.
- Optional conduit (rodent areas) on the main trench run; the branches stay direct-burial.
- Leave about 0.3 m of slack at each probe as a loop. This allows for frost heave and lets a probe be pulled and replaced without pulling the cable.
- Before inserting a probe, locate the drip line (surface or buried) and keep the probe at the standard offset from the emitter, away from the line itself.

### D16 Probe end
- The v1.2 board's electronics end is spliced to the Cat5e (3 conductors) and potted, with adhesive-lined heat-shrink over the splice. The potting compound must be rated for the -40 °C design range (part to be selected).
- Service = pull the probe, cut, re-splice. A waterproof connector per probe would make service easier but adds cost x 600. Owner to choose (round 9).

## Receiver
- The trailer is fiberglass, which passes 2.4 GHz with little loss (UNVERIFIED; check in the walk test). The receiver ESP32 can stay inside, with a short coax to the roof antenna.

## Open questions (round 9)
1. Drive lanes: are they normal 2 m gaps, or wider gaps (a row left out)? Is "every 2 rows" (between clusters) enough?
2. Stake heights 0.3 / 0.6 / 1.0 m for the product: OK, or do you want other snow classes?
3. Any larger animals (deer, rabbits, livestock) that could chew cables or knock boxes?
4. Probe-to-cable: potted splice (cheapest, re-splice to service) or a waterproof connector per probe (easier service, more cost)?
