# flipper-zero-experimental-apps

Experimenting with app development for flipper zero.

## Apps

| Screenshot | Directory | App | Description |
|---|-|-|---|
| ![Screenshot of the resistance calculator in action](resistors/screenshots/v1.1/4-bar.png) | [resistors](resistors/) | Resistance calculator | Coloured band resistance calculator. |

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

## Thanks

* [Derek Jamison](https://github.com/jamisonderek) Derek Jamison for kindly helping to debug several issues.
* [Hedger](https://github.com/hedger) for kindly adding the ufbt GitHub Action.
* [Kuronons](https://github.com/Kuronons) for the awesome new graphics.

You should really check out Derek's YouTube channel: [@MrDerekJamison](https://www.youtube.com/@MrDerekJamison)

## Credits

* [Derek Jamison](https://github.com/jamisonderek) for the [Flipper Zero tutorials](https://github.com/jamisonderek/flipper-zero-tutorials) repository (in particular, [plugins](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/plugins))
* [DroomOne](https://github.com/DroomOne) for the [Flipper Plugin tutorial](https://github.com/DroomOne/Flipper-Plugin-Tutorial).
* [Aleksey Korolev](https://github.com/playmean) for the [flipc.org](https://flipc.org/) Flipper app catalogue.

### See also

* [Flipper plugin documentation](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/documentation)
