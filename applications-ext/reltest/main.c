#include "../../applications/elf-loader/elf-loader-sys-api.h"

int main(void) {
    syscalls.printf(">> Hello from external ELF application <<\r\n");
    return 0;
}