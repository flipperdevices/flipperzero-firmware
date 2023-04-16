# Seader

[Flipper Zero](https://flipperzero.one/) application (aka "fap"). Versioning TBD.

### Hardware

Put [HID SAM](https://www.cdw.com/product/hp-sim-for-hid-iclass-for-hip2-reader-security-sim/4854794) into [adapter](https://a.co/d/1E9Zk1h) (because of chip on top) and plug into [reader](https://www.mikroe.com/smart-card-2-click).

## Connections

| Smart Card 2 Click | Flipper     |
| ------------------ | ----------- |
| 5v                 | 1           |
| GND                | 8 / 11 / 18 |
| TX                 | 16          |
| RX                 | 15          |

## Development

 * Install git version of [asnc1](https://github.com/vlm/asn1c) (`brew install asn1c --head` on macos)
 * Run `make` in in root to generate asn1c files
 * Checkout or symalink to flipper-firmware's applications_user folder
 * `./fbt fap_seader`
