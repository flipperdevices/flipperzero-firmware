# wolfCrypt Benchmarks

Tool for performing cryptographic algorithm benchmarking.

## Measurements

* Symmetric algorithms like AES and ChaCha20 are measured in Killobytes (KB) or Megabytes (MB) per second.
* Asymmetric algorithms like RSA and ECC are measured using Operations Per Second (Ops) per second.

## Compile Options

Compile with the following options for fixed units. Otherwise the units will auto-scale. See `-base10` parameter option, below.

`-DWOLFSSL_BENCHMARK_FIXED_UNITS_GB` for GB/GiB  
`-DWOLFSSL_BENCHMARK_FIXED_UNITS_MB` for MB/MiB  
`-DWOLFSSL_BENCHMARK_FIXED_UNITS_KB` for KB/KiB  
`-DWOLFSSL_BENCHMARK_FIXED_UNITS_B` for Bytes

To set the output to always be CSV:

`-DWOLFSSL_BENCHMARK_FIXED_CSV`

## Usage

```sh
./wolfcrypt/benchmark/benchmark -?
benchmark
-? <num>    Help, print this usage
            0: English, 1: Japanese
-csv        Print terminal output in csv format
-base10     Display bytes as power of 10 (eg 1 kB = 1000 Bytes)
-no_aad     No additional authentication data passed.
-dgst_full  Full digest operation performed.
-rsa_sign   Measure RSA sign/verify instead of encrypt/decrypt.
<keySz> -rsa-sz
            Measure RSA <key size> performance.
-<alg>      Algorithm to benchmark. Available algorithms include:
              cipher aes-cbc aes-gcm aes-ecb aes-xts aes-cfb aes-ctr aes-ccm
              camellia arc4 chacha20 chacha20-poly1305 des
              digest md5 poly1305 sha sha2 sha224 sha256 sha384 sha512 sha3
              sha3-224 sha3-256 sha3-384 sha3-512 ripemd
              mac cmac hmac hmac-md5 hmac-sha hmac-sha224 hmac-sha256
              hmac-sha384 hmac-sha512 pbkdf2
              asym rsa-kg rsa rsa-sz dh ecc-kg ecc ecc-enc curve25519_kg x25519
              ed25519-kg ed25519
              other rng scrypt
-lng <num>  Display benchmark result by specified language.
            0: English, 1: Japanese
<num>       Size of block in bytes
```

The `-base10` option shows as thousands of bytes (kB).

## Example Output

Run on Intel(R) Core(TM) i7-7920HQ CPU @ 3.10GHz.

```sh
./configure --enable-intelasm --enable-aesni --enable-sp --enable-sp-asm && make

./wolfcrypt/benchmark/benchmark
------------------------------------------------------------------------------
 wolfSSL version 4.0.0
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1048576, min 1.0 sec each)
RNG                180 MB took 1.022 seconds,  176.201 MB/s Cycles per byte =  16.76
AES-128-CBC-enc   1250 MB took 1.002 seconds, 1247.878 MB/s Cycles per byte =   2.37
AES-128-CBC-dec   4595 MB took 1.001 seconds, 4591.703 MB/s Cycles per byte =   0.64
AES-192-CBC-enc   1005 MB took 1.003 seconds, 1002.018 MB/s Cycles per byte =   2.95
AES-192-CBC-dec   4345 MB took 1.000 seconds, 4344.744 MB/s Cycles per byte =   0.68
AES-256-CBC-enc    905 MB took 1.005 seconds,  900.386 MB/s Cycles per byte =   3.28
AES-256-CBC-dec   3255 MB took 1.001 seconds, 3251.618 MB/s Cycles per byte =   0.91
AES-128-GCM-enc   4730 MB took 1.001 seconds, 4726.267 MB/s Cycles per byte =   0.62
AES-128-GCM-dec   5140 MB took 1.000 seconds, 5137.596 MB/s Cycles per byte =   0.57
AES-192-GCM-enc   4475 MB took 1.001 seconds, 4471.056 MB/s Cycles per byte =   0.66
AES-192-GCM-dec   3405 MB took 1.001 seconds, 3403.179 MB/s Cycles per byte =   0.87
AES-256-GCM-enc   2640 MB took 1.000 seconds, 2638.905 MB/s Cycles per byte =   1.12
AES-256-GCM-dec   2780 MB took 1.001 seconds, 2776.632 MB/s Cycles per byte =   1.06
CHACHA            2615 MB took 1.000 seconds, 2614.357 MB/s Cycles per byte =   1.13
CHA-POLY          1490 MB took 1.001 seconds, 1488.344 MB/s Cycles per byte =   1.98
MD5                440 MB took 1.010 seconds,  435.763 MB/s Cycles per byte =   6.78
POLY1305          4900 MB took 1.001 seconds, 4896.430 MB/s Cycles per byte =   0.60
SHA                515 MB took 1.011 seconds,  509.459 MB/s Cycles per byte =   5.80
SHA-224            425 MB took 1.005 seconds,  422.737 MB/s Cycles per byte =   6.98
SHA-256            420 MB took 1.006 seconds,  417.312 MB/s Cycles per byte =   7.08
SHA-384            615 MB took 1.003 seconds,  613.018 MB/s Cycles per byte =   4.82
SHA-512            560 MB took 1.007 seconds,  556.230 MB/s Cycles per byte =   5.31
SHA3-224           295 MB took 1.003 seconds,  294.133 MB/s Cycles per byte =  10.04
SHA3-256           280 MB took 1.003 seconds,  279.088 MB/s Cycles per byte =  10.58
SHA3-384           215 MB took 1.002 seconds,  214.654 MB/s Cycles per byte =  13.76
SHA3-512           145 MB took 1.005 seconds,  144.266 MB/s Cycles per byte =  20.47
HMAC-MD5           485 MB took 1.004 seconds,  483.019 MB/s Cycles per byte =   6.11
HMAC-SHA           505 MB took 1.006 seconds,  502.159 MB/s Cycles per byte =   5.88
HMAC-SHA224        415 MB took 1.007 seconds,  411.965 MB/s Cycles per byte =   7.17
HMAC-SHA256        380 MB took 1.002 seconds,  379.398 MB/s Cycles per byte =   7.78
HMAC-SHA384        610 MB took 1.006 seconds,  606.370 MB/s Cycles per byte =   4.87
HMAC-SHA512        620 MB took 1.001 seconds,  619.377 MB/s Cycles per byte =   4.77
RSA     2048 public      52000 ops took 1.001 sec, avg 0.019 ms, 51932.223 ops/sec
RSA     2048 private      1700 ops took 1.022 sec, avg 0.601 ms, 1662.697 ops/sec
DH      2048 key gen      3491 ops took 1.000 sec, avg 0.286 ms, 3490.745 ops/sec
DH      2048 agree        3500 ops took 1.014 sec, avg 0.290 ms, 3452.191 ops/sec
ECC      256 key gen     88961 ops took 1.000 sec, avg 0.011 ms, 88960.279 ops/sec
ECDHE    256 agree       20700 ops took 1.005 sec, avg 0.049 ms, 20605.239 ops/sec
ECDSA    256 sign        53200 ops took 1.001 sec, avg 0.019 ms, 53157.214 ops/sec
ECDSA    256 verify      17200 ops took 1.004 sec, avg 0.058 ms, 17124.208 ops/sec
Benchmark complete
```


## Windows Visual Studio

For building wolfCrypt Benchmark project in Visual Studio open the `benchmark.sln`. For newer Visual Studio version it may prompt for a one-way upgrade. If prompted to overwrite the `benchmark.vcxproj` project, select "No". Then you may have to right-click on the solution and choose `Retarget solution` to update the project files for your Visual Studio version.

If you see an error about `rc.exe` then you'll need to update the "Target Platform Version". You can do this by right-clicking on the benchmark project -> General -> "Target Platform Version" and changing to 8.1 (needs to match the wolfssl library project).

This solution includes the wolfSSL library project at `<wolfssl-root>wolfssl.vcxproj` and will compile the library, then the benchmark project.
