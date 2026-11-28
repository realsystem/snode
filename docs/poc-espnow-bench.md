# ESP-NOW receiver bench test (T12, desk part)

Bench test of the shared ESP-NOW receiver design (design log D30, D31) on a Windows PC, two ESP32-CAM boards
(AI-Thinker type) and a NodeMCU ESP8266. Home Assistant (HA) was used only for integration.
Camera not used for this test. Labels: **VERIFIED** = measured or seen in this test, **UNVERIFIED** = not measured or only inferred.

Status: desk part and home walk test done. The router-channel-change test is **not done** (see Open items).

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

## Home walk test (VERIFIED, measured; receiver left in place beside the PC)

Sender on a USB power bank, 2 minutes per spot (4 minutes for the last two), receiver logged over the API.
Packets lost = sends expected minus counters received. Height 0.3 m except spot 2.

| Spot | Description, distance, height | RSSI median (min / max), dBm | Lost in window | Channel |
|---|---|---|---|---|
| 1 | same room, 1 m | -69 (-73 / -54) | 0 of 13 | 3 |
| 2 | same room, 3 m, about 1.3 m high | -64 (-65 / -62) | 0 of 13 | 3 |
| 3 | next room, 10 m | -70 (-72 / -69) | 0 of 13 | 3 |
| 4 | downstairs, 10 m | -68 (-72 / -67) | 0 of 13 | 3 |
| 5 | outside near the house, 15 m | -79.5 (-82 / -77) | 0 of 14 | 3 |
| 6 | backyard on the ground, 20 m | -80 (-81 / -80) | 0 of 13 | 3 |
| 7 | street near parked cars, 30 m, first try | -92 (1 packet) | about 11 of 12 | 4 (sender was searching) |
| 8 | street near parked cars, 20 m | -93 (-94 / -89) | 0 of 14 after a 106 s outage | 4 |
| 9 | street near parked cars, 30 m, second try | -93 (-95 / -91) | about 22 of 25 in 245 s | 4 |

Walls and floors between the spots and the receiver were not counted (UNVERIFIED).

Reading:
- Reliable up to about 20 m through walls, a floor and a yard (spots 3 to 6). The link edge is around -93 dBm.
- RSSI did not follow distance at short range (1 m read weaker than 3 m or 10 m). Antenna orientation, power bank and cable position matter more than distance there. Not isolated (UNVERIFIED).
- At -93 dBm the link works at 20 m (spot 8, 14 of 14) but is mostly lost at 30 m (spot 9): the same RSSI, different result.
- Channel-follow side effect: at that margin 2 no-acks in a row happen by chance. Each one moves the sender off its channel, and a full cycle costs 100 to 220 s of no data (spots 7 to 9). The sender's own log was not available on the power bank, so this is inferred (UNVERIFIED). Proposed: more no-acks before hopping, or retry the same channel first.
- The sender stayed on channel 3 through spot 6 and was on 4 (the receiver's channel) after the hop at spot 7.
## Sender v2: retry first, hop later, remember the channel (bench-tested, marginal link NOT retested)

`firmware/espnow-tx-esp32cam.yaml` changed after the walk test: retry the same packet every 1 s, hop only after 6 failed tries
in a row on one channel (was 2 no-acks), and store the last acknowledged channel in flash and use it at boot.
The committed file compiles with the placeholder receiver address (VERIFIED). A retried packet keeps its counter,
so loss must be computed from distinct counters.

| Test | Result |
|---|---|
| Cold start from channel 6, receiver on 4 | Hopped 6 to 3 and was acknowledged on report 5, about 40 to 45 s (v1: about 170 s). VERIFIED |
| Restart after a lock | `boot: starting on channel 3 (last good channel)`, report 1 acknowledged at once, no search. VERIFIED |
| Marginal link, street 20 m and 30 m | **Not done.** The 30 m spot gave no packets for 11 minutes (out of range that day), so it could not compare the rules. The 20 m retest was not run. UNVERIFIED that v2 removes the 100 to 220 s outages |
| Receiver coming back into range | First packet at -92 dBm on channel 4, then the sender held channel 5 (a neighbour of the receiver's 4) at -75 dBm. Sender log not available (power bank), hops inferred. UNVERIFIED |
| Boot log after one power-up showed `POWERON_RESET` followed by `RTCWDT_RTC_RESET` | Cause not found; other boots showed one reset. May be a contact bounce on the 5 V plug. UNVERIFIED |

Next test: the same marginal spots at the real site, with the sender logging.

## Findings for the design

| Finding | Status |
|---|---|
| ESP32 to ESP32 unicast ESP-NOW with channel follow works against a receiver that is on the router's Wi-Fi (D30 E1) | VERIFIED on the bench |
| Neighbour channels overlap: the sender locked on channel 3 while the receiver was on 4, up to 20 m (walk test). On a marginal link it later moved to channel 4 | VERIFIED up to 20 m |
| ESP8266 as sender: addressed (unicast) sends were never acknowledged on any channel; broadcast from the ESP8266 was received. Cause not found (ESPHome ESP8266 peer or channel handling, the receiver being Wi-Fi associated, or ESP8266 to ESP32 interop) | UNVERIFIED cause. Use an ESP32 sender for ESP-NOW range tests |
| ESP32 receiver reports sender RSSI; the ESP8266 cannot (receive RSSI is always 0), so the receiver must be an ESP32 | VERIFIED in the ESPHome source behaviour seen |
| In the ESP32 receiver, unicast arrives in `on_receive`, broadcast only in `on_broadcast` | VERIFIED |
| ESP32-CAM has no reset pin on the header; flashing needs IO0 to GND at power-up. ESP-NOW channel can only be set with Wi-Fi off, so the sender cannot also be updated over the air while it keeps channel follow | VERIFIED |
| A NodeMCU ESP8266 failed to flash (MD5 mismatch): its Winbond flash was write-protected (status 0xFC). Writing status 0x0200 (non-volatile) with esptool fixed it | VERIFIED |

## Open items

| Item | Status |
|---|---|
| Walk test beyond 20 m: link is marginal at 30 m. Not repeated with sender logging | Partly done, UNVERIFIED beyond 20 m |
| Hop rule: v2 (6 tries, stored channel) is written and bench-tested; retest at a marginal link at the real site | UNVERIFIED |
| Channel follow when the router's channel changes (receiver moves, sender must follow) | **Not done** (owner did not want to change the router). UNVERIFIED |
| Sender cold-start time of about 170 s versus a remembered channel | Design change proposed, UNVERIFIED |
| ESP8266 unicast no-ack cause | UNVERIFIED |
| Power-bank sender: a power-on brownout line was seen once when 5 V was reconnected; no supply problem seen otherwise | UNVERIFIED |
| Field range: receiver position, case, antenna, ground-level sender in an orchard (home range is not farm range) | UNVERIFIED |
