# flipperzero-qrcode
Display qrcodes on the [Flipper Zero]

## Building
First, clone the [flipperzero-firmware] repo and then clone this repo in the
`applications_user` directory:

```bash
git clone git@github.com:flipperdevices/flipperzero-firmware.git
cd flipperzero-firmware/applications_user
git clone git@github.com:bmatcuk/flipperzero-qrcode.git
```

Next, in the base of the [flipperzero-firmware] directory, run fbt:

```bash
cd ..
./fbt fap_qrcode
```

This will automatically install dependencies and build the application. When it
has finished building, the .fap will be in
`build/f7-firmware-D/.extapps/qrcode.fap` (fbt output will tell you where to
find the .fap, should it change in the future).

## Installation
Copy the `qrcode.fap` file onto your [Flipper Zero] sd card in the `apps/Tools`
directory. Then create a top level directory called `qrcodes` to store your
qrcode files.

## Creating QR Codes
qrcode files are simple text files with the extension .qrcode. This app will
expect them to live in a top-level directory on your sd card called `qrcodes`.
They should have the following content:

```
Filetype: QRCode
Version: 0
Message: your content here
```

### Message Format
qrcodes support 4 formats called "modes": numeric, alpha-numeric, binary, and
kanji. Because of the limited screen real-estate on the [Flipper Zero], you'll
want to pick the best mode for the data you are trying to display.

The app will automatically detect the best mode to use, so the only thing you
need to do is make sure the message in your file is formatted to use the best
mode. For example, if your message is entirely numeric, make sure you don't
include any extraneous punctuation in your file. If you're only encoding a
domain name, make sure it's uppercase to take advantage of alpha-numeric mode,
etc.

#### Numeric Mode
Consists of only numbers, nothing else. This mode can encode the most data.

#### Alpha-Numeric Mode
This mode can encode numbers, uppercase letters *only*, spaces, and the
following symbols: `$%*+-./:`. This format _may_ be appropriate for urls, as
long as you're only encoding the domain name and you remember to use uppercase
letters (ex: `HTTP://EXAMPLE.COM`). If your url includes some path after the
domain, you'll likely need to use binary mode because the paths are usually
case-sensitive.

A qrcode in alpha-numeric mode can encode ~40% less data than numeric mode.

#### Binary Mode
This mode is a little bit of a misnomer: binary mode simply means that the
message will be encoded as 8-bit bytes. The qrcode standard stipulates that
text will use ISO-8859-1 (also known as Latin-1) encoding, _not_ utf8 as would
be the standard these days. However, _some_ readers _may_ automatically detect
utf8. To be standard-compliant, that basically means you can only use Latin
letters, numbers, and symbols.

A qrcode in binary mode can encode ~60% less data than numeric mode, and ~30%
less than alpha-numeric.

#### Kanji Mode
This mode is unsupported, so I won't go into detail.

## qrcode library
This application uses the [QRCode] library by ricmoo. This is the same library
that is in the lib directory of the flipper-firmware repo (which was originally
included for a [now-removed demo app]), but modified slightly to fix some
compiler errors.

[now-removed demo app]: https://github.com/flipperdevices/flipperzero-firmware/pull/160/files
[flipperzero-firmware]: https://github.com/flipperdevices/flipperzero-firmware
[Flipper Zero]: https://flipperzero.one/
[QRCode]: https://github.com/ricmoo/QRCode
