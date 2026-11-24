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

## Enclosure

| # | Finding | Source |
|---|---|---|
| S29 | Hammond 1554 polycarbonate enclosures: UV-stabilized for outdoor use, NEMA 4/4X/6/6P, up to IP68. 1554QA2GY (140 x 140 x 90 mm): £40.42 ex VAT, RS UK. | https://uk.rs-online.com/web/p/general-purpose-enclosures/2291079?gb=s ; https://uk.rs-online.com/web/p/general-purpose-enclosures/2291093 |
| S30 | Multi-hole gland inserts: LAPP SKINTOP DIX, M25, 4 x 5 mm holes, NBR, -40 to +100 °C, about €1.35 each. IP68 only if every hole holds a cable of nominal size or a sealing plug. Jacob WADI multi-hole M25 fits 5–7 mm cables, IP68/69, -40 to 100 °C. | https://buerklin.com/en/p/lapp/cable-management-accessories/53325450/32P2660 ; https://au.element14.com/lapp/53325260/insert-gland-m25-2x6/dp/120420501 ; https://www.digikey.ca/en/products/detail/jacob-gmbh/K152-1025-00-37/24760855 |
| S31 | Membrane vents (GORE Protective Vents) equalize enclosure pressure, keep out dust and liquids, and reduce condensation by letting water vapor out. | https://www.gore.com/node/2566 |

## Cable, splices, riser, vehicle

| # | Finding | Source |
|---|---|---|
| S32 | Southwire 547030508 18/3 irrigation and sprinkler cable: 3x 18 AWG solid copper, 0.184 in OD, direct burial, sunlight resistant, UL listed, 150 V, 75 °C; $0.25/ft in 500 ft multiples (KSCdirect). Zoro lists the same part, 500 ft for $137.99. | https://kscdirect.com/itemDetail/6061229/18-3-irrigation-bk-500-r ; https://www.zoro.com/woods-500-ft-183-solid-copper-sprinkler-wire-547030508/i/G815832909/ |
| S33 | 3M DBR/Y-6 direct bury splice kit: twist-on connector in a grease-filled tube, UL listed for direct bury, 600 V, 18–10 AWG (minimum 18 AWG), operating -40 to 105 °C; used for irrigation controls and landscape lighting. 3M UY2 gel-filled telecom connectors take 26–19 AWG and are Cat 5 rated. | https://3m.com/3M/en_US/p/d/b00026285 ; https://www.3mcanada.ca/3M/en_CA/p/d/v000099614/ ; https://www.mouser.com/datasheet/2/1/Scotchlok_Cnctrs_Link_Video-60691.pdf |
| S34 | 2026 Toyota Tundra: overall width without mirrors 80.2 in. | https://www.edmunds.com/toyota/tundra/2026/st-402077541/features-specs |
| S35 | 1-1/2 in Schedule 40 PVC conduit: inside diameter 1.56 in (Cantex, Zoro listing). Conduit fill: 40% of internal area for 3 or more conductors (NEC Chapter 9 Table 1, as tabulated). | https://zoro.com/cantex-schedule-40-conduit-1-12-in-10-ft-l-a52be12/i/G2766391 ; https://www.pdhonline.com/courses/e276/Conduit_Fill_Tables.pdf |

| S36 | Southwire underground irrigation cable (18/xx, low-energy circuit, 30 VAC): rated -20 to 75 °C. 18/4: 0.2 in OD. 18/7: 0.235–0.268 in OD. McMaster sprinkler wire: 18/2 0.2 in, 18/3 0.21 in, 18/4 0.23 in. | https://cabletechsupport.southwire.com/en/tile/49/cable/60324/?country=CA ; https://kscdirect.com/item/CAB%2B547040508 ; https://www.zoro.com/southwire-cci-barogation-547070408-sprinkler-cable-18-awg-547070408/i/G604258330/ ; https://www.mcmaster.com/products/sprinkler-wire/ |
| S37 | 3M DBR/Y-6 prices: $2.88 each in a 100-pack (Zoro); $3.25 each (TestEquity); $10.49 per retail 2-pack (Walmart). | https://zoro.com/3m-d-bury-splice-kit-pk100-dbry-6-bulk/i/G2042744 ; https://testequity.com/product/10020447-DBR-Y-6-BULK ; https://www.walmart.com/ip/38474094 |
| S38 | 1/2 in Schedule 40 PVC (Atkore spec): OD 0.840 in, minimum average ID 0.578 in. | https://www.elliottelectric.com/Media/PVC6-PVC |
| S39 | ESPHome ESP-NOW component (ESP32; ESP8266 in the next release) and the ESP-NOW Packet Transport platform, which shares sensor states between nodes without a server. | https://esphome.io/components/espnow/ ; https://esphome.io/components/packet_transport/espnow/ ; https://esphome.io/components/packet_transport/ |

| S40 | Landscape-lighting piercing connectors: pins pierce the insulation, no stripping, cutting or splicing; a branch can be made at any point on the main line. Retail: 22-pack for $14.98. Reviews of one retail connector report pin corrosion and breakage after about one year. | https://www.walmart.com/ip/14584506198 ; https://www.walmart.com/ip/17345558348 ; https://www.walmart.com/ip/seort/335003381 ; https://www.lowes.com/reviews/Portfolio-Landscape-Lighting-Cable-Connector/3612254?offset=140 |
| S41 | 16/2 low-voltage landscape lighting wire: 500 ft for $140 ($0.28/ft, Cerrowire 241-1202J, Home Depot); direct-burial approved (Southwire). Carol 02309: flat 0.155 x 0.296 in, 16 AWG stranded, 60 °C. | https://www.homedepot.com/p/202206445 ; https://homedepot.ca/product/southwire-16-2-landscape-lighting-wire-black-cut-by-the-metre-/1000823355 ; https://www.standardelectricsupply.com/Carol-Cable-16-2LOWVOLTAGE-BLK-500SPL-Landscape-Lighting-Cable |
| S42 | 1-Wire long-line guidance (Maxim/ADI tutorial 148): network radius = wire distance to the farthest slave; weight = total wire; unswitched star not recommended. A vendor guide quotes 20 m radius and 20 m weight on Cat5 for a simple master. ADI's advanced master: networks up to 500 m equivalent size. | https://www.analog.com/en/resources/technical-articles/guidelines-for-reliable-long-line-1wire-networks.html ; https://hallaweb.jlab.org/tech/Detectors/public_html/manuals/chip_specs/M-Z/maxim/lvds_maxim/AN148.pdf ; https://www.scribd.com/document/93906614/1-Wire-Basics ; https://www.analog.com:443/en/technical-articles/advanced-1-wire-network-driver.html |

| S43 | 18/2 irrigation wire: Southwire 1,000 ft $186.27 (Home Depot); 250 ft $46.77 (Sprinkler Warehouse); 18/2 nominal OD 0.190 in. | https://homedepot.com/p/Southwire-1-000-ft-18-2-Black-Solid-UL-Burial-Sprinkler-System-Wire-49278503/300916643 ; https://www.sprinklerwarehouse.com/18-awg-2-conductor-underground-sprinkler-wire-250-ft-18-2-250 ; https://www.sprinklerwarehouse.com/multi-conductor-wire-black-18-gauge-10-conductor-500-ft-sold-per-ft |
| S44 | Puya PY32F002A: Cortex-M0+, 1.7–5.5 V, -40 to 85 °C. LCSC PY32F002AA15M6TU (ESSOP-10): $0.0927 at 500+, $0.0864 at 2,500+. | https://lcsc.com/product-detail/Microcontrollers-MCU-MPU-SOC_PUYA-PY32F002AA15M6TU_C5292058.html ; https://www.lcsc.com/product-detail/Microcontroller-Units-MCUs-MPUs-SOCs_PUYA-PY32F002AL15S6TU_C5292060.html |
| S45 | WCH CH32V003J4M6 (SOP-8): $0.1238 at 500+ (LCSC). CH32V003 has an 8-channel 10-bit ADC, an op-amp and a comparator. | https://www.lcsc.com/product-detail/Microcontrollers-MCU-MPU-SOC_WCH-Jiangsu-Qin-Heng-CH32V003J4M6_C5346354.html ; https://static.chipdip.ru/lib/164/DOC045164018.pdf |
| S46 | JLCPCB assembly: standard PCBA setup $25 single side; SMT $0.0016 per joint; extended-part feeder fee $1.50 per part type (updated 2025-12-19); prototype PCBs from $2 for 5. | https://jlcpcb.com/help/article/pcb-assembly-price ; https://jlcpcb.com/pcb-assembly ; https://techoverflow.net/2024/04/09/jlcpcb-pcba-assembly-price-overview/ ; https://lcsc.com/pcba |

| S47 | Temperature effect on capacitance readings: water permittivity about 88 at 0 °C and about 80 at 20 °C (Campbell Scientific). Bound and free water act in opposite directions, so the net effect is soil-specific. Hydra Probe study, 19 soils: a 40 °C change gave up to ±0.028 m³/m³ apparent water content. | https://www.campbellsci.ca/faqs?v=1046 ; https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3444127/ ; https://agris.fao.org/search/ru/records/65df4349b766d82b1800f40d |
| S48 | PY32F002A has an internal temperature sensor on an ADC channel (py32-hal driver). Internal MCU temperature sensors have chip-to-chip offset; uncalibrated, they suit detecting temperature changes (ST reference manual wording, same sensor type). | https://docs.rs/py32-hal/0.1.0/src/py32_hal/adc/v1.rs.html ; https://wenku.csdn.net/doc/6481717fd12cbe7ec369da90 |
| S49 | Molding: Protolabs aluminum molds from $1,495, 10 to 10,000+ parts, no MOQ. RapidDirect low-volume molding 100–10,000 parts (aluminum 100–5,000; soft steel 5,000–25,000). | https://protolabs.com/injection-molding ; https://www.protolabs.com/en-gb/services/injection-moulding/on-demand-manufacturing/ ; https://www.rapiddirect.com/services/low-volume-injection-molding-service/ |
| S50 | 3D-print materials outdoors: ASA has better UV resistance; PETG yellows and turns brittle in sun (vendor test write-ups). | https://qidi3d.com/blogs/print-lab/asa-vs-petg-filament-direct-sunlight ; https://qidi3d.com/ko/blogs/print-lab/asa-petg-sprinkler-brackets-outdoor |

## Hardware (round 14)

| # | Finding | Source |
|---|---|---|
| S51 | OneWireHub library emulates 1-Wire slaves including the DS18B20 (family 0x28), up to 32 per hub; a "DS18B20 as interface" example returns arbitrary values as emulated thermometers. | https://github.com/vytautassurvila/OneWireHub/blob/master/README.md ; https://instructables.com/Arduino-1-wire-Generic-ClientSlave-Device-Sensor |
| S52 | ESPHome 1-Wire: `one_wire` bus (GPIO, or DS248x master chips with an active pull-up option) + `dallas_temp` sensors by 64-bit address. Users report reliability problems with the GPIO bit-bang implementation. | https://esphome.io/components/one_wire/index.html ; https://esphome.io/components/one_wire/ds248x/ ; https://esp32.co.uk/esphome-ds18b20-multiple-sensors-long-wires-reliable-wiring/ ; https://community.home-assistant.io/t/mipi-display-glitching-with-gpio-one-wire/967055 |
| S53 | TI TPS7A02 LDO: 25 nA typical quiescent current (also in dropout), 3 nA shutdown, 1.5–6.0 V input, 200 mA, -40 to 125 °C. | https://www.ti.com/product/TPS7A02 |
| S54 | TI TPS22917 load switch: 1–5.5 V input (6 V absolute max), 2 A, 80 mΩ; shutdown current 10 nA typical, 100 nA max at -40 to 85 °C; TPS22917L = active-low enable. | https://www.ti.com/product/TPS22917 ; https://ti.com/document-viewer/TPS22917/datasheet/GUID-XXXXXXXX-SF0T-XXXX-XXXX-000000062506 ; https://www.ti.com/lit/gpn/tps22917 |
| S55 | XIAO ESP32C3: the 5V pin can be used as an input, but Seeed requires a diode between the external source and the pin (anode to source). | https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/ |
| S56 | OpenSprinkler: the main controller has 8 zones; each zone expander adds 16 (8 + 16 = 24 with one expander); OS 3.x supports up to 72 zones. Expander $49–50. | https://opensprinkler.com/?p=16087 ; https://opensprinklershop.github.io/faq/ ; https://opensprinkler.com/product/zone-expander/ |
| S57 | PY32F002A has 2 comparators (datasheet feature list). | https://hestore.hu/prod_getfile.php?id=16039 |

| S58 | ADI DS2482-100: I2C-to-1-Wire master, selectable active or passive pull-up, strong pull-up via an internal low-impedance path, PCTLZ output to control an external MOSFET for a stronger pull-up; 2.9–5.5 V, -40 to 85 °C; from $1.18 (1k list). DS2484: strong pull-up, 1-Wire side 1.8–5.0 V, US$1.956 at 10+ (Farnell). | https://analog.com/en/DS2482-100 ; https://www-cce.cldnet.analog.com/en/products/ds2482-100 ; https://wcm-cce.cldnet.analog.com/media/en/technical-documentation/data-sheets/ds2484.pdf ; https://il.farnell.com/analog-devices/ds2484r-t/interface-bridge-i2c-to-1wire/dp/2514570 |
| S59 | 10 F 2.7 V supercapacitors: Eaton HV1030-2R7106-R, $3.39 each / $1.97 at 50 (DigiKey, USD), 1000 h at 65 °C; Tecate TPL-10-10X30F, -40 to 85 °C. | https://www.digikey.com/en/products/detail/eaton-electronics-division/HV1030-2R7106-R/3878071 ; https://www.digikey.ca/en/products/detail/tecate-group/TPL-10-10X30F/9930253 |
| S60 | TI TPS61099 boost: 0.7–5.5 V input, 1.8–5.5 V output, 1 µA quiescent current, true disconnect in shutdown, up to 300 mA from 3.3 V to 5 V. TI TPS61094: 60 nA quiescent boost with supercapacitor management. | https://www.ti.com/lit/gpn/TPS61099 ; https://www.ti.com/product/TPS61099/description |

| S61 | US burial practice for irrigation wire: up to 30 V, minimum 6 in burial (Paige installation spec, citing NEC Table 300.5); municipal irrigation codes require 6 in for low-voltage wiring to zone valves. NEC Table 300.5 note: lesser depths are permitted where cables rise for terminations or splices. | https://www.paigewire.com/docs/paigewire/library/doc/AC.docx ; https://colleyville.com/home/showpublisheddocument/126/636039493188130000 ; https://broomfield.org/DocumentCenter/View/2545 |
| S62 | ESP-NOW with Wi-Fi on one chip: both must use the same channel (Espressif forum). ESPHome `espnow`: the channel can't be set when Wi-Fi is used; ESP-NOW uses the Wi-Fi network's channel. XIAO ESP32C3 Wi-Fi active < 75 mA. | https://esp32.com/viewtopic.php?p=82304 ; https://esphome.io/components/espnow.html ; https://shop.pimoroni.com/en-eu/products/seeed-studio-xiao-esp32c3 |

| S63 | ESP32 measurements (MicroPython, optimised boot): boot + ESP-NOW message 119 ms / 7.7 mC; boot + MQTT over Wi-Fi with static IP 774 ms / 108.6 mC; with DHCP 1,626 ms. | https://github.com/glenn20/upy-esp32-experiments |
| S64 | ESPHome deep sleep: current community guidance recommends the native API (MQTT no longer required); after Wi-Fi connects, the API component sets up and HA connects. | https://community.home-assistant.io/t/native-api-vs-mqtt-when-using-deep-sleep-in-esphome/1014719/7 ; https://community.home-assistant.io/t/notes-on-esphome-deep-sleep/860987/9 |
| S65 | TI BQ25570: energy-harvesting boost charger; cold start from ≥ 600 mV, continuous harvesting from 100 mV, programmable MPPT, 488 nA quiescent; supports supercapacitors; reference design TIDA-00242 (solar + supercap). | https://www.ti.com/product/BQ25570 ; https://ti.com/document-viewer/BQ25570/datasheet |
| S66 | ESPHome time platforms exist for DS1307 and RX8025T (none found for the RV-3028). An ESP-IDF RV-3028-C7 driver exists; ESPHome documents writing a time-platform component. | https://esphome.io/components/time/ds1307.html ; https://esphome.io/components/time/rx8025t/ ; https://components.espressif.com/components/javsalv/rv-3028-c7/versions/1.0.0/readme ; https://developers.esphome.io/architecture/components/time/ |

| S67 | Starlink Gen 3 router: 2 LAN Ethernet ports for your own devices; bypass mode turns off its routing and Wi-Fi so a third-party router can take over. The app can split the 2.4/5 GHz networks. No manual channel setting found in Starlink docs. | https://starlink.com/support/article/63357ec4-8fbe-7f65-530f-76fd55192104 ; https://starlink.com/bb/support/article/a0fe8d51-32f7-d2b9-d74a-801e31ad9f6a ; https://support.simplisafe.com/articles/base-station/connect-your-base-station-to-starlink-wifi?lang=en-US |
| S68 | ESPHome espnow: `channel` can't be set when the Wi-Fi component is used (it follows the Wi-Fi network's channel). Community: an ESP32 with Ethernet can run ESP-NOW on any channel. | https://new.esphome.io/components/espnow ; https://community.home-assistant.io/t/esp-now-with-esp-home/536006/14 |
| S69 | ESPHome Ethernet component: ESP32 with LAN8720 and others; examples for Olimex ESP32-POE and WT32-ETH01; can't be used together with the Wi-Fi component. | https://esphome.io/components/ethernet.html ; https://new.esphome.io/components/ethernet |
| S70 | Fixing a 2.4 GHz channel on a typical router: choose 1, 6 or 11 at 20 MHz width. Some routers have no manual channel setting. | https://community.netgear.com/discussions/en-home-orbi-ax/can-i-change-the-wifi-channels-on-the-orbi-meshrbk862sb/2467824 ; https://support.ring.com/hc/en-au/articles/4407014643732 |

| S71 | Drip-irrigated trees: install the soil moisture sensor at the canopy drip line, 12–18 in from an emitter (Monnit installation guide). | https://support.monnit.com/article/78-soil-moisture-sensor-installation-by-crop-type |
| S72 | Vineyard extension: choose a distance from emitter and vine that captures uptake; too far from the emitter misses the wetting pattern and can lead to over-watering. | https://lodigrowers.com/using-soil-moisture-sensors-for-vineyard-irrigation-management-a-practical-guide-for-installing-and-interpreting-information-from-soil-moisture-monitoring-technologies-in-vineyards-part-ii/ |
| S73 | Starlink Mini: integrated Wi-Fi 5 router and one Ethernet port. Starlink app can split 2.4/5 GHz networks. | https://www.bestbuy.com/product/starlink-mini-kit-ac-dual-band-wi-fi-system-white/J3R8ZC259C ; https://starlink.com/support/article/acb3bee4-53d9-2f4f-ca45-c21db163543c |
| S74 | ESP-NOW channel hopping: the node iterates through channels sending to the hub until acknowledged, because access points change channels. Fixing the router channel is simplest only if you control the router. | https://thingpulse.com/esp32-espnow-wifi-simultaneous-communication/ ; https://dev.to/gtanyware/channel-hopping-on-the-esp32-c3-with-esp-now-2led |

| S75 | ESPHome espnow: ESP32 and ESP8266 supported; on ESP8266 the received RSSI is always 0 and broadcasts arrive in on_receive; send has on_sent/on_error; channel can't be set while Wi-Fi is used. | https://esphome.io/components/espnow/ |
| S76 | ESP32-CAM (AI-Thinker): PCB antenna plus u.FL/IPEX socket, chosen by a 0 Ω jumper. | https://randomnerdtutorials.com/esp32-cam-connect-external-antenna/ ; https://lastminuteengineers.com/getting-started-with-esp32-cam/ |
| S77 | ESPHome on Windows: Python with "Add to PATH", then `pip install esphome`; CLI `esphome config/compile/run/logs`, `--device COMx`. | https://new.esphome.io/guides/installing_esphome ; https://esphome.io/guides/cli/ |

## Radio path

| # | Finding | Source |
|---|---|---|
| S22 | First Fresnel zone radius r = sqrt(λ·d1·d2/(d1+d2)); keep at least 60% of it clear of obstacles (80% ideal). | https://en.wikipedia.org/wiki/Fresnel_zone ; https://www.omnicalculator.com/physics/fresnel-zone |

## Cable

| # | Finding | Source |
|---|---|---|
| S17 | Gel-filled direct-burial Cat5e, 1000 ft: $241.99 (about $0.79/m). Shielded 22/3 direct-burial cable (Alpha Wire 35193): $3.49/ft. | https://www.zoro.com/bestlink-netware-cat5e-utp-direct-burial-outdoor-cable-gel-type-1000ft-black-100915bk/i/G0319007/ ; https://nassaunationalcable.com/products/alpha-wire-35193-22-awg-3-conductor-foil-300v-xtra-guard-3-direct-burial-cable |
| S18 | Direct-burial Cat5e jacket diameter 0.215 in (Hitachi 30145-8-BK3). | https://www.rexelusa.com/p/724623/hitachi-cable-america/4-pair-cat5e-burial/30145-8-bk3 |
