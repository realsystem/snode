# Firmware (ESPHome)

| File | Board | Test | Purpose |
|---|---|---|---|
| `esp8266-walktest.yaml` | NodeMCU (ESP8266) | T11, T1 | Direct Wi-Fi walk test (RSSI, channel, connect times, cycle success rate); A0 sensor drift logger |
| `espnow-tx-esp32cam.yaml` | ESP32-CAM | T12 (optional) | ESP-NOW field sender (use this one): counter every 10 s, LED flash on acknowledgement, channel follow |
| `espnow-tx-esp8266.yaml` | NodeMCU (ESP8266) | — | Compiles, but its unicast packets got no acknowledgement from the ESP32 receiver (UNVERIFIED cause). Not for range tests |
| `espnow-rx-esp32cam.yaml` | ESP32-CAM | T12 (optional) | ESP-NOW receiver on the farm Wi-Fi: counter, RSSI, sender channel → HA |
| `secrets.example.yaml` | — | — | Template for `secrets.yaml` (never commit the real one) |

Status: `espnow-rx-esp32cam.yaml` and `espnow-tx-esp8266.yaml` compiled on the first try (ESPHome 2026.11.0); a one-hour desk run with an ESP32-CAM sender lost no packets (`docs/poc-espnow-bench.md`). `espnow-tx-esp32cam.yaml` and `esp8266-walktest.yaml` are not compiled yet: UNVERIFIED.
Why this split: ESPHome supports ESP-NOW on ESP32 and ESP8266, but on the ESP8266 the received signal strength is always 0, so the receiver must be the ESP32-CAM.

## Windows PC setup (once)
These steps can be done by hand or by Claude in the terminal on the PC.
1. Install Python 3.12 or newer from python.org. Tick **Add Python to PATH**. Check: `py --version`.
2. Install Git for Windows. Then: `git clone https://github.com/realsystem/snode` and `cd snode\firmware`.
3. Make a virtual environment and install ESPHome:
   ```
   py -m venv %USERPROFILE%\esphome-venv
   %USERPROFILE%\esphome-venv\Scripts\activate
   pip install esphome
   esphome version
   ```
4. USB driver: plug in the NodeMCU. In Device Manager > Ports, note the COM number. If no port appears, install the driver for the USB chip printed on the board (CP2102: Silicon Labs; CH340: WCH).
5. Secrets: `copy secrets.example.yaml secrets.yaml`, then edit it. Make each API key with:
   `py -c "import secrets,base64;print(base64.b64encode(secrets.token_bytes(32)).decode())"`

## Build and flash
```
esphome config esp8266-walktest.yaml      # check the YAML
esphome compile esp8266-walktest.yaml     # build
esphome run esp8266-walktest.yaml --device COM3   # build + flash over USB (use your COM number)
esphome logs esp8266-walktest.yaml --device COM3  # serial log
```
- NodeMCU: remove the D0–RST wire while flashing over USB; put it back for cycle mode.
- ESP32-CAM: it has no USB of its own. Use an ESP32-CAM-MB adapter, or a 3.3 V USB-serial adapter with IO0 tied to GND during flashing (remove the tie and reset to run).
- If a line marked "if this does not compile" fails: delete that sensor (walk test), or in the sender replace the `set_channel_from_action` line with a fixed channel and remove channel follow. Note it in `docs/01-interview-log.md`.

## Add to Home Assistant (at the farm)
When a board is on the farm Wi-Fi: HA > Settings > Devices & services > ESPHome (discovered) > Configure > paste that board's API key from `secrets.yaml`.

## T11: walk test (direct Wi-Fi, N1)
1. Continuous mode (no jumper): at each spot, hold the board 0.3 m above ground for 1 minute. Note the time and the spot. Read RSSI and channel from HA history.
2. Cycle mode (jumper D5–GND and wire D0–RST): leave it at the worst spot for 30 minutes or more.
   - Success rate = boot numbers received ÷ (last − first + 1). Missing numbers are failed reports.
   - Note "Boot to Wi-Fi" and "Boot to HA". Repeat once with the static IP block enabled.
3. Pass mark (`docs/07-poc-tests.md`, T9): ≥ 98% delivered, median awake time < 2 s.

## T12 (optional): ESP-NOW range test (N2)
1. Flash the receiver first and read "Receiver MAC" in HA (or the boot log). Put it in `receiver_mac` in `espnow-tx-esp32cam.yaml`, then flash the sender (ESP32-CAM: hold IO0 to GND at power-up).
2. Receiver: inside the trailer where a real receiver would sit, on USB power.
3. Sender: on a power bank, 0.3 m above ground. The LED flashes about every 10 s while packets are acknowledged. 5 minutes per spot, same spots as T11. Note the time and spot.
4. In HA: "Sender RSSI" per spot; loss from "Sender counter" vs "Packets received". "Sender channel" shows channel follow at work.
5. Channel follow check: change the router's channel during the test, if the router allows it (not done yet). The LED should stop for up to about 4 minutes, then flash again on the new channel.
6. A sender that starts on the wrong channel needs about 170 s to lock (measured). Start it a few minutes before the first spot.

## T1: drift logger
Walk-test firmware, continuous mode, old moisture sensor on A0 (NodeMCU A0 range 0–3.3 V), in a pot of site soil at constant moisture for 1 week.

## Node firmware (`node/`) and POC receiver (`receiver/`)
| File | Board | Purpose |
|---|---|---|
| `node/snode-node-base.yaml` | XIAO ESP32C3 | Shared node logic (docs/08-node-design.md) |
| `node/snode_f_01.yaml` | XIAO ESP32C3 | Example node: name, grove, number, receiver MAC, probe addresses |
| `components/rv3028/` | — | ESPHome external component for the RV-3028-C7 RTC (time, alarm) |
| `components/snode_bus/` | — | ESPHome external component: powered-bus master, protocol v1 (READ, DISCOVER) |
| `receiver/snode-receiver-poc.yaml` | ESP32-CAM or XIAO | Receives reports, replies with time and next wake, sends HA events |

Build: `esphome run node/snode_f_01.yaml --device COMx` and `esphome run receiver/snode-receiver-poc.yaml --device COMx`.
Bench: probe serials come from install mode (button wake → HA event `esphome.snode_install`); paste them into the
node file `snode_bus` slot lists in row order.
Not compiled yet. Lines most likely to need fixes on the first compile: `set_channel_from_action`, `wait_for_sent`,
the `std::array` globals, strapping-pin UART RX, and the two external components (ESPHome API names).
Log any changes in `docs/01-interview-log.md`.
