# Silicon Labs (silabs) Port

Support for the Silicon Labs hardware acceleration

Tested on ERF32 Gecko Series 2 device config 1 (Secure Element)

* https://docs.silabs.com/mcu/latest/efr32mg21/group-SE
* https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager

## Building

To enable support define the following:

```
#define WOLFSSL_SILABS_SE_ACCEL
```

## Simplicity Studio Example

For the Silicon Labs Simplicity Studio example see [/IDE/SimplicityStudio/README.md](/IDE/SimplicityStudio/README.md).

## Caveats

:warning: **Be sure to update the SE firmware** Testing and results were done using SE firmware `1.2.6`

Update was performed under Simplicity Studio directory:
    `./developer/adapter_packs/commander/commander  flash ./offline/efr32/firmware/series2config1/se_firmware_package/s2c1_se_fw_upgrade_app_1v2p6.hex`

 * AES GCM tags length >= 16 bytes
 * By default random generator is seeded by the TRNG, but not used to
   generate all random data. `WOLFSSL_SILABS_TRNG` can be set to
   generate all random data with hardware TRNG. On early SE firmware
   versions requesting too much data or too quickly may result in
   system reset and setting `SESYSREQ`.

### Multi-threading

The SE manager supports multi-threading for FreeRTOS and Micrium
([ref](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager#autotoc-md152)).
If a different OS is used with multi-threading, additional mutex protection may be necessary.

## Benchmarks

See our [benchmarks](https://www.wolfssl.com/docs/benchmarks/) on the wolfSSL website.

```
RNG                  2 MB took 1.004 seconds,    1.897 MB/s
AES-128-CBC-enc      5 MB took 1.001 seconds,    4.902 MB/s
AES-128-CBC-dec      5 MB took 1.004 seconds,    4.912 MB/s
AES-192-CBC-enc      5 MB took 1.002 seconds,    4.800 MB/s
AES-192-CBC-dec      5 MB took 1.000 seconds,    4.810 MB/s
AES-256-CBC-enc      5 MB took 1.001 seconds,    4.707 MB/s
AES-256-CBC-dec      5 MB took 1.005 seconds,    4.713 MB/s
AES-128-GCM-enc      4 MB took 1.000 seconds,    4.468 MB/s
AES-128-GCM-dec      4 MB took 1.005 seconds,    4.324 MB/s
AES-192-GCM-enc      4 MB took 1.003 seconds,    4.381 MB/s
AES-192-GCM-dec      4 MB took 1.001 seconds,    4.244 MB/s
AES-256-GCM-enc      4 MB took 1.005 seconds,    4.300 MB/s
AES-256-GCM-dec      4 MB took 1.002 seconds,    4.166 MB/s
AES-CCM-Enc          4 MB took 1.005 seconds,    4.203 MB/s
AES-CCM-Dec          4 MB took 1.005 seconds,    4.057 MB/s
SHA                  7 MB took 1.000 seconds,    7.202 MB/s
SHA-224              7 MB took 1.001 seconds,    7.341 MB/s
SHA-256              7 MB took 1.000 seconds,    7.349 MB/s
HMAC-SHA             6 MB took 1.001 seconds,    6.390 MB/s
HMAC-SHA224          6 MB took 1.003 seconds,    6.475 MB/s
HMAC-SHA256          6 MB took 1.000 seconds,    6.470 MB/s
ECC      256 key gen       169 ops took 1.003 sec, avg 5.935 ms, 168.495 ops/sec
ECDHE    256 agree         184 ops took 1.003 sec, avg 5.451 ms, 183.450 ops/sec
ECDSA    256 sign          158 ops took 1.010 sec, avg 6.392 ms, 156.436 ops/sec
ECDSA    256 verify        148 ops took 1.001 sec, avg 6.764 ms, 147.852 ops/sec
```


# Support

Email us at [support@wolfssl.com](mailto:support@wolfssl.com).
