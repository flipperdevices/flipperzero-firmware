#pragma once

#include <furi_hal_spi_types.h>
#include <furi_hal_resources.h>

#define CAN0_PIN_IRQ &gpio_ext_pa4
#define CAN0_SPI_CS 2
#define CAN_TIMEOUT_SPI_INTERFACE  ( 1000 )

extern FuriHalSpiBusHandle mcp251xfd_1m_spi_bus_handle_external;
extern FuriHalSpiBusHandle mcp251xfd_spi_bus_handle_external;