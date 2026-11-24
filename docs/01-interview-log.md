# Interview log

Each round: the questions asked (max 4) and the owner's answers verbatim. The
owner's answers are VERIFIED facts. Newest round at the bottom.

## Round 1 — 2026-11-23

Questions:
1. How many DIY sensors, and how are they spread out (area, distances to the trailer, line of sight)?
2. What is planted, soil type, depths, and how will the data be used?
3. Power near sensor locations, required runtime between visits, winter operation?
4. Budget, and what is already bought or tried?

Answers (2026-11-24):
1. "hundreds of trees on a few acres, my area is 5 acres 330x600ft(half of the whole lot), the trailer is in the middle."
2. "trees, we need moisture control."
3. "sensors should be grouped for solar power, need to calculate proper grouping."
4. "I have esp8266, moisture sensors for arduino, central wifi router."

Notes: 330 x 600 ft = 198,000 ft² ≈ 4.5 acres (arithmetic). Farthest corner is about 104 m from the trailer, assuming the trailer is at the center.

## Round 2 — 2026-11-24

Questions:
1. Exact parts on hand (sensor type, ESP8266 board type, router location)?
2. What "moisture control" means, and how the trees are watered now?
3. Tree layout (count, rows, species/age, soil variation)?
4. Runtime through winter, and budget?

Answers:
1. "let's design one SNODE(code name for soil node) based on optimal set of components, not what I have. my current devices can used for POC only. but we need a final product with minimal budget."
2. "monitoring soil moisture reported to central HA which will control irrigation controller(possibly opensprinkler)"
3. "doesn't matter, I want one moisture sensor per tree with SNODE grouped for one solar. should be universal mesh for different patterns."
4. "this is we need to calculate. minimal cost for maximum coverage. distance between trees is about 1m."

Interpretation recorded: "universal mesh" = one SNODE design that works for any planting pattern, not necessarily a radio mesh (radio mesh not recommended; see design log D4).

## Round 3 — 2026-11-24

Questions:
1. Total tree count and row spacing?
2. One probe at one depth per tree? Tree age?
3. Custom PCB acceptable for the final product?
4. Must SNODEs report through winter? Is a 30–60 min report interval enough?

Answers:
1. "lets target 200 with grid 1m by 2m"
2. "one probe one depth"
3. "yes, but after poc"
4. "winter is most important"

## Round 4 — 2026-11-24

Questions:
1. Is the 200-tree block contiguous; rows x trees per row; distance to the trailer?
2. Probe depth / tree age?
3. Burrowing rodents?
4. Drip layout per tree?

Answers:
1. "let's establish terminology that we will use across different related projects: forest(the whole plot), groves(200 trees on the front, the same on the back, the same on the side), grid(1m by 2m), row(calculate it), node(SNODE or UNODE). save this in claude.md template..."
2. "new trees, 6 inch depth max"
3. "yes, that's problem, let's do optional conduit"
4. "drip per tree, one emitter per tree, same offset from emitter"

Result: 3 groves x 200 = 600 trees. Terms recorded in `CLAUDE.md`.

Follow-up (2026-11-24): owner asked to use https://github.com/realsystem/snode as the project memory.

## Round 5 — 2026-11-24

Questions:
1. Grid direction (1 m within a row, 2 m between rows?) and each grove's shape (default 10 rows x 20 trees)?
2. Distance and direction from the trailer to each grove, and anything in between?
3. Is a 30-min report interval acceptable?
4. Accept the row/tree numbering origin proposed in `CLAUDE.md`?

Answers:
1. "1m between trees in a row, 2m between rows. 10x20 is ok"
2. "you calculate, the trailer is in the middle of 330x600ft plot"
3. "once per 12h(night/day)"
4. "ok"

Follow-up instruction (2026-11-24): "do not mention [the site's town and state] in anything goes to github". Location removed from all files; rule added to CLAUDE.md section 1.

Result: worst-case radio link 105 m, so a 2 m mast is needed (design log, radio section). With 2 reports a day, primary lithium AA with no solar becomes the recommended power option (D6), pending the owner's choice.

## Round 6 — 2026-11-24

Questions:
1. D6: no solar (3x lithium AA, replaced about every 2–3 years on a visit), or solar + LiFePO4 as originally planned?
2. Can each SNODE have a 2 m mast? Is the trailer skin metal, and can a receiver antenna be mounted outside it?
3. Report times: about 05:00 and 17:00, or other times?
4. Which Ecowitt soil sensor model do you have?

Answers:
1. "let's design two solutions: solar only(no battery) and battery only(no solar)."
2. "no, too much, antenna can be short small, almost invisible, otherwise the forest will be a forest of antennas."
3. "yes"
4. "gw3000 with wh51"

Result: added RTC power gating (D11), which cuts the load from 2.24 to 0.24 mAh/day. Designed variant A (solar + 10 F supercapacitor, about 6 days without sun) and variant B (3x lithium AA, plan a 5-year swap); B recommended. No field masts: one tall receiver antenna on the trailer roof, small antennas at the nodes. MCU changed to XIAO ESP32C3 for its u.FL antenna. Added a WH51 comparison plan.

## Round 7 — 2026-11-24

Questions:
1. Variant A: is it acceptable to lose readings during long storms or snow cover (more than about 6 days without sun)?
2. Is one tall antenna on the trailer roof (about 5 m) acceptable? Is the trailer skin metal?
3. Enclosure: a small box on a short stake about 0.3 m above ground, or buried with only the antenna showing?
4. How many WH51 sensors do you have, or plan, for the comparison?

Answers:
1. "yes, but it's impossible for my location, we have enough sun. just in case we do this project for commercial we need to provide the limits."
2. "antenna is fine, trailer is standard fiberglass"
3. "box on a stake"
4. "I have 4 and one gw3000, it can hold 16 max, let's keep this numbers"

Also: owner approved a force-push to remove the location from git history.

Result: added an operating-limits table (design log). WH51 placement: one per grove plus a second in the front grove.

## Round 8 — 2026-11-24

Questions:
1. Machine traffic between rows (mower, ATV, tractor)? How wide?
2. Typical and worst snow depth on the ground?
3. Can you hand-trench to about 15–20 cm? Is the soil rocky?
4. Are the drip lines on the surface or buried?

Answers:
1. "every few rows should be drivable by car/tractor"
2. "almost nothing for now but the product should be designed with snow accumulation in mind"
3. "not a problem, put it on user"
4. "I have it on surface but for this product it should not be any matter"

Result: drafted enclosure, mounting and burial (D12–D16): drive lanes only between clusters, antenna inside the box, bottom cable entry through multi-hole glands, stake heights by snow class, installer trench instructions.

## Round 9 — 2026-11-24

Questions:
1. Drive lanes: normal 2 m gaps or wider? Is "every 2 rows" (between clusters) enough?
2. Stake heights 0.3 / 0.6 / 1.0 m for the product: OK?
3. Any larger animals that could chew cables or knock boxes?
4. Probe-to-cable: potted splice or a waterproof connector per probe?

Answers:
1. "the gap should be ok for toyota tundra"
2. "minimal"
3. "rabbits but not a problem for wooden stake and plastic box, the only critical part is conduit from the box to the ground. it's a must have thing."
4. "sealed splice or similar cap used for regular irrigation wires for 24AC"

Interpretation recorded: "minimal" = one standard box height (0.3 m), longer stake for snowy sites.
Conflict found: a Tundra is 80.2 in (2.04 m) wide without mirrors, wider than the 2 m row spacing, so lanes need a wider gap (asked in round 10).
Result: required 1-1/2 in PVC riser from the box to the ground (D13); cable changed to 18/3 irrigation wire so standard direct-bury irrigation splices fit (D16).

## Round 10 — 2026-11-24

Questions:
1. Accept about 3.5 m drive lanes (measure mirror to mirror)? How many lanes per grove?
2. Accept 18/3 irrigation wire with bench-built potted probe assemblies?
3. Riser: 1-1/2 in Schedule 40 PVC, rigid, strapped to the wooden stake. OK?
4. ESPHome acceptable for receiver and nodes, or custom firmware?

Answers:
1. "yes"
2. "yes, no soldering, usual caps for connecting wires, no prep work for user, no bench stuff, just go to the field with spool and bag of connectors(bought from nearest home depot or similar store) and install it."
3. "1/2 standard pvc conduit"
4. "esphome is ok"

Conflict found: a 1/2 in riser can't hold 20 home-run cables (about 213% fill). Only a bus architecture fits. Splice caps cost about $2.88 each, so the cap count per tree drives cost. Proposed a 2-wire digital bus probe for the product (D17).

## Round 11 — 2026-11-24

Questions:
1. Accept the 2-wire digital bus probe for the product (analog for POC only)?
2. Caps at about $2.88 each: acceptable, or look for cheaper?
3. How many drive lanes per grove?
4. Irrigation cable rated to -20 °C vs -27 °C design point: accept or look for colder-rated cable?

Answers:
1. "yes"
2. "need to decrease this cost to a minimum, should be almost nothing, don't rely on standard irrigation wire caps, it maybe an option for a user but the design should be with something simple."
3. "calculate it and provide different variants, user may choose depending on custom needs"
4. "yes"

Result: piercing clamp built into the probe (landscape-light style) on flat 16/2 landscape wire (D16); 1-Wire style bus as the candidate protocol (D17); drive-lane variants V0/V1/V2/V4 calculated; cable cost for the forest drops from $2,125 to $700.

## Round 12 — 2026-11-24

Questions:
1. Piercing clamp built into the probe as the standard connection (caps and retail connectors as options)?
2. 16/2 flat landscape lighting wire as the bus cable?
3. Which drive-lane variant for this site?
4. Target cost per finished bus probe?

Answers:
1. "yes"
2. "compare cost for 16/2 landscape lighting and standard irrigation valve wire"
3. "v1"
4. "I don't know, you tell me the minimal we can get"

Result: cable comparison (16/2 $700 vs 18/2 $559 for the forest; 16/2 kept as standard because the clamp needs no prep). V1 recorded. Minimum bus probe estimated at about $0.45–1.30 (D19).

## Round 13 — 2026-11-24

Questions:
1. Confirm 16/2 landscape wire as standard?
2. Expected commercial volume (decides housing method)?
3. Next: POC shopping list and test plan, or finish the remaining design sections first?
4. Probe temperature compensation: one DS18B20 per node, or a sensor in every probe?

Answers:
1. "yes, keep the other one as backup(user choice)"
2. "3d print in house for POC and small batches is acceptable, after that we can order proper molded enclosures, find where and keep a note for future."
3. "finish design first, ask questions, we don't need shopping list now, but need a clear design and POC"
4. "explain"

Result: D20 temperature compensation explained (T1/T2/T3; T2 recommended), D21 housing path (ASA 3D print now; Protolabs/RapidDirect molding later), HA alerts and irrigation interlock, `docs/04-spec.md` and `docs/05-poc-plan.md` written.

## Round 14 — 2026-11-24

Questions:
1. Temperature compensation: T2 (recommended), T1 or T3?
2. Irrigation zones: count, and which trees/rows per zone?
3. Alert delivery (app push, email) and quiet hours?
4. Accuracy target ±5% after site calibration, judged on trends?

Answers:
1. "T1 and let's report temperature with soil moisture to HA"
2. "let's assume standard opensprinkler without extension for start and with one for later."
3. "we have separate threads about HA automation for the farm, let's leave a note here about it and move to hardware."
4. "yes"

Result: T1 recorded; `docs/06-ha-note.md` written for the HA threads; hardware drafted: bus electrical design with DS18B20 emulation (D22), node power tree (D23), probe hardware (D24).

## Round 15 — 2026-11-24

Questions:
1. Probe polarity: bridge rectifier or keyed clamp?
2. Probe sensing depth (center of the sensing zone)?
3. Node enclosure: 3D-printed ASA or bought box?
4. POC tools on hand (µA meter, enclosed 3D printer, soldering station, chest freezer)?

Answers:
1. "ok" (bridge rectifier)
2. "6 inches"
3. "3d in house for POC"
4. "yes"

Result: bus master DS2482-100 x2 (D22), variant A parts (10 F supercap, TPS61099), probe mechanics (D25), 3D-printed node enclosure (D26). Depth interpretation to be confirmed.

## Round 16 — 2026-11-24

Questions:
1. Sensing zone 5–15 cm (bottom at 6 in), or centered at 6 in?
2. Bus cable in a shallow slit at about 5 cm along the tree line?
3. POC: both power variants, or B first?
4. Receiver in a roof box (USB power down) or inside with coax to a roof antenna?

Answers:
1. "6 in bottom"
2. "use irrigation wire standard for US"
3. "both need POC"
4. "box anyway, support indoor/outdoor with optional solar"

Interpretation recorded: answer 2 = burial depth per US irrigation practice (6 in cover for ≤ 30 V), with a rise loop to each probe head. To confirm in round 17.
Result: D25 updated, receiver design D27 (fixed Wi-Fi channel requirement, R-USB / R-solar), POC plan covers both variants and both receiver modes; forest cable +590 ft.

## Round 17 — 2026-11-24

Questions:
1. Burial interpretation: depth (6 in + rise loop) or cable type?
2. Can the trailer router's 2.4 GHz channel be fixed?
3. Receiver solar option with LiFePO4 18650 + cold-cut-off charger?
4. Freeze design v0.1 and start the POC?

Answers:
1. "I mean keep the cable type just use same standard used for irrigation cable. in our case I don't understand loop you mention, explain."
2. "why do we need receiver?"
3. "depends on 2)"
4. "let's continue cover all open items"

Result: loop explained with a sketch, and a foot-clamp alternative proposed (D25). Receiver need analysed (D27): energy is a minor factor; range/setup decides; test direct Wi-Fi first. Closed: variant A charger (BQ25570, D28), RTC in ESPHome (D29), bus read rule.

## Round 18 — 2026-11-24

Questions:
1. Foot clamp (B, recommended) or rise loop (A)?
2. Test direct Wi-Fi first, receiver as fallback?
3. Router location; can its channel be fixed?
4. UNODE as receiver/bridge, or SNODE fully independent?

Answers:
1. "can we just put main cable flat 6 in deep and the probe should has its own short cable to connect to the main one? simple operation with the same clamp under ground."
2. "yes, prepare POC tests in a separate small doc and schedule task for me on [visit date removed]"
3. "inside, in the middle of the trailer. but we need to support any type of user choice. provide few examples how to do it from scratch and reuse currently installed one."
4. "optional"

Result: option C (probe lead + clamp) adopted (D25). POC checklist written (`07-poc-tests.md`), with a site survey (T11) for the next visit; a reminder was scheduled outside the repo. Network setups E1–E6 (D30); Ethernet receiver is the default. UNODE as receiver: optional.

## Round 19 — 2026-11-24

Questions:
1. Emitter-to-probe offset (cm), and which side of the row the cable runs?
2. Starlink router generation, and a free LAN port?
3. Write ESP8266 walk-test firmware before the next visit?

Answers:
1. "research and let's discuss"
2. "starlink mini right now, but don't assume all users have it, they may use different ISP, different LAN solutions. my own example E1 should be based on wifi, I don't have enough ethernet ports."
3. "yes, prepare firmware with enough data to cover what we can by esp8266."

Result: probe position options P1/P2 and cable side K1/K2 (D25, draft lead 40 cm). E1 is now a Wi-Fi receiver with channel follow (D30, D31). Firmware `firmware/esp8266-walktest.yaml` written (not yet compiled).

## Round 20 — 2026-11-24

Questions:
1. Probe position P1 or P2; current emitter distance from trunk; drip lateral on surface?
2. Cable side K2 or K1?
3. ESP8266 board type?
4. Second board for an ESP-NOW range test?

Answers:
1. "P1. emitter is almost near the trunk"
2. "K2"
3. "nodemcu. I havae setup local windows pc with claude in terminal to compile and upload. just prepare steps and code."
4. "yes, I have few esp8266 and few esp-cam if they can also be used for something, research this as optional."

Result: P1 + K2 decided (probe 10 cm from the emitter on the cable side; lead 40 cm draft). Walk test set to `nodemcuv2`. Windows steps in `firmware/README.md`. Optional T12 ESP-NOW range test: NodeMCU sender + ESP32-CAM receiver (D32). Drip lateral position still open (next-visit check).

## Round 21 — 2026-11-24

Questions:
1. Next design step: (a) node schematic + firmware, (b) probe PCB + bus firmware, (c) receiver firmware?
2. ESP32-CAM type and USB adapter?
3. First-compile results.

Answers:
1. "a"
2. "not sure, will check later"
3. "ok"

Result: node design v0.1 (`docs/08-node-design.md`, D33); node firmware, RV-3028 component and POC receiver in `firmware/`. Not compiled yet.

## Round 22 — 2026-11-24

Questions:
1. Write the bench POC parts list now (with cited prices)?
2. Next design step: probe PCB + PY32 firmware, or final receiver?
3. ESP32-CAM type and USB adapter (from round 21).

Answers:
1. "yes"
2. "probe"
3. (no answer)

Result: bench parts list in `05-poc-plan.md`. Probe design v0.1 (`09-probe-design.md`, D34). New finding: the 1-Wire pull-up can't power 10 awake probes; powered-bus option O2 proposed (D35). Portable probe firmware written and host-tested.

## Round 23 — 2026-11-24

Questions:
1. Bus: O2 powered bus (recommended) or O1 1-Wire?
2. Bench parts list OK to order, or changes?
3. ESP32-CAM type and USB adapter.

Answers:
1. "O2, but are there ready to use devices for this purpose?"
2. "make sure all parts available in US, do not order, just prepare list with pricing."
3. (no answer)

Result: O2 decided (D35). Ready M-Bus chips and masters reviewed and not used (D36). US-only bench list, $125.06 cited (05-poc-plan). Probe MCU US availability question raised (D37).

## Round 24 — 2026-11-24

Questions:
1. Production probe MCU: ATtiny202/402 (US) or PY32F002A (LCSC)?
2. O2 bus at about 5 V: OK?
3. ESP32-CAM type and USB adapter.

Answers:
1. "let's note it"
2. "ok if no change for our initial power estimation"
3. (no answer)

Result: both MCUs noted, decision after bench tests (D37). O2 bus parameters set; energy check shows no change (25 µAh/wake worst case vs 26 µAh; budget 0.24 mAh/day unchanged) (D38).

## Round 25 — 2026-11-24

Questions:
1. Next step: node revision for O2 (recommended), probe hardware layer + protocol, or final receiver?
2. ESP32-CAM type and USB adapter.

Answers:
1. "a"
2. (no answer)

Result: node design v0.2 for the powered bus (D39), bus protocol v1 (`10-bus-protocol.md`, D40), `snode_bus` ESPHome component, node and receiver firmware updated. Found: ATtiny202 has one comparator; the probe needs two functions on it.

## Round 26 — 2026-11-24

Questions:
1. Probe step: which MCU path for the comparator question (ATtiny202 shared / PY32F002A / research a US part with 2 comparators)?
2. Or the final receiver first?
3. ESP32-CAM type and USB adapter.

Answers:
1. "can you check aliexpress for parts that we need?"
2. (no answer)
3. (no answer)

Result: AliExpress check (D41, table in `05-poc-plan.md`). PY32F002A dev boards and chips are deliverable to the US from AliExpress; proposed for bench probes. Mixed bench list $119.92.

## Round 27 — 2026-11-24

Questions:
1. AliExpress delivery counts as "available in the US"? (decides PY32 vs ATtiny for bench probes)
2. Next step: probe firmware (recommended) or final receiver?
3. ESP32-CAM type and USB adapter.

Answers:
1. "yes"
2. "later"
3. "later"
Also: "did you check amazon as a supplier?"

Result: bench probe MCU = PY32F002A (D42). Amazon check (D43): L91 24-packs, 16/2 wire (pure copper only) and the XIAO 3-pack are worth buying there; mixed bench list $118.94 (corrected: the XIAO 3-pack is $13.99 from Seeed's US warehouse; owner saw $16.99 on Amazon; $13.49 is the China-warehouse price).

## Round 28 — open (asked 2026-11-24)

1. Next step when ready: probe firmware (PY32) or final receiver?
2. 16/2 wire: require pure copper (no CCA)?
3. ESP32-CAM type and USB adapter (deferred).
