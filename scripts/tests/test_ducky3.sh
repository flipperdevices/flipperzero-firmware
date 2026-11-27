#!/bin/sh
set -eu

cd "$(dirname "$0")/../.."
BUILD_DIR="${TMPDIR:-/tmp}/ducky3-test"
mkdir -p "$BUILD_DIR"

cc -std=gnu11 -Wall -Wextra -Werror \
    -Wno-error=implicit-function-declaration \
    -Wno-format -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast -Wno-overflow \
    '-D_ATTRIBUTE(x)=__attribute__(x)' \
    -DSTM32WB55xx -DUSE_FULL_LL_DRIVER \
    -include lib/stm32wb_hal/Inc/stm32wbxx_ll_spi.h \
    -I. -Ilib -Ilib/mjs -Ilib/mlib -Ilib/flipper_format \
    -Iapplications/services -Iapplications/main/bad_usb/helpers \
    -Ifuri -Ilib/cmsis_core -Itargets/f7/inc -Itargets/f7/furi_hal \
    -Itargets/furi_hal_include -Itargets/f7/ble_glue \
    -Ilib/libusb_stm32/inc -Ilib/stm32wb_hal/Inc -Ilib/stm32wb_cmsis/Include \
    scripts/tests/test_ducky3.c applications/main/bad_usb/helpers/ducky_script_3.c \
    -o "$BUILD_DIR/test_ducky3"

"$BUILD_DIR/test_ducky3"
