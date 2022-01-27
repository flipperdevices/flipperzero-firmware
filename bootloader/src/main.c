#include "target.h"

int main() {
    // Initialize hardware
    target_init();
    // Check desired boot mode
    switch(target_get_boot_mode()) {
    case BootMode_DFU:
        target_switch2dfu();
        break;
    case BootMode_SDUpdate:
        target_switch2sdupdate();
        break;
    case BootMode_Normal:
    default:
        target_switch2os();
        break;
    }

    // Never should get here
    return 0;
}