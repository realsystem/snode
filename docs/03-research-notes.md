# Research notes and sources

Findings in my own words, with links. Prices are as seen on 2026-11-24 and must
be re-checked before purchase. S-numbers are referenced from `02-design-log.md`.

## Sensing

| # | Finding | Source |
|---|---|---|
| S1 | Cheap capacitive "v1.2/v2.0" boards often have one of 3 faults: missing voltage regulator, NE555 used instead of TL555C, or a disconnected output resistor. The version label means nothing; check for a real 662K regulator. A disconnected resistor makes readings change very slowly. | https://www.makerguides.com/capacitive-soil-moisture-sensor-with-arduino/ ; https://forum.arduino.cc/t/something-to-know-about-soil-moisture-sensors/1335790 ; https://www.hackster.io/news/martin-uhlmann-finds-an-epidemic-of-faulty-soil-moisture-sensors-and-comes-up-with-a-quick-fix-c05153bef67e |
| S2 | Resistive probes run on DC, which electroplates the positive electrode away. Powering only during a reading, or alternating polarity, slows this down (one user: about 20 sensors, no corrosion after 6 months). | https://neonaut.neocities.org/blog/2018/resistive-soil-sensors ; https://forum.mysensors.org/post/80569 |
| S3 | Capacitive v1.2 probe 10-pack: $16.95 (about $1.70 each). Board size about 99 x 16 mm, output 0–3.0 V, supply 3.3–5.5 V. | https://pmdway.com/products/analogue-capacitive-soil-moisture-sensor-ten-pack ; https://makerlab.ph/products/capacitive-analog-soil-moisture-sensor-v1-2 |
| S4 | ESP32 touch pins can read a home-made capacitive probe directly (Rezodo project). The main concerns are capacitance range and protecting the MCU input. | https://hackaday.io/project/190577/log/217776-soil-moisture-probe ; https://esp32.com/viewtopic.php?p=54048 |
| S8 | A v1.2 board's operating current is listed as 5 mA (seller spec, UNVERIFIED). | https://robogearsin.odoo.com/shop/9851247-capacitive-soil-moisture-sensor-4371 |
| S13 | Frozen soil: liquid water's permittivity is about 80, soil solids 3–6, ice about 3.8. Capacitive/TDR sensors therefore read frozen soil as dry, and the reading holds steady until it thaws. | https://www.campbellsci.co.uk/faqs?v=1033 ; https://www.campbellsci.com/faqs?v=1802 |
| S20 | Waterproof DS18B20 probes: Aideepen 5-pack $9.49 (about $1.90 each). Range -55 to +125 °C, ±0.5 °C from -10 to +85 °C, 1-Wire bus. | https://hackster.io/products/buy/26534 ; https://www.sparkfun.com/temperature-sensor-waterproof-ds18b20.html |

## MCU and radio

| # | Finding | Source |
|---|---|---|
| S5 | ESP8266 ADC: 0–1 V on the bare chip; NodeMCU/D1 Mini add a 220k/100k divider for about 0–3.2 V. Only one analog input. | https://randomnerdtutorials.com/esp8266-adc-reading-analog-values-with-nodemcu/ ; https://Tasmota.github.io/docs/ADC/ |
| S6 | ESP-NOW field tests (Espressif blog): usable at 150 m in forest; in open fields, low latency up to about 300 m. | https://developer.espressif.com/blog/esp-now-for-outdoor-applications/ |
| S7 | ESP-NOW walk test: about 320 m standard, about 500 m in long-range mode. | https://makiuto.github.io/p/esp-now-range-test-real-world-results-for-esp32-devices.html |
| S9 | TI CD74HC4067 16:1 analog mux: $1.05 chip (SparkFun COM-00299), $6.50 breakout (BOB-09056). On-resistance about 70 Ω typ at 4.5 V; 2–6 V supply. | https://www.sparkfun.com/16-channel-multiplexer.html ; https://www.sparkfun.com/sparkfun-analog-digital-mux-breakout-cd74hc4067.html ; https://ti.com/product/CD74HC4067/part-details/CD74HC4067SM96 |
| S10 | ESP32-C3 SuperMini: about 43 µA deep sleep (standard version). The "Plus" version draws about 0.6–1.5 mA in sleep because of a WS2812B LED on the 3.3 V rail. Price seen: 2 boards for $19.99 (Newegg; likely cheaper elsewhere, not yet cited). | https://randomnerdtutorials.com/getting-started-esp32-c3-super-mini/ ; https://mischianti.org/esp32-c3-supermini-plus-v2-high-resolution-pinout-datasheet-and-specs/ ; https://www.newegg.com/p/3D0-0057-00024 |
| S11 | Zigbee routers send periodic Link Status messages to stay connected, so they cannot deep-sleep. | https://docs.espressif.com/projects/esp-zigbee-sdk/en/latest/esp32c5/faq.html |
| S12 | ESP32-C3 chip: 5 µA deep sleep; TX peak 335 mA (802.11b, 1 Mbps, 21 dBm); modem-sleep 13–28 mA. | https://documentation.espressif.com/esp32-c3_datasheet_en.html ; https://deneyapkart.org/en/docs/guc-tuketimi-esp32-c3 |

## Power and climate

| # | Finding | Source |
|---|---|---|
| S14 | LiFePO4 must not be charged below 0 °C (lithium plating); the -30 to +80 °C range applies to discharge only. | https://us.support.redarcelectronics.com/hc/en-us/articles/13856244101007-Why-you-should-not-charge-a-lithium-battery-below-0-C-or-32-F |
| S15 | Energizer L91 lithium AA: operates -40 to 60 °C, about 3000+ mAh, more than 20-year shelf life. | https://data.energizer.com/pdfs/L91.pdf ; https://data.energizer.com/pdfs/lithiumintrinsicsafety.pdf |
| S16 | LiFePO4 18650 1500 mAh 3.2 V: $8.49 (Zoro, Zeus PCIFR18650-1500). | https://www.zoro.com/zeus-battery-products-32v-1500mah-lifepo4-battery-pcifr18650-1500/i/G611386156/ |
| S19 | Site climate: January about 46 °F highs / 28 °F lows; 100–125 frost days a year. Record low at the nearest NWS station: -17 °F (proxy). December solar at that station about 2.0 kWh/m2/day (lowest month). | Sources are not stored in this repo because they name the location (owner's request). |
| S21 | Energizer L91 lithium AA, 24-pack: $54.87 ($2.29 each), Walmart, seen 2026-11-24. Rated 3000 mAh, 25-year shelf life (Battery Junction listing). | https://www.walmart.com/ip/seort/710283269 ; https://batteryjunction.com/batteries/shop-by-application-batteries/security-alarm/brand/simplisafe/energizer-ind-aa-l91-24pk |
| S23 | ESP32-C3 SuperMini power input: 5 V on USB, or 3.3–6 V on the VCC pin through its LDO. | https://mischianti.org/esp32-c3-supermini-plus-v2-high-resolution-pinout-datasheet-and-specs/ |
| — | LTO (lithium titanate) cells can be charged and discharged at -30 °C (Toshiba SCiB). A possible cold-charging alternative, not yet priced. | https://dashcamtalk.com/forum/attachments/toshiba-lto-battery-spec-sheet-pdf.88824/ |

## Power gating and storage

| # | Finding | Source |
|---|---|---|
| S24 | TI TPL5110 nano-power timer: 35 nA typ at 2.5 V, 1.8–5.5 V, intervals 100 ms to 7200 s, drives a MOSFET for power gating. | https://www.ti.com/product/TPL5110 ; https://ti.com/lit/pdf/snas650 |
| S25 | Micro Crystal RV-3028-C7 RTC: 45 nA at 3 V, runs on 1.1–5.5 V, ±1 ppm at 25 °C, alarm and interrupt output, -40 to +85 °C. $3.74 at DigiKey. | https://www.microcrystal.com/en/products/real-time-clock-rtc-modules/rv-3028-c7/ ; https://www.digikey.com.au/en/product-highlight/m/micro-crystal/rv-3028-c7-real-time-clock-rtc-module |
| S28 | Supercapacitor leakage is typically about 1 µA/F (datasheets give the maximum). Large 100 F, 2.7 V cells list 140–260 µA leakage. Rated -40 to 65 °C at 2.7 V. Maxwell projects 10-year DC life at 2.7 V, 25 °C. | https://www.cap-xx.com/wp-content/uploads/datasheets/CAP-XX-GY--L-Series-Datasheet.pdf ; https://www.eaton.com/kr/ko-kr/skuPage.HV1860-2R7107-R.html ; https://www.mouser.com/datasheet/2/257/3003111-EN.1_3V_100F_Datasheet-1879326.pdf ; https://maxwell.com/wp-content/uploads/2024/05/3003123-EN2_ds_Std.-2.7V-100F_20200908.pdf |
| S23b | December sunset at the nearest weather station is about 16:40. | Source not stored (it names the location). |

## Boards and antennas

| # | Finding | Source |
|---|---|---|
| S26 | Seeed XIAO ESP32C3: ESP32-C3 with an external u.FL antenna included; £3.93 ex VAT at Farnell UK. Optional 2.4 GHz rod antenna rated 2.81 dBi (£2.20). | https://export.farnell.com/seeed-studio/113991054/development-board-risc-arduino/dp/4060379 ; https://thepihut.com/collections/maker-store/products/2-4ghz-rod-antenna-for-xiao-esp32c3 |
| S26b | XIAO ESP32C3 working temperature -40 to 85 °C (seller listing; check the Seeed wiki). | https://thinkrobotics.com/products/seeed-studio-xiao-esp32c3 |
| S12b | ESP32-C3 module operating voltage 3.0–3.6 V. | https://espressif.com/sites/default/files/russianDocumentation/esp32-c3-mini-1_datasheet_en.pdf |

## Ecowitt WH51

| # | Finding | Source |
|---|---|---|
| S27 | WH51: capacitive FDR soil-moisture sensor, 1x AA, updates every 70 s, user-set 0%/100% AD calibration. The WH51/WH51L are rated for 100 m in open field. The WH51L lists -10 to 50 °C working temperature. | https://shop.ecowitt.com/collections/gardening/products/wh51 ; https://shop.ecowitt.com/products/wh51l ; https://shop.ecowitt.com/collections/soil-sensor/products/wh51l ; https://smarthomescene.com/reviews/ecowitt-wh51-vs-wh52-soil-sensor-comparison/ |

## Radio path

| # | Finding | Source |
|---|---|---|
| S22 | First Fresnel zone radius r = sqrt(λ·d1·d2/(d1+d2)); keep at least 60% of it clear of obstacles (80% ideal). | https://en.wikipedia.org/wiki/Fresnel_zone ; https://www.omnicalculator.com/physics/fresnel-zone |

## Cable

| # | Finding | Source |
|---|---|---|
| S17 | Gel-filled direct-burial Cat5e, 1000 ft: $241.99 (about $0.79/m). Shielded 22/3 direct-burial cable (Alpha Wire 35193): $3.49/ft. | https://www.zoro.com/bestlink-netware-cat5e-utp-direct-burial-outdoor-cable-gel-type-1000ft-black-100915bk/i/G0319007/ ; https://nassaunationalcable.com/products/alpha-wire-35193-22-awg-3-conductor-foil-300v-xtra-guard-3-direct-burial-cable |
| S18 | Direct-burial Cat5e jacket diameter 0.215 in (Hitachi 30145-8-BK3). | https://www.rexelusa.com/p/724623/hitachi-cable-america/4-pair-cat5e-burial/30145-8-bk3 |
