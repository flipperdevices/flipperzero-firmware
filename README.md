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

## Credits

* [Derek Jamison](https://github.com/jamisonderek) for the [Flipper Zero tutorials](https://github.com/jamisonderek/flipper-zero-tutorials) repository (in particular, [plugins](https://github.com/jamisonderek/flipper-zero-tutorials/tree/main/plugins))
