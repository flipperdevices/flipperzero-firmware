# flipper-zero-experimental-apps

Experimenting with app development for flipper zero.

## Apps

| Screenshot | Directory | App | Description |
|---|-|-|---|
| ![Screenshot of the resistance calculator in action](resistors/screenshots/2023-04-26%20resistor%20view.png) | [resistors](resistors/) | Resistance calculator | Coloured band resistance calculator. |

## Build status

| App | Status |
|-|-|
| Resistance calculator |  ![Build status](https://github.com/instantiator/flipper-zero-experimental-apps/actions/workflows/build-resistors.yml/badge.svg?branch=main) |

## Build tools

These apps are built using [ufbt](https://pypi.org/project/ufbt/) - a subset of the flipper build tool (fbt) targeted at building apps. Install it with:

```bash
pip install ufbt
```

Each directory contains the following scripts:

* `build.sh` - invokes `ufbt` to build the tool
* `deploy.sh` - invokes `ufbt` to deploy and launch the app on a flipper over USB

Build outputs are found in the `dist` directory for each application.

## Distribution

Apps are also distributed through:

* [flipc.org](https://flipc.org/) (unofficial Flipper app catalogue)

## Credits

* [Derek Jamison](https://github.com/jamisonderek) for the [Flipper Zero tutorials](https://github.com/jamisonderek/flipper-zero-tutorials) repository (in particular, [plugins](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/plugins))
* [DroomOne](https://github.com/DroomOne) for the [Flipper Plugin tutorial](https://github.com/DroomOne/Flipper-Plugin-Tutorial).
* [Aleksey Korolev](https://github.com/playmean) for the [flipc.org](https://flipc.org/) Flipper app catalogue.

A big thank you to Derek Jamison for kindly helping with debugging several issues, too. You should really check out his YouTube channel: [@MrDerekJamison](https://www.youtube.com/@MrDerekJamison)

### See also

* [Flipper plugin documentation](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/documentation)
