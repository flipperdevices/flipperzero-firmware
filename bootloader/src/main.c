#include "platform.h"

int main() {
    // Initialize hardware
    platform_init();
    // Check if dfu requested
    if (platform_is_dfu_requested()) {
        platform_switch2dfu();
    }
    // Switch to OS
    platform_switch2os();
    // Never should get here
    return 0;
}