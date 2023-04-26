# flipper-zero-experimental-apps

Experimenting with app development for flipper zero.

## Apps

NB. These apps are in development - and will be marked complete when ready.

| Directory | App | Description | State |
|-|-|-|-|
| resistors | Resistance calculator | Coloured band resistance calculator. | Testing |

### Resistance calculator

A simple app to calculate resistance from the coloured bands. See:

* [README](resistors/README.md)
* [video](https://github.com/instantiator/flipper-zero-experimental-apps/blob/main/resistors/screenshots/2023-04-26%20resistance%20calculator%20flipper%20app.mov?raw=true)

![Screenshot of the resistance calculator in action](resistors/screenshots/2023-04-26%20resistor%20view.png)

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

## TODO

- [x] Complete first experiment (resistance calculator)
- [x] List on [flipc.org](https://flipc.org/)
- [ ] Add official [ufbt GitHub Action](https://github.com/marketplace/actions/build-flipper-application-package-fap)

## Credits

* [Derek Jamison](https://github.com/jamisonderek) for the [Flipper Zero tutorials](https://github.com/jamisonderek/flipper-zero-tutorials) repository (in particular, [plugins](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/plugins))
* [DroomOne](https://github.com/DroomOne) for the [Flipper Plugin tutorial](https://github.com/DroomOne/Flipper-Plugin-Tutorial).
* [Aleksey Korolev](https://github.com/playmean) for the [flipc.org](https://flipc.org/) Flipper app catalogue.

A big thank you to Derek Jamison for kindly helping with debugging several issues, too. You should really check out his YouTube channel: [@MrDerekJamison](https://www.youtube.com/@MrDerekJamison)

### See also

* [Flipper plugin documentation](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/documentation)
