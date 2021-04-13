#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Boot modes */
typedef enum {
    ApiHalBootModeNormal,
    ApiHalBootModeDFU
} ApiHalBootMode;

/** Set boot mode */
void api_hal_boot_set_mode(ApiHalBootMode mode);

#if !NO_BOOTLOADER
/**
 * Get address of version structure of bootloader, stored in chip flash.
 *
 * @return Address of boot version structure.
 */
uint32_t api_hal_boot_version_address_get(void);
#endif


#ifdef __cplusplus
}
#endif
