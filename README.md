# flipper-zero-experimental-apps

Experimenting with app development for flipper zero.

## Apps

NB. These apps are in development - and will be marked complete when ready.

| App | Description | State |
|-|-|-|
| resistors | Coloured band resistance calculator. | In development |

## Build tools

These apps are built using [ufbt](https://pypi.org/project/ufbt/) - a subset of the flipper build tool (fbt) targeted at building apps. Install it with:

```bash
pip install ufbt
```

Each directory contains the following scripts:

* `build.sh` - invokes `ufbt` to build the tool
* `deploy.sh` - invokes `ufbt` to deploy and launch the app on a flipper over USB

Build outputs are found in the `dist` directory for each application.

## TODO

- [ ] Complete first experiment (resistance calculator)
- [ ] Add official [ufbt GitHub Action](https://github.com/marketplace/actions/build-flipper-application-package-fap)

## Credits

* [Derek Jamison](https://github.com/jamisonderek) for the [Flipper Zero tutorials](https://github.com/jamisonderek/flipper-zero-tutorials) repository (in particular, [plugins](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/plugins))
* [DroomOne](https://github.com/DroomOne) for the [Flipper Plugin tutorial](https://github.com/DroomOne/Flipper-Plugin-Tutorial).

### See also

* [Flipper plugin documentation](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/documentation)
