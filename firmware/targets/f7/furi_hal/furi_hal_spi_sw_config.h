#pragma once

#include <furi_hal_spi_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Software SPI bus */
extern FuriHalSpiBus furi_hal_spi_sw_bus;

/** External CC1101 on software spi bus */
extern FuriHalSpiBusHandle furi_hal_spi_sw_bus_handle_subghz_ext;

#ifdef __cplusplus
}
#endif