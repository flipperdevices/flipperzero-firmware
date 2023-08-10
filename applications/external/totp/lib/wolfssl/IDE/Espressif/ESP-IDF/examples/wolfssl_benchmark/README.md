# wolfSSL Benchmark Example

The Example contains of wolfSSL benchmark program.

1. `idf.py menuconfig` to configure the program.  
    1-1. Example Configuration ->

    BENCH_ARG : argument that you want to use. Default is "-lng 0"  
    The list of argument can be find in help.

When you want to run the benchmark program

1. `idf.py -p <PORT> flash` to compile and load the firmware
2. `idf.py monitor` to see the message

See the README.md file in the upper level 'examples' directory for more information about examples.

## Performance

Reminder than when building on WSL in `/mnt/c` there will be a noticeable performance degradation at compile time. Using `~/` will be faster at the cost of shared filesystems.

Example build on WSL:

```
Optionally install wolfSSL component
# cd /mnt/c/workspace/wolfssl/IDE/Espressif/ESP-IDF
./setup.sh

cd /mnt/c/workspace/wolfssl/IDE/Espressif/ESP-IDF/examples/wolfssl_benchmark

# Pick ESP-IDF install directory, this one for v4.4.2 in VisualGDB
. /mnt/c/SysGCC/esp32/esp-idf/v4.4.2/export.sh


idf.py build flash -p /dev/ttyS20 -b 921600 monitor
```

## Example Output

Note the default wolfSSL `user_settings.h` is configured by default to be the most 
compatible across the widest ranges of targets. Contact wolfSSL at support@wolfssl.com
for help in optimizing for your particular application, or see the 
[docs](https://www.wolfssl.com/documentation/manuals/wolfssl/index.html).

Compiled and flashed with `idf.py build  flash -p /dev/ttyS7 -b 921600 monitor`:

```
--- idf_monitor on /dev/ttyS7 115200 ---
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
 (377) cpu_start: Starting scheduler ets Jun  8 2016 00:22:57

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:6664
load:0x40078000,len:14848
load:0x40080400,len:3792
0x40080400: _init at ??:?

entry 0x40080694
I (27) boot: ESP-IDF v4.4.2-dirty 2nd stage bootloader
I (27) boot: compile time 13:41:41
I (27) boot: chip revision: 1
I (30) boot_comm: chip revision: 1, min. bootloader chip revision: 0
I (37) boot.esp32: SPI Speed      : 40MHz
I (42) boot.esp32: SPI Mode       : DIO
I (46) boot.esp32: SPI Flash Size : 2MB
I (51) boot: Enabling RNG early entropy source...
I (56) boot: Partition Table:
I (60) boot: ## Label            Usage          Type ST Offset   Length
I (67) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (75) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (82) boot:  2 factory          factory app      00 00 00010000 00100000
I (90) boot: End of partition table
I (94) boot_comm: chip revision: 1, min. application chip revision: 0
I (101) esp_image: segment 0: paddr=00010020 vaddr=3f400020 size=12bf4h ( 76788) map
I (137) esp_image: segment 1: paddr=00022c1c vaddr=3ffb0000 size=02420h (  9248) load
I (141) esp_image: segment 2: paddr=00025044 vaddr=40080000 size=0afd4h ( 45012) load
I (161) esp_image: segment 3: paddr=00030020 vaddr=400d0020 size=33148h (209224) map
I (237) esp_image: segment 4: paddr=00063170 vaddr=4008afd4 size=00550h (  1360) load
I (238) esp_image: segment 5: paddr=000636c8 vaddr=50000000 size=00010h (    16) load
I (249) boot: Loaded app from partition at offset 0x10000
I (249) boot: Disabling RNG early entropy source...
I (266) cpu_start: Pro cpu up.
I (266) cpu_start: Starting app cpu, entry point is 0x40081098
0x40081098: call_start_cpu1 at /mnt/c/SysGCC/esp32/esp-idf/v4.4.2/components/esp_system/port/cpu_start.c:160

I (0) cpu_start: App cpu up.
I (280) cpu_start: Pro cpu start user code
I (280) cpu_start: cpu freq: 240000000
I (280) cpu_start: Application information:
I (285) cpu_start: Project name:     wolfssl_benchmark
I (291) cpu_start: App version:      v5.5.3-stable-108-gbd7b442df-di
I (298) cpu_start: Compile time:     Nov 17 2022 14:10:03
I (304) cpu_start: ELF file SHA256:  fbb520f5bbf963a0...
I (310) cpu_start: ESP-IDF:          v4.4.2-dirty
I (316) heap_init: Initializing. RAM available for dynamic allocation:
I (323) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (329) heap_init: At 3FFB3DE8 len 0002C218 (176 KiB): DRAM
I (335) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (341) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (348) heap_init: At 4008B524 len 00014ADC (82 KiB): IRAM
I (355) spi_flash: detected chip: generic
I (359) spi_flash: flash io: dio
W (362) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (377) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (391) wolfssl_benchmark: app_main CONFIG_BENCH_ARGV = -lng 0
I (397) wolfssl_benchmark: construct_argv arg:-lng 0

------------------------------------------------------------------------------
 wolfSSL version 5.5.3
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                          1 MiB took 1.017 seconds,    1.320 MiB/s
AES-128-CBC-enc              6 MiB took 1.002 seconds,    5.726 MiB/s
AES-128-CBC-dec              5 MiB took 1.000 seconds,    5.347 MiB/s
AES-192-CBC-enc              6 MiB took 1.004 seconds,    5.714 MiB/s
AES-192-CBC-dec              5 MiB took 1.001 seconds,    5.341 MiB/s
AES-256-CBC-enc              6 MiB took 1.000 seconds,    5.713 MiB/s
AES-256-CBC-dec              5 MiB took 1.002 seconds,    5.336 MiB/s
AES-128-GCM-enc            300 KiB took 1.004 seconds,  298.805 KiB/s
AES-128-GCM-dec            300 KiB took 1.004 seconds,  298.805 KiB/s
AES-192-GCM-enc            300 KiB took 1.007 seconds,  297.915 KiB/s
AES-192-GCM-dec            300 KiB took 1.008 seconds,  297.619 KiB/s
AES-256-GCM-enc            300 KiB took 1.011 seconds,  296.736 KiB/s
AES-256-GCM-dec            300 KiB took 1.011 seconds,  296.736 KiB/s
GMAC Default               403 KiB took 1.002 seconds,  402.196 KiB/s
3DES                       450 KiB took 1.028 seconds,  437.743 KiB/s
MD5                         14 MiB took 1.001 seconds,   13.756 MiB/s
SHA                         14 MiB took 1.001 seconds,   14.463 MiB/s
SHA-256                     14 MiB took 1.000 seconds,   14.233 MiB/s
SHA-512                     17 MiB took 1.000 seconds,   16.626 MiB/s
HMAC-MD5                    14 MiB took 1.000 seconds,   13.599 MiB/s
HMAC-SHA                    14 MiB took 1.000 seconds,   13.989 MiB/s
HMAC-SHA256                 14 MiB took 1.000 seconds,   13.940 MiB/s
HMAC-SHA512                 16 MiB took 1.000 seconds,   16.064 MiB/s
PBKDF2                     640 bytes took 1.009 seconds,  634.291 bytes/s
RSA     2048   public        52 ops took 1.022 sec, avg 19.654 ms, 50.881 ops/sec
RSA     2048  private         4 ops took 1.056 sec, avg 264.000 ms, 3.788 ops/sec
ECC   [      SECP256R1]   256  key gen         4 ops took 1.216 sec, avg 304.000 ms, 3.289 ops/sec
ECDHE [      SECP256R1]   256    agree         4 ops took 1.215 sec, avg 303.750 ms, 3.292 ops/sec
ECDSA [      SECP256R1]   256     sign         4 ops took 1.226 sec, avg 306.500 ms, 3.263 ops/sec
ECDSA [      SECP256R1]   256   verify         2 ops took 1.172 sec, avg 586.000 ms, 1.706 ops/sec
CURVE  25519  key gen         3 ops took 1.279 sec, avg 426.333 ms, 2.346 ops/sec
CURVE  25519    agree         4 ops took 1.701 sec, avg 425.250 ms, 2.352 ops/sec
ED     25519  key gen        46 ops took 1.008 sec, avg 21.913 ms, 45.635 ops/sec
ED     25519     sign        42 ops took 1.038 sec, avg 24.714 ms, 40.462 ops/sec
ED     25519   verify        26 ops took 1.009 sec, avg 38.808 ms, 25.768 ops/sec
Benchmark complete
```

## Known Issues and Common Issues

### Permission denied

Delete project directory `build`, `.visualgdb` and `.vs` directories. Ensure no other build
process is using the same files in Windows, WSL, Visual Studio, VSCode, etc.

Example message:

```
-- Building ESP-IDF components for target esp32
CMake Error at /mnt/c/SysGCC/esp32/esp-idf/v4.4.2/tools/cmake/component.cmake:131 (file):
  file failed to open for writing (Permission denied):

    /mnt/c/workspace-pr/wolfssl/IDE/Espressif/ESP-IDF/examples/wolfssl_test/build/bootloader/component_properties.temp.cmake
```


### user_settings.h: No such file or directory

Ensure wolfSSL is installed in the ESP-IDF or local project `components` directory.

Example message:

```
 esp-idf/main/CMakeFiles/__idf_main.dir/main.c.obj   -c ../main/main.c
../main/main.c:27:10: fatal error: user_settings.h: No such file or directory
 #include <user_settings.h>
          ^~~~~~~~~~~~~~~~~
compilation terminated.
```

A 'clean` may be needed after freshly installing a new component:

```
idf.py clean build  flash -p /dev/ttyS7 -b 921600 monitor
```