# MSP430 Example

This example was designed to be used with the MSP430F5359/MSP430F5659 but can be ported to any similar MSP platform. It will take ~50KB of ROM space and a 8KB of statically allocated RAM (nearly half for constants).

The example runs at 8MHz and executes a benchmark of ECC key generations, shared secrets and 1KB ChaCha20/Poly1305 encryption.

At 8MHz the ECC steps will take 13-15 seconds each and 1000 iterations of ChaCha20/Poly1305 will take about 45 seconds.

## Hardware Setup

A basic development board / ISP combination will work fine, such as the MSP-TS430PZ100 series with the MSP-FET430 series programmer.

The example will output text via UART 1, on the MSP430 which is port 8 bits 2&3 (pins 60/61) on the MSP430F5359. The UART will run at 57600 baud.

In addition every second port 1 bit 1 will be toggled on/off (typically an LED would be here).

## IDE setup

When setting up the IDE, copy the wolfSSL source code to your project's directory and add all the .c and .h files from `wolfcrypt/src` to your project.

Use the `main.c` provided here and copy the `user_settings.h` file to the `wolfssl/wolfcrypt` subdirectory of wolfSSL.

You will need to set at least 700 bytes of stack, no heap is required. You will also need to change the "Library Configuration" to "Full DLIB" so the `printf()` functions work correctly.

Make sure to add the definition `WOLFSSL_USER_SETTINGS` to the preprocessor settings in your project to that `user_settings.h` is loaded in. You will also need to add the wolfSSL root directory to the "Additional include directories".

From here you can set any optimizer settings you need.

## MSP430 GCC Makefile

Also included is a `Makefile` for TI's GCC, when compiling with GCC the code size will be larger and the application will be ~2x slower.

To use this Makefile you will need [TI's MSP430 GCC](https://www.ti.com/tool/MSP430-GCC-OPENSOURCE) installed as well as `mspdebug` with the `libmsp430.so` accessible to it. You will need to edit the `Makefile` to set the `TI_COMPILER_PATH` to where you have installed this.

Once everything is in place you can run `make` to build it and `make install` to flash the MSP430 with it.

**Note**: this will not work with the much older version of MSP430 GCC that comes in Linux distribution repositories.

## Example Output

This is an example output for the demo application when compiled with IAR.

```
START!
Rand generated: 2317
Rand generated: -31901
Rand generated: 13538
Rand generated: -24035
Rand generated: 18849
Rand generated: -1593
Rand generated: 29653
Rand generated: -8148
Rand generated: -27438
Rand generated: 618
Rand generated: -17119
Rand generated: 4668
Rand generated: -26289
Rand generated: 28126
Rand generated: -15749
Rand generated: 22041
Rand generated: 8710
Rand generated: -22039
Rand generated: 1781
Rand generated: -32168
Rand generated: 6187
Rand generated: -7650
Rand generated: 30268
Rand generated: -13585
Rand generated: 24388
Rand generated: 8520
RNG init
Alice init
Bob init
.............Alice keygen 13 seconds
..............Bob keygen 14 seconds
.............Bob secret 13 seconds
..............Alice secret 14 seconds
Successfully generated a common secret
Alices' Secret: 85f3c7f599620c768e6dbb77dc2f8f764254cc1821aeb0a30503632dbc9bdb54
Bobs' Secret: 85f3c7f599620c768e6dbb77dc2f8f764254cc1821aeb0a30503632dbc9bdb54
ChaCha20/Poly1305 Encryption Start, 1000 iterations, 1024 bytes
............................................
End 44 seconds
ChaCha20/Poly1305 Decryption Start, 1000 iterations
............................................
End 44 seconds
end
```
