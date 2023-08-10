# NuttX + wolfSSL

## Installation

### Installing from nuttx-apps

Clone wolfssl into the the the crypto/wolfssl folder

```
git clone https://github.com/wolfSSL/wolfssl.git apps/crypto/wolfssl/wolfssl
```

Skip to step 6

### Installing from wolfssl

1) Create working directory (e.g. ~/nuttxspace):
    ```
    $ cd ~
    $ mkdir nuttxspace
    ```
2) Install dependencies:
    ```
    $ cd ~/nuttxspace
    $ sudo apt install -y bison flex gettext texinfo libncurses5-dev libncursesw5-dev gperf automake libtool pkg-config build-essential gperf genromfs libgmp-dev libmpc-dev libmpfr-dev libisl-dev binutils-dev libelf-dev libexpat-dev gcc-multilib g++-multilib picocom u-boot-tools util-linux
    $ sudo apt install -y kconfig-frontends
    $ sudo apt install -y gcc-arm-none-eabi binutils-arm-none-eabi
    ```
3) Clone nuttx and nuttx-apps into working directory:
    ```
    $ git clone https://github.com/apache/nuttx.git nuttx
    $ git clone https://github.com/apache/nuttx-apps apps
    ```
4) Copy this directory into the working directory applications:
    ```
    $ cp -R RTOS/nuttx/wolfssl ~/nuttxspace/apps/crypto/wolfssl
    ```
5) Setup wolfSSL in preparation for the build, `WOLFSSL_DIR` must be the path to the original wolfssl repo:
    ```
    $ cd ~/nuttxspace/apps/crypto/wolfssl
    $ WOLFSSL_DIR=<path-to-wolfssl-repo> ./setup-wolfssl.sh
    ```
6) Setup baseline NuttX configuration (board + NuttX Shell):
    ```
    $ cd ~/nuttxspace/nuttx
    $ ./tools/configure.sh -l <board>:nsh
    ```
   If you are using wolfSSL for TLS you should use the `netnsh` target if your board supports it
   ```
    $ ./tools/configure.sh -l <board>:netnsh
   ```
> **EXAMPLES:**
>   - For NuttX Simulator: `$ ./tools/configure.sh sim:nsh`
>   - For BL602 (RISC-V): `$ ./tools/configure.sh -l bl602evb:nsh`
>   - For NUCLEO-L552ZE-Q (Cortex-M33): `$ ./tools/configure.sh -l nucleo-l552ze:nsh`
>   - For NUCLEO-H753ZI: `$ ./tools/configure.sh -l nucleo-h743zi:nsh`
>   - For NUCLEO-F756ZG: `./tools/configure.sh -l nucleo-144:f746-nsh`

7) Start custom configuration system:
    ```
    $ make menuconfig
    ```
8) Configure NuttX to enable the wolfSSL crypto library test applications:
    - From main menu select: **Application Configuration > Cryptography Library Support**
    - Enable and then select **wolfSSL SSL/TLS Cryptography Library**
    - Enable and then select **wolfSSL applications**
    - Enable applications:
        - **wolfCrypt Benchmark application**
        - **wolfCrypt Test application**
        - **wolfSSL client and server example**
    - Select Save from bottom menu, saving to `.config` file
    - Exit configuration tool

    If you are using wolfSSL for TLS you should use the `netnsh` target and should enable an NTP or some for of system time keeping so that wolfSSL has the current date to check certificates. You will also need to set the right networking settings for NuttX to connect to the internet.
9) Build NuttX and wolfSSL:
    ```
    $ make
    ```
10) Flash the target
    ### Simulator
      ./nuttx
    ### STM32 Targets (address may vary)
      STM32_Programmer_CLI -c port=swd -d ./nuttx.bin 0x08000000
11) Connect to the target with a serial monitoring tool, the device on linux is usually /dev/ttyACM0 but it may vary
    - minicom -D /dev/ttyACM0
12) Run the wolfcrypt benchmark and/or test in the NuttX Shell:
    ```
    nsh> wolfcrypt_test
    nsh> wolfcrypt_benchmark
    nsh> wolfssl_client_server
    ```
## Notes
- Developed using the following targets:
    - STM NUCLEO-L552ZE-Q (Cortex-M33)
    - STM NUCLEO-H753ZI
    - STM NUCLEO-F756ZG
    - DT-BL10 / BL602 (RISC-V)
    - NuttX simulator
