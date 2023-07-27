---
layout: home
---
<script src="https://cdn.jsdelivr.net/npm/anchor-js/anchor.min.js"></script>

# Seader

{% responsive_image path: ./fake_screenshot.png %}

[Flipper Zero](https://flipperzero.one/) application (aka "fap").

## FAQ

See "Probably Frequently Asked Questions:" on [Red Team Tools product page](https://www.redteamtools.com/nard-sam-expansion-board-for-flipper-zero-with-hid-seos-iclass-sam/)

## Download

### Release

Download release versions via the Flipper App Catalog (when released)

### Beta/Dev

Download builds based off git at [flipc](https://flipc.org/bettse/seader?branch=main)

## Bugs

File issues in [GitHub](https://github.com/bettse/seader/issues).

## Hardware

### Option 1: NARD flipper add-on

Buy it assembled at [Red Team Tools](https://www.redteamtools.com/nard-sam-expansion-board-for-flipper-zero-with-hid-seos-iclass-sam/), with or without SAM.


Or build it yourself from the files in the [NARD repo](https://github.com/killergeek/nard).

### Option 2: Smart Card 2 Click

Put **[SAM](https://www.cdw.com/product/hp-sim-for-hid-iclass-for-hip2-reader-security-sim/4854794)** into **[adapter](https://a.co/d/1E9Zk1h)** (because of chip on top) and plug into **[reader](https://www.mikroe.com/smart-card-2-click)**. Connect reader to Flipper Zero (See `Connections` below).

#### Connections

| Smart Card 2 Click | Flipper     |
| ------------------ | ----------- |
| 5v                 | 1           |
| GND                | 8 / 11 / 18 |
| TX                 | 16          |
| RX                 | 15          |

## Development

See repo readme.

<script>
   anchors.add();
</script>
