# Infineon AURIX Development Studio

An Eclipse based IDE for developing software for the Infineon TriCore AURIX TX3XX.

Tested Platform:
* Infineon AURIX™ Development Studio 1.7.2 (Build 20220617-0730)
* Infineon TriBoard TC399 v2.0
* wolfSSL v5.4.0 (with PR 5419)

## Running wolfCrypt on TriCore

1) Add the wolfSSL source and headers to `Libraries/wolfssl`. 
  - Only the following folders are required: `src`, `wolfcrypt` and `wolfssl`.
  - See script to help with producing bundle here: https://github.com/wolfSSL/wolfssl/blob/master/scripts/makedistsmall.sh
2) Add `WOLFSSL_USER_SETTINGS` to the Preprocessing symbols list. C/C++ Build -> Settings -> TASKING C/C++ Compiler -> Preprocessing.
3) Add `Libraries/wolfssl` to the include path. C/C++ General -> Paths and Symbols -> Includes -> GNU C
4) Add ignores for the following warnings. Unused static function (553) and switch missing break (536). C/C++ Build -> Settings -> TASKING C/C++ Compiler -> Diagnostics
5) Copy `Cpu0_Main.c`, `user_settings.h` and `wolf_main.c` into the project folder.
6) Increase the stack by modifying `Lcf_Tasking_Tricore_Tc.lsl` to adjusting the USTACK0-4 (`LCF_USTACK#_SIZE`) from 2k to 12k.
6) Build and run/debug.

### Example output from wolfCrypt test and benchmark

Benchmark Configuration:
* TriCore (TC1.6.2P) 32-bit super-scalar running at 300MHz:
* Release build: `-O2`
* SP Math SMALL: sp_c32.c for RSA/ECC/DH
* AES GCM SMALL

```
Running wolfCrypt Tests...
------------------------------------------------------------------------------
 wolfSSL version 5.4.0
------------------------------------------------------------------------------
error    test passed!
MEMORY   test passed!
base64   test passed!
asn      test passed!
RANDOM   test passed!
SHA      test passed!
SHA-256  test passed!
Hash     test passed!
HMAC-SHA test passed!
HMAC-SHA256 test passed!
HMAC-KDF    test passed!
TLSv1.3 KDF test passed!
GMAC     test passed!
Chacha   test passed!
POLY1305 test passed!
ChaCha20-Poly1305 AEAD test passed!
AES      test passed!
AES192   test passed!
AES256   test passed!
AES-GCM  test passed!
RSA      test passed!
ECC      test passed!
ECC buffer test passed!
CMAC     test passed!
logging  test passed!
time test passed!
mutex    test passed!
memcb    test passed!
Test complete
Crypt Test: Return code 0
Running wolfCrypt Benchmarks...
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                725 KB took 1.023 seconds,  708.703 KB/s
AES-128-CBC-enc      2 MB took 1.002 seconds,    2.071 MB/s
AES-128-CBC-dec      2 MB took 1.005 seconds,    2.065 MB/s
AES-192-CBC-enc      2 MB took 1.002 seconds,    1.779 MB/s
AES-192-CBC-dec      2 MB took 1.013 seconds,    1.783 MB/s
AES-256-CBC-enc      2 MB took 1.003 seconds,    1.558 MB/s
AES-256-CBC-dec      2 MB took 1.009 seconds,    1.573 MB/s
AES-128-GCM-enc    225 KB took 1.013 seconds,  222.112 KB/s
AES-128-GCM-dec    225 KB took 1.014 seconds,  221.892 KB/s
AES-192-GCM-enc    225 KB took 1.046 seconds,  215.107 KB/s
AES-192-GCM-dec    225 KB took 1.046 seconds,  215.104 KB/s
AES-256-GCM-enc    225 KB took 1.070 seconds,  210.279 KB/s
AES-256-GCM-dec    225 KB took 1.069 seconds,  210.477 KB/s
GMAC Small         251 KB took 1.000 seconds,  251.000 KB/s
AES-128-ECB-enc      2 MB took 1.000 seconds,    2.000 MB/s
AES-128-ECB-dec      2 MB took 1.000 seconds,    2.049 MB/s
AES-192-ECB-enc      2 MB took 1.000 seconds,    1.727 MB/s
AES-192-ECB-dec      2 MB took 1.000 seconds,    1.772 MB/s
AES-256-ECB-enc      2 MB took 1.000 seconds,    1.518 MB/s
AES-256-ECB-dec      2 MB took 1.000 seconds,    1.563 MB/s
CHACHA               3 MB took 1.007 seconds,    3.322 MB/s
CHA-POLY             2 MB took 1.011 seconds,    2.028 MB/s
POLY1305             6 MB took 1.003 seconds,    6.012 MB/s
SHA                  3 MB took 1.004 seconds,    3.380 MB/s
SHA-256              2 MB took 1.003 seconds,    1.558 MB/s
AES-128-CMAC         2 MB took 1.010 seconds,    2.055 MB/s
AES-256-CMAC         2 MB took 1.010 seconds,    1.547 MB/s
HMAC-SHA             3 MB took 1.004 seconds,    3.356 MB/s
HMAC-SHA256          2 MB took 1.010 seconds,    1.547 MB/s
RSA     2048 public         50 ops took 1.020 sec, avg 20.400 ms, 49.019 ops/sec
RSA     2048 private         2 ops took 2.377 sec, avg 1188.492 ms, 0.841 ops/sec
ECC   [      SECP256R1]   256 key gen        16 ops took 1.061 sec, avg 66.313 ms, 15.080 ops/sec
ECDHE [      SECP256R1]   256 agree          16 ops took 1.059 sec, avg 66.187 ms, 15.109 ops/sec
ECDSA [      SECP256R1]   256 sign           14 ops took 1.058 sec, avg 75.570 ms, 13.233 ops/sec
ECDSA [      SECP256R1]   256 verify          8 ops took 1.080 sec, avg 135.002 ms, 7.407 ops/sec
Benchmark complete
Benchmark Test: Return code 0
```


## Running wolfCrypt on the HSM (Cortex M3)

Coming soon


## Support

For questions please email facts@wolfssl.com
