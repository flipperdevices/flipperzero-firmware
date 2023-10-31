# flipperzero-qrcode
Display qrcodes on the [Flipper Zero]

![qrcode screenshot](https://github.com/bmatcuk/flipperzero-qrcode/blob/main/ss2.png?raw=true)

## Download
Grab the latest `qrcode.fap` from [Releases].

## Installation
Copy the `qrcode.fap` file onto your [Flipper Zero] sd card in the `apps/Tools`
directory. Then create a top level directory called `qrcodes` to store your
qrcode files. This can be done using [qFlipper], for example, by
draging-and-dropping `qrcode.fap` into `apps/Tools` and then navigating back to
the top level (where the directories like `infrared` and `nfc` live), right
click, and create a new folder called `qrcodes`.

## Creating QR Codes
qrcode files are simple text files with the extension `.qrcode`. This app will
expect them to live in a top-level directory on your sd card called `qrcodes`.
They should have the following content:

```
Filetype: QRCode
Version: 0
Message: your content here
```

qrcode v2 supports a newer format as well (the old format still works for
backward compatibility, or, if you don't need the newer features, the app will
read version "0" files faster):

```
Filetype: QRCode
Version: 1
QRMode: B
QRVersion: 6
QRECC: L
Message: your content here
Message: multi-line content is possible
```

In a version "1" file, the `QRMode`, `QRVersion`, and `QRECC` are optional
(though, must be in that order if more than one are specified). The app will
attempt to use the specified mode, version, and/or ECC, if the content will
fit. Otherwise, it may select a different mode, version, and/or ECC. Keep
reading to learn about the meaning of `QRMode`, `QRVersion`, and `QRECC`.

Version "1" files also support multi-line content. Each line starting with
`Message:` will be concatenated together with newline characters.

My recommendation is to allow the app to select a mode, version, and ECC level
for you and, then, if you find that your qrcode reader prefers specific
settings, update the file appropriately.

### Message Format
qrcodes support 4 formats called "modes": numeric, alpha-numeric, binary, and
kanji. Because of the limited screen real-estate on the [Flipper Zero], you'll
want to pick the best mode for the data you are trying to display.

If unspecified in the `.qrcode` file, the app will automatically detect the
best mode to use based on the message content.

#### Numeric Mode (QRMode: N)
Consists of only numbers, nothing else. This mode can encode the most data and
is useful for things like phone numbers. To use this mode, your message must
_not_ contain non-numeric characters. For example, a message content of "(xxx)
xxx-xxxx" can _not_ use numeric mode (it would require "binary" mode, in fact).
Instead, your message should just be "xxxxxxxxxx".

#### Alpha-Numeric Mode (QRMode: A)
This mode can encode numbers, uppercase letters *only*, spaces, and the
following symbols: `$%*+-./:`. This format _may_ be appropriate for urls, as
long as you're only encoding the domain name and you remember to use uppercase
letters (ex: `HTTP://EXAMPLE.COM`). If your url includes some path after the
domain, you'll likely need to use binary mode because the paths are usually
case-sensitive.

A qrcode in alpha-numeric mode can encode ~40% less data than numeric mode.

#### Binary Mode (QRMode: B)
This mode is a little bit of a misnomer: binary mode simply means that the
message will be encoded as 8-bit bytes. The qrcode standard stipulates that
text will use ISO-8859-1 (also known as Latin-1) encoding, _not_ utf8 as would
be the standard these days. However, _some_ readers _may_ automatically detect
utf8. To be standard-compliant, that basically means you can only use Latin
letters, numbers, and symbols.

Multi-line messages will always be in binary mode, since the other modes cannot
encode a newline character.

A qrcode in binary mode can encode ~60% less data than numeric mode, and ~30%
less than alpha-numeric.

#### Kanji Mode (QRMode: K)
This mode is unsupported, so I won't go into detail. A limitation of the
underlying qrcode library that I'm using, unfortunately. If there's interest,
perhaps I'll hack in support sometime.

### QRVersion
A qrcode's version specifies how "big" it is. Higher versions contain more
"modules" (ie, the "pixels" that make up qrcodes) and, thus, can encode more
data. A version 1 qrcode contains 21x21 modules, whereas a version 11 code (the
largest the Flipper Zero can display) contains 61x61 modules. The modules of a
version 1 code will be 3x3 pixels on the Flipper Zero screen; version 2 and 3
qrcodes will each have 2x2 pixel modules; and version 4 through 11 qrcodes will
have single pixel modules.

If unspecified in the `.qrcode` file, the app will automatically select the
lowest version that can contain all of the message content, given the mode
selected in the previous step.

### QRECC
A qrcode's ECC level determines the qrcode's resilience to "damage". In the
case of the Flipper Zero, "damage" might be a dirty screen, dead pixels, or
even screen glare. Higher ECC modes are more resilient, but can contain less
data. The ECC modes are Low, Medium, Quartile, and High and can be specified in
the `.qrcode` file using the first letter (L, M, Q, and H).

qrcode readers may have an easier time reading qrcodes with higher ECC levels,
so, if unspecified in the `.qrcode` file, the app will select the highest ECC
level that can contain all of the message content, given the qrcode mode and
version selected in the previous steps.

## Using the App
The app is fairly straightforward. When it first starts, the file browser will
automatically open to the `qrcodes` directory and display any `.qrcode` files.
Select one using the arrow keys and the center button. The qrcode will display.
If you push the right arrow, some stats will display: the qrcode "Version"; the
ECC level; and the qrcode Mode (Numeric, Alpha-Numeric, Binary, or Kanji).

While viewing the stats, you can select Version or ECC using the up and down
arrows and the center button. You can then increase or decrease the Version or
ECC using up and down and save your choice using the center buttton. This
feature was mostly added for my own amusement and testing, but, theoretically,
it may help a reader that's having trouble if the default ECC is less than the
highest value ("H"): you can increase the Version by 1 and then set the ECC to
"H". Whether or not this helps depends on the reader.

You can hide the stats by pressing the left arrow.

When you're done viewing the qrcode, press the back button to return to the
file browser. If you push the back button in the file browser, the app will
exit.

I will ask that you temper your expectations: the Flipper Zero screen is small
and many readers may have difficulty reading the qrcodes, especially if they
are encoding a lot of data. However, I have successfully got my iPhone to read
qrcodes encoding phone numbers, wifi info, and a url, all the way up to a
version 11 qrcode (ie, the largest size the screen will fit).

## Example: Wifi QRCodes
Most phones can automatically connect to wifi networks from a qrcode. If you
should like to encode your wifi's connection info into a qrcode, here's how
you'd do it:

```
Filetype: QRCode
Version: 0
Message: WIFI:S:<ssid>;P:<password>;T:<encryption>;
```

Replace `<ssid>` with the name of your wifi, `<password>` with the password.
`<encryption>` would be "WPA" or "WEP". If your wifi is open (no password),
this can be "None" and you can remove `P:<password>;` from the message. If your
wifi is hidden (ie, does not broadcast the ssid), you can add `H:true;` to the
end.

Note that if your ssid or password contain any of these characters: `\";,:`,
you'll need to "escape" it by placing a backslash (`\`) before it.

For example, if my ssid was "wifiball" and not broadcast, and the password was
"pa$$:word" with WPA encryption, the message would be:

```
Message: WIFI:S:wifiball;P:pa$$\:word;T:WPA;H:true;
```

## Example: vCard
Phones can scan [vCard] qrcodes to automatically add a contact to their address
book. Starting with qrcode v2, multi-line qrcodes can be created, allowing you
to create vCards!

```
Filetype: QRCode
Version: 1
Message: BEGIN:VCARD
Message: VERSION:3.0
Message: N:Smith;John
Message: FN:John Smith
Message: ADR;TYPE=dom,home,postal,parcel:;;123 Example St;Exampleton;CA;90210;
Message: BDAY:1970-01-01
Message: TEL;TYPE=pref,voice,msg,cell:+18005551212
Message: END:VCARD
```

Check the [vCard] specification to learn about all of the fields and their
values.

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
./fbt fap_qrcode
```

This will automatically install dependencies and build the application. When it
has finished building, the .fap will be in
`build/f7-firmware-D/.extapps/qrcode.fap` (fbt output will tell you where to
find the .fap, should it change in the future).

## qrcode library
This application uses the [QRCode] library by ricmoo. This is the same library
that is in the lib directory of the flipper-firmware repo (which was originally
included for a [now-removed demo app]), but modified slightly to fix some
compiler errors and allow the explicit selection of the qrcode mode.

[now-removed demo app]: https://github.com/flipperdevices/flipperzero-firmware/pull/160/files
[flipperzero-firmware]: https://github.com/flipperdevices/flipperzero-firmware
[Flipper Zero]: https://flipperzero.one/
[QRCode]: https://github.com/ricmoo/QRCode
[qFlipper]: https://docs.flipperzero.one/qflipper
[Releases]: https://github.com/bmatcuk/flipperzero-qrcode/releases/latest
[vCard]: https://www.evenx.com/vcard-3-0-format-specification
