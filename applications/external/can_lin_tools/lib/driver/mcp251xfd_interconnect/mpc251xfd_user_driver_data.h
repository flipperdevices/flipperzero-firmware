#include "../mcp251xfd/MCP251XFD.h"
#include "furi_hal_gpio.h"

typedef struct {
    MCP251XFD_Config* config;
    const GpioPin* can_irq;
    const GpioPin* can_gpio0;
    const GpioPin* can_gpio1;
} MCP251XFD_UserDriverData;
