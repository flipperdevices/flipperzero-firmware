# FlipFrid

A simple implmentation of ZigFrid on Flipper zero

(https://z4ziggy.wordpress.com/2017/07/21/zigfrid-a-passive-rfid-fuzzer/)

## Installation

- Create the `flipperzero-firmware/applications/flipfrid` folder in flipper zero firmware sources.
- Git clone this repo inside flipfrid
- Update `/flipperzero-firmware/applications/meta/application.fam` to add an entry in `App/provides`

```
App(
    appid="basic_apps",
    name="Basic applications for main menu",
    apptype=FlipperAppType.METAPACKAGE,
    provides=[
        "gpio",
        "ibutton",
        "infrared",
        "lfrfid",
        "nfc",
        "subghz",
        "bad_usb",
        "u2f",
        "flip_frid_app",
    ],
)
```

- Compile and push new firmware `/fbt --with-updater flash_usb`
- Enjoy !
