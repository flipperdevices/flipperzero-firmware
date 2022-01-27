#include "sd_update.h"
#include <target.h>
#include <furi_hal.h>
#include <fatfs.h>

void execute_sdcard_update() {
    MX_FATFS_Init();
    delay(3000.f);
}
