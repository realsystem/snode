# Original design brief (verbatim from owner, 2026-11-23)

This is the prompt that started the project. The working rules in it are also in `CLAUDE.md` section 4.

```text
ROLE
You are helping me design a low-cost DIY soil moisture sensor system for my
remote off-grid farm. Work as an engineer who interviews me. Do not design in
one shot.

WORKING RULES (strict)
- Use only verified data. If a fact is not in this prompt, ask me; don't assume.
- Mark every claim as: VERIFIED (from me or a cited source), or UNVERIFIED
  (needs a test or my confirmation).
- Research engineering questions on the web and cite sources (manufacturer
  datasheets, manuals, and tested comparisons first). Give links.
- Be precise and agile: ask short batches of questions (max 4 at a time), then
  move forward. Show options with trade-offs, then recommend one and say why.
- Monitoring only, no irrigation control, unless I ask later.

SITE AND SYSTEM CONTEXT (VERIFIED, from me)
- Location: [removed at owner's request]. Cold winters, soil can freeze. Site visited
  about every 3 weeks. Nobody on site otherwise.
- Home Assistant (HA) runs on a Raspberry Pi 5 in a trailer. Starlink internet
  (notifications need internet). Wi-Fi in the trailer. Victron devices report
  to HA over Bluetooth.
- I have an Ecowitt GW3000 gateway, which has 16 channels for soil sensors.
  I will use some of those channels with Ecowitt soil sensors, but I need many
  more sensors than that, and buying Ecowitt soil sensors for all of them costs
  too much. So I need DIY sensors in addition.
- I also have a separate project: a self-powered "UNODE" monitoring node
  (ESP32 or similar, own battery + small solar panel, logs to HA). It is being
  designed in a different thread. Your design may share ideas with it, but do
  not depend on it. Tell me if sharing would help or hurt.

WHAT I DON'T KNOW YET (ask me, in this order of importance; do not guess)
- How many DIY sensors I need, and how they are spread out (area, spacing,
  distances to the trailer, line of sight, buildings or terrain between).
- What is planted or what the sensors are for (crops, trees, beds, pasture),
  the soil types, the depths to measure, and how I'll use the data (alerts,
  irrigation decisions, logging only).
- How the area is watered, and what is installed now.
- Whether mains or solar power is anywhere near each sensor location.
- Budget per sensor and total.
- Whether I want the Ecowitt sensors and DIY sensors shown together in one
  place in HA, and whether the DIY sensors must appear in the Ecowitt app
  (probably not).
- How long between visits sensors must run without service (3 weeks minimum?
  a full season?).
- Anything I have already bought or tried.

WHAT I WANT YOU TO DESIGN
1. Requirements list (accuracy needed, depths, range, battery life, freezing,
   burial, cost per sensor).
2. Sensing element options, compared with sources: capacitive vs resistive vs
   other types (e.g. TDR-style). Include: corrosion, salinity effects,
   temperature effects, freezing soil behavior, calibration per soil type,
   and known problems with cheap capacitive boards (for example, are
   waterproofing and coating needed, and which supply voltage and ADC
   behavior matter). Recommend one, and say how I should calibrate and verify
   it, including against my Ecowitt sensors.
3. Node design options per sensor, compared on power draw, range, HA
   integration, parts availability, and cost: ESP32 on Wi-Fi, ESP-NOW,
   LoRa, Zigbee, BLE, or sending the DIY data into the Ecowitt gateway
   (research whether this is possible, and say plainly if it is not).
   Show how readings reach HA and how many sensors one link can handle.
4. Power per sensor: battery chemistry, solar or no solar, sleep and wake
   schedule, and a power budget with arithmetic and stated assumptions for the
   required runtime. Include cold-weather battery behavior.
5. Cabling or wireless layout: a single multi-sensor node with several probes
   vs one node per probe, with the cost and reliability trade-off.
6. Enclosure and burial: waterproofing, probe mounting, rodents, UV, frost
   heave, cable protection, how to pull a sensor out for service.
7. Firmware and HA integration: ESPHome if suitable, entity naming,
   calibration values stored in one place, alerts to define (dry, wet,
   sensor offline, battery low, frozen soil).
8. Bill of materials with part numbers, per-sensor cost and total cost for my
   quantity, using only prices from sources you can cite.
9. Test plan: how to validate one prototype for a few weeks before building
   many, including a comparison against an Ecowitt sensor in the same soil.
10. Open questions for me, ranked by what blocks the design.

OUTPUT FORMAT
End with a spec-ready section I can paste into my spec:
- a decisions table (decision, choice, reason, source),
- an open items list,
- a test list,
- the BOM with per-sensor and total cost.
Keep it plain, concise, no marketing language.

START
Do not design yet. First list the 4 most blocking questions for me.
```
