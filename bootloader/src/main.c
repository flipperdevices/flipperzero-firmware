#include "platform.h"

int main() {
    platform_init();

    if (platform_is_dfu_requested()) {
        platform_switch2boot();
    }

    platform_switch2os();

    return 0;
}