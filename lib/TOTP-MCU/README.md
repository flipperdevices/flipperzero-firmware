TOTP Pure C Library for ALL MCU
====================

Library to generate Time-based One-Time Passwords.

Implements the Time-based One-Time Password algorithm specified in [RFC 6238](https://tools.ietf.org/html/rfc6238). 
Supports different time steps and is compatible with tokens that use the same standard (including software ones, like the Google Authenticator app).

Tested on MCUs: MSP430, RP2040

Installation & usage:
--------------------
First include header to your file
```
#include <totp.h>
```
After included, define key ex. Key is ```MyLegoDoor```
- Note: The format of hmacKey is array of hexadecimal bytes.
- Most websites provide the key encoded in base32 - RFC3548/RFC4648, either upper or lower case. You can use [this site](https://cryptii.com/pipes/base32-to-hex) to convert the base32 string to hex (make sure you upcase it first if it's lowercase and remove all whitespaces).
```
uint8_t hmacKey[] = {0x4d, 0x79, 0x4c, 0x65, 0x67, 0x6f, 0x44, 0x6f, 0x6f, 0x72};               // Secret key
```
Instantiate the TOTP class by providing the secret hmacKey, the length of the hmacKey and the Timestep between codes.
```
TOTP(hmacKey, 10, 30);                                     // Secret key, Secret key length, Timestep (30s)
```
Use the ```getCodeFromTimestamp()``` function to get a TOTP from a unix epoch timestamp
```
uint32_t newCode = getCodeFromTimestamp(1557414000);       // Current timestamp since Unix epoch in seconds
```
Or ```getCodeFromTimeStruct()``` if you want to get a TOTP from a tm struct (Time Struct in C), 
```
struct tm datetime;
datetime.tm_hour = 9;
datetime.tm_min = 0;
datetime.tm_sec = 0;
datetime.tm_mday = 13;
datetime.tm_mon = 5;
datetime.tm_year = 2019;
uint32_t newCode = getCodeFromTimeStruct(datetime);
```

If the provided unix timestamp isn't in UTC±0, use ```setTimezone()``` before ```getCodeFromTimestamp()``` or ```getCodeFromTimeStruct()``` to offset the time.

```
setTimezone(9);                                            // Set timezone +9 Japan
```

You can see an example in blink.c

Thanks to:
----------

* Jose Damico, https://github.com/damico/ARDUINO-OATH-TOKEN
* Peter Knight, https://github.com/Cathedrow/Cryptosuite
* Maniacbug, https://github.com/maniacbug/Cryptosuite
* lucadentella, https://github.com/lucadentella/TOTP-Arduino
