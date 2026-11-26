# ESP-NOW receiver bench test (T12, desk part)

Bench test of the shared ESP-NOW receiver design (design log D30, D31) on a Windows PC, two ESP32-CAM boards
(AI-Thinker type) and a NodeMCU ESP8266. Home Assistant (HA) was used only for integration.
Camera not used for this test. Labels: **VERIFIED** = measured or seen in this test, **UNVERIFIED** = not measured or only inferred.

Status: desk part done. The walk test and the router-channel-change test are **not done** (see Open items).

## Setup

| Item | Detail |
|---|---|
| Tool | ESPHome 2026.11.0 (ESP-IDF 5.5.6), Windows 10 |
| Receiver | `firmware/espnow-rx-esp32cam.yaml` on an ESP32-CAM, joined to the home router (its ESP-NOW channel = the router's channel), reports to HA over the API with encryption |
| Sender | `firmware/espnow-tx-esp8266.yaml`, logic ported to a second ESP32-CAM (see below), counter every 10 s, channel follow (2 no-acks in a row, then next channel 1..11) |
| Distance | 1 to 3 m apart, line of sight |
| Receiver channel | Router channel 4. Sender settled on channel 3 (see findings) |

Local changes used for the run (not committed; secrets and the receiver MAC are not in the repo): the sender built from
the ESP8266 file with the board changed to ESP32-CAM, status LED on, DEBUG logging; the receiver with DEBUG logging,
a log line per packet and an LED flash per packet.

## Compile

| Step | Result |
|---|---|
| `espnow-rx-esp32cam.yaml` | Compiled on the first try, no fix needed (VERIFIED) |
| `espnow-tx-esp8266.yaml` | Compiled on the first try, no fix needed (VERIFIED). `set_channel_from_action` compiles and runs |
| No compile-fix PRs were needed for these two files | |

## Camera check (UNODE vision add-on, same boards)

The same ESP32-CAM hardware was bench-checked in the UNODE repo before this test. Results and the weak-Wi-Fi fix
(camera clock 8 MHz, Wi-Fi power save off) are in the UNODE `docs/vision-addon.md`. UNODE pull requests 10, 11 and 12
(https://github.com/realsystem/unode/pull/10, /11, /12). VERIFIED there.

## Results

### 1. One-hour bench run, sender and receiver 1 to 3 m apart (VERIFIED)

| Item | Receiver log | Sender log |
|---|---|---|
| Counters | 117 to 477 | 118 to 479 |
| Packets | 361 expected, 361 received, no gaps | 362 sent, 362 acknowledged |
| No-acks, channel hops | n/a | 0, 0 |
| Packet loss = 1 − received ÷ (last − first + 1) | **0.0 %** | n/a |
| RSSI at receiver (dBm) | min −59, median −48, max −45 | n/a |
| Channel | 3 for the whole hour | 3 |

The first one-hour run read 1.66 % loss (6 packets in one gap). That gap lined up with a dropped connection of the
logging script, and the second run, with the sender log available, showed none (consistent, not proven).
The receiver log ends two sends before the sender's last, which is a logger cutoff.

### 2. Receiver reboot (VERIFIED)

The receiver was restarted while the sender kept sending. The sender saw 1 no-ack (the packet sent while the receiver
was down), did not change channel (it needs 2 in a row), and the next packet was acknowledged about 22 s after the
reboot began. One more counter was missing in the receiver's log although the sender logged it as acknowledged;
it fell on a logging-script reconnect, likely a logging gap (UNVERIFIED).

### 3. Sender cold start, twice (VERIFIED)

Sender powered up on channel 6, receiver on router channel 4. The sender hopped 6, 7, 8, 9, 10, 11, 1, 2, 3 and locked on 3
with its first acknowledged send at counter 17, both times, after about 170 s. Search time = 10 s × 2 sends × channels
tried, so up to about 190 s. For a node that wakes every 12 h, the node should start from the last good channel
(RTC or flash): design change, UNVERIFIED for the real node.
During the search the receiver also heard some packets the sender counted as no-ack (sent on a neighbour channel,
ack not received). Two of these (counter 2 in run 1, counter 5 in run 2) fell on channels two or more away from the receiver
and are unexplained (UNVERIFIED).

## Findings for the design

| Finding | Status |
|---|---|
| ESP32 to ESP32 unicast ESP-NOW with channel follow works against a receiver that is on the router's Wi-Fi (D30 E1) | VERIFIED on the bench |
| At 1 to 3 m, neighbour channels overlap: the sender locked on channel 3 while the receiver was on 4. At longer range this may not hold: the walk test must show it | VERIFIED at 1 to 3 m, UNVERIFIED beyond |
| ESP8266 as sender: addressed (unicast) sends were never acknowledged on any channel; broadcast from the ESP8266 was received. Cause not found (ESPHome ESP8266 peer or channel handling, the receiver being Wi-Fi associated, or ESP8266 to ESP32 interop) | UNVERIFIED cause. Use an ESP32 sender for ESP-NOW range tests |
| ESP32 receiver reports sender RSSI; the ESP8266 cannot (receive RSSI is always 0), so the receiver must be an ESP32 | VERIFIED in the ESPHome source behaviour seen |
| In the ESP32 receiver, unicast arrives in `on_receive`, broadcast only in `on_broadcast` | VERIFIED |
| ESP32-CAM has no reset pin on the header; flashing needs IO0 to GND at power-up. ESP-NOW channel can only be set with Wi-Fi off, so the sender cannot also be updated over the air while it keeps channel follow | VERIFIED |
| A NodeMCU ESP8266 failed to flash (MD5 mismatch): its Winbond flash was write-protected (status 0xFC). Writing status 0x0200 (non-volatile) with esptool fixed it | VERIFIED |

## Open items

| Item | Status |
|---|---|
| Home walk test (sender on a power bank 0.3 m above ground at several spots, 2 minutes each: RSSI, loss, channel) | **Not done.** Table below to be filled |
| Channel follow when the router's channel changes (receiver moves, sender must follow) | **Not done** (owner did not want to change the router). UNVERIFIED |
| Sender cold-start time of about 170 s versus a remembered channel | Design change proposed, UNVERIFIED |
| ESP8266 unicast no-ack cause | UNVERIFIED |
| Power-bank sender: a power-on brownout line was seen once when 5 V was reconnected; no supply problem seen otherwise | UNVERIFIED |
| Range beyond 3 m, other receiver positions, effect of the case and antenna | UNVERIFIED |

## Walk test (to fill at the next test)

| Spot (description, distance) | RSSI (dBm) | Packets lost in 2 min | Channel |
|---|---|---|---|
| | | | |
