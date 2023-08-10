# Silicon Labs Simplicity Studio

Tested with ERF32xG21 Starter Kit and Gecko SDK v3.2.2 and v4.2.3.

## Simplicity Studio Example

Based on `cli_kernel_freertos`.
* Create a new example project for your board based on the CLI FreeRTOS example.
* Create a new `wolfssl` and put wolfSSL into it. Tip: Use `./scripts/makedistsmall.sh` to produce a reduced bundle.
* Exclude (or delete) all .S and asm.c files.
* Exclude (or delete) directory not used (only the `src`, `wolfcrypt` and `wolfssl` directory are used).
* Add the `IDE/SimplicityStudio/user_settings.h` into `wolfssl/user_settings.h`.
* Add the `IDE/SimplicityStudio/test_wolf.c` to the project root.
* Add C preprocessor `WOLFSSL_USER_SETTINGS`.
* Add C include path `wolfssl`.
* Disable UART flow control: `config/sl_iostream_usart_vcom_config.h` -> `#define SL_IOSTREAM_USART_VCOM_FLOW_CONTROL_TYPE usartHwFlowControlNone`
* Adjust stack and heap to larger size 12KB:
  - Adjust the CLI stack. Edit `config/sl_cli_config_inst.h` to `#define SL_CLI_INST_TASK_STACK_SIZE (12*1024)`
  - Adjust the FreeRTOS heap. Edit `config/FreeRTOSConfig.h` to `#define configTOTAL_HEAP_SIZE (12*1024)`
* Add the following code to `cli.c`:

```c
extern void wolf_test(sl_cli_command_arg_t *arguments);
extern void wolf_bench(sl_cli_command_arg_t *arguments);

static const sl_cli_command_info_t cmd_wolf_test =
  SL_CLI_COMMAND(wolf_test, "Run wolfCrypt tests", "",
                 { SL_CLI_ARG_WILDCARD, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cmd_wolf_bench =
  SL_CLI_COMMAND(wolf_bench, "Run wolfCrypt benchmarks", "",
                 { SL_CLI_ARG_WILDCARD, SL_CLI_ARG_END, });

static sl_cli_command_entry_t a_table[] = {
  { "wolf_test", &cmd_wolf_test, false },
  { "wolf_bench", &cmd_wolf_bench, false },
  { NULL, NULL, false },
};
```

* If running wolfCrypt benchmark enable printf float
  - `C/C++ Build Settings` -> `Settings` -> `Tool Settings` -> `GNU ARM C Linker` -> `General` -> `C Library "Printf float"`.

## SE Manager

For SE Manager cryptography hardware acceleration see [/wolfcrypt/src/port/silabs/README.md](/wolfcrypt/src/port/silabs/README.md).

Enabled with `WOLFSSL_SILABS_SE_ACCEL`. Requires the "SE Manager" component to be installed.

## Benchmarks with SE Accel

Tested on ERF32xG21 Starter Kit (Cortex M33 at 80 MHz) and Gecko SDK v3.2.2 (-Os).

```
Benchmark Test
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                        200 KiB took 1.057 seconds,  189.215 KiB/s
AES-128-CBC-enc              6 MiB took 1.000 seconds,    5.542 MiB/s
AES-128-CBC-dec              6 MiB took 1.000 seconds,    5.518 MiB/s
AES-192-CBC-enc              5 MiB took 1.001 seconds,    5.415 MiB/s
AES-192-CBC-dec              5 MiB took 1.001 seconds,    5.390 MiB/s
AES-256-CBC-enc              5 MiB took 1.004 seconds,    5.301 MiB/s
AES-256-CBC-dec              5 MiB took 1.001 seconds,    5.268 MiB/s
AES-128-GCM-enc              5 MiB took 1.003 seconds,    4.844 MiB/s
AES-128-GCM-dec              5 MiB took 1.003 seconds,    4.625 MiB/s
AES-192-GCM-enc              5 MiB took 1.002 seconds,    4.751 MiB/s
AES-192-GCM-dec              5 MiB took 1.002 seconds,    4.532 MiB/s
AES-256-GCM-enc              5 MiB took 1.002 seconds,    4.654 MiB/s
AES-256-GCM-dec              4 MiB took 1.000 seconds,    4.443 MiB/s
AES-128-GCM-enc-no_AAD       5 MiB took 1.004 seconds,    4.888 MiB/s
AES-128-GCM-dec-no_AAD       5 MiB took 1.001 seconds,    4.658 MiB/s
AES-192-GCM-enc-no_AAD       5 MiB took 1.000 seconds,    4.785 MiB/s
AES-192-GCM-dec-no_AAD       5 MiB took 1.000 seconds,    4.565 MiB/s
AES-256-GCM-enc-no_AAD       5 MiB took 1.004 seconds,    4.693 MiB/s
AES-256-GCM-dec-no_AAD       4 MiB took 1.003 seconds,    4.479 MiB/s
GMAC Small                   5 MiB took 1.000 seconds,    4.653 MiB/s
CHACHA                       2 MiB took 1.012 seconds,    1.809 MiB/s
CHA-POLY                     1 MiB took 1.006 seconds,    1.189 MiB/s
POLY1305                     5 MiB took 1.004 seconds,    5.082 MiB/s
SHA                          8 MiB took 1.000 seconds,    7.812 MiB/s
SHA-256                      8 MiB took 1.000 seconds,    8.032 MiB/s
HMAC-SHA                     7 MiB took 1.000 seconds,    7.056 MiB/s
HMAC-SHA256                  7 MiB took 1.002 seconds,    7.237 MiB/s
RSA     2048   public        30 ops took 1.022 sec, avg 34.067 ms, 29.354 ops/sec
RSA     2048  private         2 ops took 2.398 sec, avg 1199.000 ms, 0.834 ops/sec
ECC   [      SECP256R1]   256  key gen       172 ops took 1.004 sec, avg 5.837 ms, 171.315 ops/sec
ECDHE [      SECP256R1]   256    agree       186 ops took 1.005 sec, avg 5.403 ms, 185.075 ops/sec
ECDSA [      SECP256R1]   256     sign       174 ops took 1.007 sec, avg 5.787 ms, 172.790 ops/sec
ECDSA [      SECP256R1]   256   verify       160 ops took 1.003 sec, avg 6.269 ms, 159.521 ops/sec
Benchmark complete
Benchmark Test: Return code 0
```
