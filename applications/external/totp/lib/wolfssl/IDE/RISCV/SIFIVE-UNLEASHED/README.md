# SiFive HiFive Unleashed

Instructions for cross-compiling and running wolfSSL on the HiFive Unleashed board.

## Board SiFive HiFive Unleashed Board

SiFive Freedom U540 SoC at 1.5GHz

Getting started guide: 
https://sifive.cdn.prismic.io/sifive%2Ffa3a584a-a02f-4fda-b758-a2def05f49f9_hifive-unleashed-getting-started-guide-v1p1.pdf

Make sure your ethernet is attached and power up board. You can connecct the micro-usb to get a UART console that will display the DHCP IP address. Default user is "root" and login password is "sifive".

## Building Freedom-U-SDK

```sh
git clone https://github.com/sifive/freedom-u-sdk.git
cd freedom-u-sdk
git submodule update --recursive --init
make
```

See `freedom-u-sdk/README.md` file for instructions on updating the SD card U-Boot and Linux image.

### Installing U-Boot

Insert SD card from Unleashed into host and determine the assigned `/dev/sdX` for the media.

From `freedom-u-sdk` directory:

```sh
sudo make DISK=/dev/sdX format-boot-loader

# Copy U-Boot .fit image to first FAT partition (32MB)
sudo mkdir /media/hifiveu_boot
sudo mount -t vfat /dev/sdX1 /media/hifiveu_boot
cp ./work/image-<GITID>.fit /media/hifiveu_boot/hifiveu.fit
sudo umount /media/hifiveu_boot

# Copy Linux FS
sudo dd if=./work/hifive-unleashed-<ID>.gpt of=/dev/sdX2 bs=1M
```

Note: Make sure S1 Switch 5 (MSEL2) is OFF, rest ON (MSEL=1011) to boot from SD

### Installing Debian

Insert SD card from Unleashed into host and determine the assigned character sequence (X) for the media.

From `freedom-u-sdk` directory:

```sh
sudo make DISK=/dev/sdX format-demo-image
```

## Building wolfSSL

Make sure you are using wolfSSL sources based on this PR https://github.com/wolfSSL/wolfssl/pull/2456
The PR 2456 includes a patch to `wolfssl/wolfcrypt/types.h` to detect 64-bit types based on the `__riscv_xlen` macro.

### Cross Compiling

This example assumes the `wolfssl` root directory is along side the `freedom-u-sdk` directory. If not then adjust paths.

```
~\
	wolfssl
	freedom-u-sdk
```

```sh
./configure --host=riscv64 \
	CC="`pwd`/../freedom-u-sdk/work/buildroot_initramfs/host/bin/riscv64-sifive-linux-gnu-gcc" \
	--with-sysroot="`pwd`/../freedom-u-sdk/work/buildroot_initramfs_sysroot/" \
	--disable-shared \
	--enable-sp \
	CFLAGS="-mabi=lp64d -march=rv64imafdc"
make
```

Copy files to device (replace IP address):

```sh
scp ./wolfcrypt/test/testwolfcrypt root@192.168.0.144:~
scp ./wolfcrypt/benchmark/benchmark root@192.168.0.144:~
scp ./examples/client/client root@192.168.0.144:~
scp ./examples/server/server root@192.168.0.144:~

# manually `mkdir certs` on target
scp ./certs/* root@192.168.0.144:~/certs
```

### Native Compiler

```sh
./configure --enable-sp
make
```

## Benchmark Results

The following is running the wolfCrypt benchmark at 1.4GHz on a single thread (default CPU speed is 1.0GHz).

```sh
echo 1400000000 > /sys/devices/platform/soc/10000000.prci/rate

./benchmark
------------------------------------------------------------------------------
 wolfSSL version 4.1.0
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1048576, min 1.0 sec each)
RNG                 10 MB took 1.165 seconds,    8.585 MB/s
AES-128-CBC-enc     15 MB took 1.346 seconds,   11.141 MB/s
AES-128-CBC-dec     15 MB took 1.380 seconds,   10.867 MB/s
AES-192-CBC-enc     10 MB took 1.002 seconds,    9.983 MB/s
AES-192-CBC-dec     10 MB took 1.020 seconds,    9.805 MB/s
AES-256-CBC-enc     10 MB took 1.100 seconds,    9.091 MB/s
AES-256-CBC-dec     10 MB took 1.117 seconds,    8.952 MB/s
AES-128-GCM-enc     10 MB took 1.809 seconds,    5.528 MB/s
AES-128-GCM-dec     10 MB took 1.810 seconds,    5.524 MB/s
AES-192-GCM-enc     10 MB took 1.911 seconds,    5.233 MB/s
AES-192-GCM-dec     10 MB took 1.911 seconds,    5.232 MB/s
AES-256-GCM-enc      5 MB took 1.013 seconds,    4.935 MB/s
AES-256-GCM-dec      5 MB took 1.014 seconds,    4.933 MB/s
CHACHA              25 MB took 1.181 seconds,   21.168 MB/s
CHA-POLY            20 MB took 1.188 seconds,   16.833 MB/s
MD5                 80 MB took 1.025 seconds,   78.066 MB/s
POLY1305            85 MB took 1.032 seconds,   82.357 MB/s
SHA                 40 MB took 1.033 seconds,   38.728 MB/s
SHA-256             20 MB took 1.023 seconds,   19.557 MB/s
SHA-384             25 MB took 1.059 seconds,   23.597 MB/s
SHA-512             25 MB took 1.059 seconds,   23.597 MB/s
HMAC-MD5            80 MB took 1.026 seconds,   77.950 MB/s
HMAC-SHA            40 MB took 1.034 seconds,   38.700 MB/s
HMAC-SHA256         20 MB took 1.023 seconds,   19.559 MB/s
HMAC-SHA384         25 MB took 1.059 seconds,   23.598 MB/s
HMAC-SHA512         25 MB took 1.059 seconds,   23.599 MB/s
RSA     2048 public       2000 ops took 1.032 sec, avg 0.516 ms, 1938.304 ops/sec
RSA     2048 private       100 ops took 1.713 sec, avg 17.132 ms, 58.370 ops/sec
DH      2048 key gen       133 ops took 1.003 sec, avg 7.544 ms, 132.552 ops/sec
DH      2048 agree         200 ops took 1.531 sec, avg 7.653 ms, 130.676 ops/sec
ECC      256 key gen      1330 ops took 1.001 sec, avg 0.752 ms, 1329.260 ops/sec
ECDHE    256 agree         400 ops took 1.243 sec, avg 3.107 ms, 321.830 ops/sec
ECDSA    256 sign         1000 ops took 1.043 sec, avg 1.043 ms, 958.539 ops/sec
ECDSA    256 verify        300 ops took 1.104 sec, avg 3.680 ms, 271.766 ops/sec
Benchmark complete
```

## TLS Benchmarks

```
echo 1400000000 > /sys/devices/platform/soc/10000000.prci/rate
./examples/benchmark/tls_bench -p 8192 -m
Side    Cipher                             Total Bytes  Num Conns      Rx ms      Tx ms    Rx MB/s    Tx MB/s   Connect Total ms   Connect Avg ms
Server  DHE-RSA-AES128-SHA                      557056         18     94.722     34.548      2.804      7.689            915.005           50.834
Client  DHE-RSA-AES128-SHA                      557056         18    113.339     34.464      2.344      7.707            896.122           49.785
Server  DHE-RSA-AES256-SHA                      524288         17    102.691     37.624      2.434      6.645            866.921           50.995
Client  DHE-RSA-AES256-SHA                      524288         17    123.016     37.391      2.032      6.686            846.925           49.819
Server  ECDHE-RSA-AES128-SHA                    851968         27    144.719     52.871      2.807      7.684            828.128           30.671
Client  ECDHE-RSA-AES128-SHA                    851968         27    173.414     52.593      2.343      7.724            799.406           29.608
Server  ECDHE-ECDSA-AES128-SHA                 1245184         39    210.728     75.683      2.818      7.845            702.403           18.010
Client  ECDHE-ECDSA-AES128-SHA                 1245184         39    251.039     76.824      2.365      7.729            660.166           16.927
Server  ECDHE-ECDSA-AES256-SHA                 1179648         37    232.303     85.585      2.421      6.572            673.207           18.195
Client  ECDHE-ECDSA-AES256-SHA                 1179648         37    277.830     85.551      2.025      6.575            626.807           16.941
Server  DHE-RSA-AES128-SHA256                   524288         17    106.848     39.533      2.340      6.324            867.397           51.023
Client  DHE-RSA-AES128-SHA256                   524288         17    127.601     39.334      1.959      6.356            846.556           49.797
Server  DHE-RSA-AES256-SHA256                   524288         17    120.685     45.184      2.072      5.533            870.931           51.231
Client  DHE-RSA-AES256-SHA256                   524288         17    144.842     44.807      1.726      5.579            847.038           49.826
Server  DHE-RSA-AES128-GCM-SHA256               524288         17    124.636     49.373      2.006      5.064            869.243           51.132
Client  DHE-RSA-AES128-GCM-SHA256               524288         17    148.326     49.277      1.685      5.073            845.442           49.732
Server  DHE-RSA-AES256-GCM-SHA384               491520         16    129.714     51.389      1.807      4.561            822.309           51.394
Client  DHE-RSA-AES256-GCM-SHA384               491520         16    154.349     51.488      1.518      4.552            797.458           49.841
Server  ECDHE-RSA-AES128-GCM-SHA256             753664         24    179.251     71.130      2.005      5.052            744.773           31.032
Client  ECDHE-RSA-AES128-GCM-SHA256             753664         24    213.410     71.098      1.684      5.055            710.151           29.590
Server  ECDHE-RSA-AES256-GCM-SHA384             753664         24    198.233     78.075      1.813      4.603            751.299           31.304
Client  ECDHE-RSA-AES256-GCM-SHA384             753664         24    235.646     78.914      1.525      4.554            712.908           29.705
Server  ECDHE-ECDSA-AES128-GCM-SHA256          1114112         35    263.646    103.830      2.015      5.117            641.739           18.335
Client  ECDHE-ECDSA-AES128-GCM-SHA256          1114112         35    312.647    105.155      1.699      5.052            590.693           16.877
Server  ECDHE-ECDSA-AES256-GCM-SHA384          1048576         33    275.753    117.824      1.813      4.244            615.062           18.638
Client  ECDHE-ECDSA-AES256-GCM-SHA384          1048576         33    336.538    109.886      1.486      4.550            561.107           17.003
Server  ECDHE-RSA-AES128-SHA256                 786432         25    159.515     58.314      2.351      6.431            777.706           31.108
Client  ECDHE-RSA-AES128-SHA256                 786432         25    190.152     58.939      1.972      6.362            746.025           29.841
Server  ECDHE-ECDSA-AES128-SHA256              1179648         37    239.776     87.656      2.346      6.417            675.020           18.244
Client  ECDHE-ECDSA-AES128-SHA256              1179648         37    285.535     88.530      1.970      6.354            626.898           16.943
Server  ECDHE-RSA-AES256-SHA384                 786432         25    173.038     63.549      2.167      5.901            780.063           31.203
Client  ECDHE-RSA-AES256-SHA384                 786432         25    206.355     63.950      1.817      5.864            745.912           29.836
Server  ECDHE-ECDSA-AES256-SHA384              1146880         36    252.686     94.012      2.164      5.817            666.287           18.508
Client  ECDHE-ECDSA-AES256-SHA384              1146880         36    302.699     93.089      1.807      5.875            616.081           17.113
Server  ECDHE-RSA-CHACHA20-POLY1305             983040         31     67.015     25.344      6.995     18.496            929.187           29.974
Client  ECDHE-RSA-CHACHA20-POLY1305             983040         31     79.041     25.765      5.930     18.193            916.451           29.563
Server  ECDHE-ECDSA-CHACHA20-POLY1305          1572864         49    110.132     40.284      6.810     18.618            848.004           17.306
Client  ECDHE-ECDSA-CHACHA20-POLY1305          1572864         49    131.014     41.404      5.725     18.114            824.766           16.832
Server  DHE-RSA-CHACHA20-POLY1305               589824         19     40.016     15.086      7.028     18.643            951.114           50.059
Client  DHE-RSA-CHACHA20-POLY1305               589824         19     46.994     15.467      5.985     18.184            943.807           49.674
Server  ECDHE-RSA-CHACHA20-POLY1305-OLD         983040         31     66.802     25.246      7.017     18.567            928.078           29.938
Client  ECDHE-RSA-CHACHA20-POLY1305-OLD         983040         31     78.402     25.915      5.979     18.088            915.354           29.528
Server  ECDHE-ECDSA-CHACHA20-POLY1305-OLD      1572864         49    106.853     40.000      7.019     18.750            844.887           17.243
Client  ECDHE-ECDSA-CHACHA20-POLY1305-OLD      1572864         49    124.956     41.720      6.002     17.977            824.284           16.822
Server  DHE-RSA-CHACHA20-POLY1305-OLD           589824         19     40.200     14.991      6.996     18.762            951.477           50.078
Client  DHE-RSA-CHACHA20-POLY1305-OLD           589824         19     46.852     15.688      6.003     17.928            944.071           49.688
```

## Support

For questions please email us at support@wolfssl.com.
