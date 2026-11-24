# Hand-off note for the HA automation threads

SNODE hardware delivers data; the HA farm-automation threads own the logic. This is what HA needs from SNODE and what SNODE expects HA to do.

## What arrives in HA (from the receiver, via ESPHome)
- Per probe: raw moisture count (entity `sensor.soil_raw_<tree id>`), addressed by the probe's 64-bit 1-Wire address.
- Per SNODE: soil temperature (`sensor.soil_temp_snode_<id>`), battery or supercapacitor voltage, RSSI, report time. Reported **together with** the moisture values of the same wake.
- Twice a day: 05:00 and 17:00.

## What HA must do
1. Convert raw → % per probe with `config/calibration.yaml` (dry/wet counts, temperature coefficient). Temperature compensation method T1: use the node's soil temperature for all its probes.
2. Map probes to trees (`config/probes.yaml`, address → tree ID) and trees to irrigation zones (`config/zones.yaml`, planned).
3. Alerts (proposed rules, design log D18): dry, wet, frozen (≤ 0.5 °C: mark invalid and block irrigation), probe fault, node offline (25 h), battery low, weak radio. Delivery method and quiet hours: decided in the HA threads.
4. Irrigation interlock: never open a zone that is frozen or whose data is older than 25 h.

## Irrigation controller
- OpenSprinkler: the main controller has 8 zones; each expander adds 16 (24 with one expander).
- Starting suggestion (PROPOSED, for the HA threads to decide):
  - 8 zones: 2 per grove, split at the V1 drive lane = 6 zones + 2 spare.
  - 24 zones: 1 per row pair = 15 zones + 9 spare.

## Accuracy
- Target ±5% after site calibration, judged mainly on trends (owner accepted).
