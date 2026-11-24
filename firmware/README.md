# Firmware

| File | Device | Purpose |
|---|---|---|
| `esp8266-walktest.yaml` | ESP8266 (D1 mini or NodeMCU) | POC walk test (T11) and sensor drift logger (T1) |
| `secrets.example.yaml` | — | Template for `secrets.yaml` (never commit the real one) |

Status: written without compiling (no ESPHome in the writing environment). UNVERIFIED until the first compile.

## Use: walk test (direct Wi-Fi, N1)
1. ESPHome dashboard (HA add-on) > New device > paste the YAML. Set `board` (d1_mini or nodemcuv2). Install over USB.
2. Bench check, continuous mode (no jumper): entities appear in HA; RSSI updates every 10 s.
3. Field, continuous mode: at each test spot, hold the board 0.3 m above ground for 1 minute. Note the time and the spot (grove, corner). Read RSSI and channel from HA history afterwards.
4. Field, cycle mode (jumper D5–GND and wire D0–RST): leave the board at the farthest spot for 30 minutes or more (about 30 cycles).
   - Success rate = boot numbers received ÷ (last − first + 1). Missing numbers are failed reports.
   - Note "Boot to Wi-Fi" and "Boot to HA" times; repeat with a static IP.
5. Compare against the T9 pass mark (`docs/07-poc-tests.md`): ≥ 98% delivered, median awake time < 2 s.

## Use: drift logger (T1)
Continuous mode, sensor on A0, in a pot of site soil at constant moisture for 1 week. "Soil raw A0" every 60 s.
