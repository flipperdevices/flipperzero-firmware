Apache Mynewt Port
=============

## Overview

This port is for Apache Mynewt available [here](https://mynewt.apache.org/).

It provides follows mynewt packages.

- crypto/wolfssl
    - wolfssl library
- apps/wolfcrypttest
    - wolfcrypt unit test application

## How to setup

### delopy wolfssl source to mynewt project
Specify the path of the mynewt project and execute  `wolfssl/IDE/mynewt/setup.sh`.

```bash
./IDE/mynewt/setup.sh　/path/to/myproject_path
```

This script will deploy wolfssl's mynewt package described in the Overview to the mynewt project.

## Customization
### logging

To enable logging, please append `-DDEBUG_WOLFSSL` to `crypto.wolfssl.pkg.yml`(or `/path/to/myproject_path/crypto/wolfssl/pkg.yml`) in `pkg.cflags:` and inject dependency of mynewt log modules.

mynewt/crypto.wolfssl.pkg.yml
```yaml
pkg.req_apis:
    ...
    - log
    - stats
    - console
pkg.cflags: -DWOLFSSL_APACHE_MYNEWT ... -DDEBUG_WOLFSSL
```

Please add `wolfSSL_Debugging_ON()` to application code.

When it executes application, display tty device for output display devices.
please confirm with the `cat /dev/ttysXXX` command or `kermit` or `screen`.

## example setting example applications to targets

This section eplains how to set wolfssl example application to target device.

Please execute follows commands at only once.

create wolfcrypttest_sim that is simulator device for unit test of wolfcrypt.

```
cd /path/to/myproject_path
newt target create wolfcrypttest_sim
newt target set wolfcrypttest_sim app=apps/wolfcrypttest
newt target set wolfcrypttest_sim bsp=@apache-mynewt-core/hw/bsp/native
newt target set wolfcrypttest_sim build_profile=debug
```

## build & test

build and execute wolfcrypttest_sim

```
newt clean wolfcrypttest_sim
newt build wolfcrypttest_sim
./bin/targets/wolfcrypttest_sim/app/apps/wolfcrypttest/wolfcrypttest.elf
```

