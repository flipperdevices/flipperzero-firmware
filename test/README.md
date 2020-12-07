
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

Before running qemu tests, qemu itself has to be launched first. 

QEMU_PATH environment variable pointing to qemu-system-xtensa has to be defined.
```
export QEMU_PATH=path_to_qemu-system-xtensa
sh run_qemu.sh
```

Run qemu test
```
cmake .. -DQEMU_TEST=True && cmake --build . && ./serial_flasher_test
```

### Host test

Run host test
```
cmake .. -DQEMU_TEST=False && cmake --build . && ./serial_flasher_test
```