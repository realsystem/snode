# Probe firmware (PY32F002A)

Design: `docs/09-probe-design.md`. Toolchain: GNU Arm GCC + the open-source `IOsetting/py32f0-template`
(LL drivers, Makefile, flashing with a DAPLink probe through pyOCD).

| File | Status |
|---|---|
| `src/measure.c/.h` | Portable: 64-sample RC timing, trimmed mean, fault detection. Host-tested. |
| `src/crc8.c/.h` | Portable: Dallas/Maxim CRC-8. Host-tested (AN27 example). |
| `src/probe_id.c/.h` | Portable: 1-Wire ROM code and DS18B20-style scratchpad (bus option O1). Host-tested. |
| `src/hal.h` | Interface between portable code and hardware. |
| `src/hal_py32.c` | Stub: register code to write against the py32f0-template LL drivers. |
| Protocol layer | Not written: waits for the bus choice (design log D35): O1 1-Wire slave, or O2 powered bus (UART-style). |

Host tests (any PC with gcc): `cd tests && gcc -I../src ../src/measure.c ../src/crc8.c ../src/probe_id.c test_host.c && ./a.out`
