
# Serial flasher Tests

##Overview

Two kinds of tests are written for serial flasher:

* Host tests 
* Qemu tests

Qemu tests uses emulated esp32 to test correctness of the library. 

## Installation (Only for qemu tests)

Please refer to [building qemu](https://github.com/espressif/qemu) for instructions how to compile.

## Build and run

### Qemu test

QEMU_PATH environment variable pointing to compiled `qemu/build/xtensa-softmmu/qemu-system-xtensa` has to be defined.
```
export QEMU_PATH=path_to_qemu-system-xtensa
./run_test.sh qemu
```

### Host test
```
./run_test.sh host
```