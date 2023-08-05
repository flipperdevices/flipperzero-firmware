#pragma once

#include <furi_hal_spi_types.h>
#include <furi_hal_resources.h>

#define MCP252XFD_PIN_IRQ_EXT1 &gpio_ext_pa4
#define MCP252XFD_SPI_CS_EXT1 2 //&gpio_ext_pb2
#define MCP252XFD_TIMEOUT_SPI_INTERFACE  ( 1000 )

extern FuriHalSpiBusHandle mcp251xfd_1m_spi_bus_handle_external;
extern FuriHalSpiBusHandle mcp251xfd_spi_bus_handle_external;