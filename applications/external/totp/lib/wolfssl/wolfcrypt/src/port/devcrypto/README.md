# Description

Used to build with cryptodev-linux library with Linux OS.

# Quick Start

## Installing cryptodev module

If not already installed then the cryptodev-linux module will need installed.

```
git clone https://github.com/cryptodev-linux/cryptodev-linux.git
cd cryptodev-linux
make
sudo make install
modprobe cryptodev
```


## Options for building wolfSSL

For default build with all supported features use:

```
./configure --enable-devcrypto
```

Or for more control over features used:

```
./configure --enable-devcrypto=cbc
./configure --enable-devcrypto=hash
./configure --enable-devcrypto=aes
./configure --enable-devcrypto=all
```

Then build the wolfSSL library with:

```
make
sudo make install
./wolfcrypt/test/testwolfcrypt
```
