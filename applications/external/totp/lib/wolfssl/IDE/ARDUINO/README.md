### wolfSSL with Arduino

##### Reformatting wolfSSL as a compatible Arduino Library
This is a shell script that will re-organize the wolfSSL library to be 
compatible with Arduino projects. The Arduino IDE requires a library's source
files to be in the library's root directory with a header file in the name of 
the library. This script moves all src/ files to the `IDE/ARDUINO/wolfSSL`
directory and creates a stub header file called `wolfssl.h`.

Step 1: To configure wolfSSL with Arduino, enter the following from within the
wolfssl/IDE/ARDUINO directory:

        `./wolfssl-arduino.sh`

Step 2: Copy the directory wolfSSL that was just created to:
`~/Documents/Arduino/libraries/` directory so the Arduino IDE can find it.

Step 3: Edit `<arduino-libraries>/wolfSSL/user_settings.h`
If building for Intel Galileo platform add: `#define INTEL_GALILEO`.
Add any other custom settings, for a good start see the examples in wolfssl root
"/examples/configs/user_settings_*.h"

Step 4: If you experience any issues with custom user_settings.h see the wolfssl
porting guide here for more assistance: https://www.wolfssl.com/docs/porting-guide/

Step 5: If you still have any issues contact support@wolfssl.com for more help.

##### Including wolfSSL in Arduino Libraries (for Arduino version 1.6.6)

1. In the Arduino IDE:
    - In `Sketch -> Include Library -> Add .ZIP Library...` and choose the
        `IDE/ARDUNIO/wolfSSL` folder.
    - In `Sketch -> Include Library` choose wolfSSL.

2. Open an example Arduino sketch for wolfSSL:
	- wolfSSL Client INO sketch: `sketches/wolfssl_client/wolfssl_client.ino`
	- wolfSSL Server INO sketch: `sketches/wolfssl_server/wolfssl_server.ino`
