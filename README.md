# flip-onity-opener

Flipper zero exploiting vulnerability to open an Onity hotel room lock.

[Vulnerability described here](reference/brocious_bhpaper2018.md)

### Installation

- Download [last release fap file](https://github.com/xtruan/flip-onity-opener/releases/latest)
- Copy fap file to the apps folder of your Flipper SD card

### Usage

- Start "Onity Opener" plugin
- Place wires as described on the plugin screen
- Press enter
- Open hotel lock

### Build

- Recursively clone your base firmware (official or not)
- Clone this repository in `applications_user`
- Build with `./fbt fap_dist APPSRC=applications_user/flip-onity-opener`
- Retreive build fap in dist subfolders

(More info about build tool [here](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/fbt.md))
