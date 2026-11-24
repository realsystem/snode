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

## Round 10 — open (asked 2026-11-24)

1. Accept about 3.5 m drive lanes (measure mirror to mirror)? How many lanes per grove?
2. Accept 18/3 irrigation wire with bench-built potted probe assemblies?
3. Riser: 1-1/2 in Schedule 40 PVC, rigid, strapped to the wooden stake. OK?
4. ESPHome acceptable for receiver and nodes, or custom firmware?
