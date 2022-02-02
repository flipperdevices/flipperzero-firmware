#include "target.h"
#include "sd_update.h"

int main() {
    // Initialize hardware
    target_init();
    // Check desired boot mode
    switch(target_get_boot_mode()) {
    case BootMode_DFU:
        target_switch2dfu();
        break;
    case BootMode_SDUpdate:
        sdcard_update_execute();
    case BootMode_Normal:
    default:
        target_switch2os();
        break;
    }

    // Never should get here
    return 0;
}