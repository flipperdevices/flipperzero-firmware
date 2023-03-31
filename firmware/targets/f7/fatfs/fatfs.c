#include "fatfs.h"
#include "furi_hal_rtc.h"

/** logical drive path */
char fatfs_path[4];
/** File system object */
FATFS fatfs_object;

void fatfs_init(void) {
    FATFS_LinkDriver(&sd_fatfs_driver, fatfs_path);
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */

DWORD get_fattime() {
    FuriHalRtcDateTime furi_time;
    furi_hal_rtc_get_datetime(&furi_time);

    return ((uint32_t)(furi_time.year - 1980) << 25) | furi_time.month << 21 |
           furi_time.day << 16 | furi_time.hour << 11 | furi_time.minute << 5 | furi_time.second;
}
