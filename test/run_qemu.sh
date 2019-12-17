
# QEMU_PATH environment variable has to be defined, pointing to qemu-system-xtensa
# Example: export QEMU_PATH=/home/user/esp/qemu/xtensa-softmmu/qemu-system-xtensa
if [ -z "${QEMU_PATH}" ]; then
    echo "QEMU_PATH environment variable needs to be set"
    exit 1
fi

# Restore content of hello-world.bin image before passing to qemu,
# as it was modified by tests, flashing new application.
# cp ../images/generated_hello-world.bin ../images/hello-world.bin

# generate empty file into which application will be flashed and compared against.
dd if=/dev/zero bs=1024 count=4096 of="empty_file.bin"

${QEMU_PATH} \
    -nographic \
    -machine esp32 \
    -drive file=empty_file.bin,if=mtd,format=raw \
    -global driver=esp32.gpio,property=strap_mode,value=0x0f \
    -serial tcp::5555,server,nowait