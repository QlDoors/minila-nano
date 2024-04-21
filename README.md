# Minila-nano zmk firmware

This repository contains custom firmware for the modified Filco Minila Air keyboard, which replaces the original controller with the n52840nano_v2—an affordable alternative to the nice!nano v2.

## Minila Air Matrix Circuit Pin Map

| NA   | C12  | C11  | C10  | C9   | C8   | C7   | C6   | C5   | C4   | C3   | C2   | C1   | C0   |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 13   | 12   | 11   | 10   | 9    | 8    | 7    | 6    | 5    | 4    | 3    | 2    | 1    | P3/0 |
| 14   | 15   | 16   | 17   | 18   | 19   | 20   | 21   | 22   | 23   | 24   | 25   | 26   | 27   |
| C13  | D1-  | D2-  | D+   | C15  | C14  | R7   | R6   | R5   | R4   | R3   | R2   | R1   | R0   |

To reduce pin usage, the following pin mergers were implemented:

- **C0, C14, C15** merged to **C0**
- **C3, C11** merged to **C3**
- **C7, C13** merged to **C7**
- **C10, C12** merged to **C10**

Additionally, to conserve power, LED pins have been swapped.

### Revised Pin Map

The final configuration of the pin map is as follows:

| NA   | NA   | NA   | C10  | C9   | C8   | C7   | C6   | C5   | C4   | C3   | C2   | C1   | C0   |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 13   | 12   | 11   | 10   | 9    | 8    | 7    | 6    | 5    | 4    | 3    | 2    | 1    | P3/0 |
| 14   | 15   | 16   | 17   | 18   | 19   | 20   | 21   | 22   | 23   | 24   | 25   | 26   | 27   |
| NA   | D1+  | D2+  | D-   | NA   | NA   | R7   | R6   | R5   | R4   | R3   | R2   | R1   | R0   |

## Building the Firmware

To build the firmware, follow these steps:

```bash
git clone https://github.com/QlDoors/minila-nano.git
git clone https://github.com/zmkfirmware/zmk.git
cd zmk
python3 -m virtualenv .venv --python=python3.7.5
source .venv/bin/activate
pip3 install -U west
west build -p -b nice_nano_v2 -- -DSHIELD=minila_nano -DZMK_EXTRA_MODULES="/path/to/minila-nano" -DZMK_CONFIG="/path/to/minila-nano/config"
```

Replace `/path/to/minila-nano` with the actual path to the cloned `minila-nano` directory on your system.