# Flipper Authenticator config file description

By default Flipper Authenticator stores all its settings in `/ext/apps_data/totp/totp.conf` file.

File format is standard for Flipper Zero device. Each line has one setting identified by key, where key and value are separated by `:` symbol.

## Available keys

<details>
<summary><h3 style="display: inline">Filetype</h3></summary>
<p>

**Type:** const string

**Description:** File type definition. Used internally. Should not be updated manually

</p>
</details>

<details>
<summary><h3 style="display: inline">Version</h3></summary>
<p>

**Type:** const unsigned int

**Description:** File version. Used internally. Should not be updated manually.

</p>
</details>

<details>
<summary><h3 style="display: inline">CryptoVersion</h3></summary>
<p>

**Type:** const unsigned int

**Description:** Cryptographic algorithms version used for encryption\decryption. Should not be updated manually.

</p>
</details>

<details>
<summary><h3 style="display: inline">CryptoKeySlot</h3></summary>
<p>

**Type:** const unsigned int

**Description:** Key vault slot to be used for encryption\decryption. Should not be updated manually, instead CLI should be used to update encryption parameters.

</p>
</details>

<details>
<summary><h3 style="display: inline">BaseIV</h3></summary>
<p>

**Type:** array of bytes

**Default value:** none

**Description:** Initialization vector (IV) base seed which is getting generated randomly at first app start. It is used to setup encryption subsytem. Should not be updated manually.

**Important note:** changing or loosing this value will lead to incorrect decryption of all the encrypted data in the application and as a result it will not be possible to generate valid OTP tokens

</p>
</details>

<details>
<summary><h3 style="display: inline">Crypto</h3></summary>
<p>

**Type:** array of bytes

**Default value:** none

**Description:** Used internally to verify user's PIN and\or file consistency. Should not be changed manually.

**Important note:** changing or loosing this value will lead to incorrect PIN verification and it will not be possible to signin into app

</p>
</details>

<details>
<summary><h3 style="display: inline">Timezone</h3></summary>
<p>

**Type:** float

**Default value:** 0.000000

**Description:** Timezone offset **in hours**. Need to be modified manually. Because of Flipper Zero API doesn't provide an access to timezone offset it is necessary to set it manually for correct TOTP tokens generation. You may find your timezone offset (or another name is "UTC offset") [here](https://www.utctime.net/time-zone-abbreviations), [here](https://time.is/) or on any other website found in [google](https://letmegooglethat.com/?q=What+is+my+timezone+offset).

**Important note:** if your timezone offset is negative, use negative sign, like this `-2.0`, however if your timezone offset is positive DO NOT use explicit positive sign, just put offset without any sign like this `2.0`

</p>
</details>

<details>
<summary><h3 style="display: inline">PinIsSet</h3></summary>
<p>

**Type:** bool

**Default value:** none

**Description:** Used internally to identify whether user set PIN or not. Should not be changed manually.

**Important note:** changing or loosing this value will lead to incorrect PIN verification and it will not be possible to signin into app

</p>
</details>

<details>
<summary><h3 style="display: inline">NotificationMethod</h3></summary>
<p>

**Type:** enum (available options are `0`, `1`, `2`, `3`)

**Default value:** `3`

**Description:** How to notify user when new token is generated or badusb mode is activated. Possible values are:

* `0` - do not notify
* `1` - notify using sound only
* `2` - notify using vibro only
* `3` - notify using sound and vibro

</p>
</details>

<details>
<summary><h3 style="display: inline">AutomationMethod</h3></summary>
<p>

**Type:** enum (available options are `0`, `1`, `2`, `3`)

**Default value:** `1`

**Description:** Which OTP code input automation method is enabled for the user. Possible values are:

* `0` - none
* `1` - USB - application will represent itself as an USB keyboard and type current code (BadUSB)
* `2` - Bluetooth - application will represent itself as an Bluetooth keyboard and type current code (BadBT)
* `3` - USB and Bluetooth

</p>
</details>

<details>
<summary><h3 style="display: inline">Font</h3></summary>
<p>

**Type:** enum (available options are `0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`)

**Default value:** `7`

**Description:** Font index to be used to display token. Can be modified manually.

</p>
</details>

<details>
<summary><h3 style="display: inline">AutomationKbLayout</h3></summary>
<p>

**Type:** enum (available options are `0`, `1`)

**Default value:** `0`

**Description:** Keyboard layout index to be used during token input automation. Can be modified manually.

* `0` - QWERTY
* `1` - AZERTY
* `3` - QWERTZ

</p>
</details>

<details>
<summary><h3 style="display: inline">TokenName</h3></summary>
<p>

**Type:** string

**Default value:** none

**Description:** Token name which will be visible in the UI and used just to let user identify token. Can be modified manually.

</p>
</details>

<details>
<summary><h3 style="display: inline">TokenSecret</h3></summary>
<p>

**Type:** array of bytes OR string

**Default value:** none

**Description:** Token secret. It can be either an array of encrypted bytes OR pure unencrypted token secret.

**Important note:** if app finds pure unencrypted token in config file app will encrypt it and replace in a config file for security purposes

</p>
</details>

<details>
<summary><h3 style="display: inline">TokenAlgo</h3></summary>
<p>

**Type:** enum (available options are: `0`, `1`, `2`, `3`)

**Default value:** `0`

**Description:** Token hashing algorithm to be used to generate OTP code. If you don't know which one to use - use `0`.

* `0` - `SHA1`
* `1` - `SHA256`
* `2` - `SHA512`
* `3` - `STEAM`

</p>
</details>

<details>
<summary><h3 style="display: inline">TokenDigits</h3></summary>
<p>

**Type:** enum (available options are `6` and `8`)

**Default value:** `6`

**Description:** Defines OTP code length. If you don't know which to use - use `6` as majority of websites requires 6-digits code.

</p>
</details>

<details>
<summary><h3 style="display: inline">TokenDuration</h3></summary>
<p>

**Type:** unsigned int

**Default value:** `30`

**Description:** Token lifetime duration in seconds. Applicable for TOTP tokens only. Should be between `15` and `255`. Majority websites requires `30`, however some rare websites may require custom lifetime. If you are not sure which one to use - use `30`.

</p>
</details>

<details>
<summary><h3 style="display: inline">TokenAutomationFeatures</h3></summary>
<p>

**Type:** unsigned int

**Default value:** `0` (no features enabled)

**Description:** Token automation features. Meaningful bits:

* Bit 1 - if set, token input automation will type \<ENTER\> key at the end of automation
* Bit 2 - if set, token input automation will type \<TAB\> key at the end of automation
* Bit 3 - if set, token input automation will type slower

</p>
</details>

<details>
<summary><h3 style="display: inline">TokenType</h3></summary>
<p>

**Type:** enum (available options are `0` and `1`)

**Default value:** `0` (Time-based token, TOTP)

**Description:** Token type.

* `0` - Time-based (TOTP)
* `1` - Counter-based (HOTP)

</p>
</details>

<details>
<summary><h3 style="display: inline">TokenCounter</h3></summary>
<p>

**Type:** unsigned int

**Default value:** `0`

**Description:** Token counter. Applicable for HOTP tokens only.

</p>
</details>

## Example config file

```text
Filetype: Flipper TOTP plugin config file
Version: 10
CryptoVersion: 2
CryptoKeySlot: 12
BaseIV: 71 88 EF D4 F0 3E 40 B5 3D 0A 0E 39 C2 AF C5 D6
Crypto: 43 41 1F 91 A8 41 77 DB D7 EC 9D 0E 23 36 B5 0B
Timezone: 2.000000
PinIsSet: true
NotificationMethod: 0
AutomationMethod: 3
Font: 7
AutomationKbLayout: 0
TokenName: Steam token
TokenSecret: C7 F5 35 9D 77 B6 99 8A 5F 05 71 EB 9F DD A4 07 29 82 1E 50 C8 A7 D0 E9 9D 21 52 56 6C 09 DA 38
TokenAlgo: 3
TokenDigits: 5
TokenDuration: 30
TokenAutomationFeatures: 0
TokenType: 0
TokenCounter: 0000000000000000
TokenName: sha1 token
TokenSecret: 1F 13 EC CB C7 24 32 BC 48 28 92 BB A6 A2 60 68 D6 DA 74 65 53 08 84 BD 7D 2B 4F FC 17 F1 C8 35 3E B0 37 EA 82 69 95 00 47 5E DA 12 7F 3C 73 05 10 A1 E6 89 1D C0 58 A7 E4 1D 46 B9 28 9E 16 48
TokenAlgo: 0
TokenDigits: 6
TokenDuration: 30
TokenAutomationFeatures: 0
TokenType: 0
TokenCounter: 0000000000000000
TokenName: sha256 token
TokenSecret: 80 02 69 36 AC 28 6B 83 08 90 75 81 CA 39 4B 11 C9 69 C7 45 C2 F8 98 14 D7 BB 1E B8 03 BB 52 3E 79 6E 1D F3 A7 E5 89 99 06 4E 08 87 66 9A BD C2 D9 95 65 ED 3A 40 50 F5 D3 2B C4 FE 48 D0 78 4B
TokenAlgo: 1
TokenDigits: 6
TokenDuration: 30
TokenAutomationFeatures: 0
TokenType: 0
TokenCounter: 0000000000000000
```
