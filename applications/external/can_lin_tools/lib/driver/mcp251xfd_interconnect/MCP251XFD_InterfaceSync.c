//-----------------------------------------------------------------------------
#include "Conf_MCP251XFD.h"
#include "../mcp251xfd/MCP251XFD.h"
#include "MCP251XFD_InterfaceSync.h"

#include "mcp251xfd_spi.h"
#include <furi_hal_spi.h>
#include "CANEXTFunctions.h"

//-----------------------------------------------------------------------------

//=============================================================================
// MCP251XFD_Ext1 INT pin configuration for the Flipper Zero
//=============================================================================
eERRORRESULT MCP251XFD_IntPinInit(MCP251XFD* pComp) {
    if(pComp == NULL) return ERR__PARAMETER_ERROR;
    // INT from MCP251xFD board is linked to IRQ pin of EXT1 extension header (pin 9 as PA4)

    // Here it's always an input pin from the microcontroller side
    // ioport_set_pin_dir(EXT1_PIN_IRQ, IOPORT_DIR_INPUT);
    // ioport_set_pin_mode(EXT1_PIN_IRQ, IOPORT_MODE_PULLUP);
    // ioport_set_pin_sense_mode(EXT1_PIN_IRQ, IOPORT_SENSE_FALLING);
    furi_hal_gpio_init(
        MCP252XFD_PIN_IRQ_EXT1, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);
    // Enable interrupt
    // if (pio_handler_set_pin(EXT1_PIN_IRQ, PIO_IT_FALL_EDGE, &MCP251XFD_INT_Handler) != 0) return ERR__CONFIGURATION;
    // pio_enable_pin_interrupt(EXT1_PIN_IRQ);

    furi_hal_gpio_remove_int_callback(MCP252XFD_PIN_IRQ_EXT1);
    //furi_hal_gpio_add_int_callback(MCP252XFD_PIN_IRQ_EXT1, MCP251XFD_INT_Handler, NULL);

    return ERR_OK;
}

//=============================================================================
// MCP251XFD_Ext1 INT interrupt handler
//=============================================================================
void MCP251XFD_INT_Handler(uint32_t id, uint32_t mask) {
    UNUSED(id);
    UNUSED(mask);
    // OMG an interrupt occur !!!!
    asm("nop");
}

//**********************************************************************************
//=============================================================================
// MCP251XFD_Ext1 INT0 (GPIO0) pin configuration for the Flipper Zero
//=============================================================================
eERRORRESULT MCP251XFD_Int0Gpio0PinInit(MCP251XFD* pComp) {
    if(pComp == NULL) return ERR__PARAMETER_ERROR;

    if(pComp->UserDriverData == NULL) return ERR__PARAMETER_ERROR;

    MCP251XFD_Config* Conf = (MCP251XFD_Config*)(pComp->UserDriverData);
    switch(Conf->GPIO0PinMode) {
    case MCP251XFD_PIN_AS_GPIO0_IN: // As input on the device then as output on the microcontroller
    {
        // ioport_set_pin_dir(EXT1_PIN_13, IOPORT_DIR_OUTPUT);
        // ioport_set_pin_level(EXT1_PIN_13, IOPORT_PIN_LEVEL_LOW);

    } break;
    case MCP251XFD_PIN_AS_GPIO0_OUT: // As output on the device then as input on the microcontroller
    {
        // ioport_set_pin_dir(EXT1_PIN_13, IOPORT_DIR_INPUT);
        // ioport_set_pin_mode(EXT1_PIN_13, IOPORT_MODE_PULLUP);
        // ioport_set_pin_sense_mode(EXT1_PIN_13, IOPORT_SENSE_FALLING);
    } break;
    case MCP251XFD_PIN_AS_INT0_TX: // As interrupt on the device then as input with interruption on the microcontroller
    {
        // ioport_set_pin_dir(EXT1_PIN_13, IOPORT_DIR_INPUT);
        // ioport_set_pin_mode(EXT1_PIN_13, IOPORT_MODE_PULLUP);
        // ioport_set_pin_sense_mode(EXT1_PIN_13, IOPORT_SENSE_FALLING);
        // // Enable interrupt
        // if(pio_handler_set_pin(EXT1_PIN_13, PIO_IT_FALL_EDGE, &MCP251XFD_INT0_Handler) != 0)
        //     return ERR__CONFIGURATION;
        // pio_enable_pin_interrupt(EXT1_PIN_13);
    } break;
    case MCP251XFD_PIN_AS_XSTBY: {
    } break;
    }
    return ERR_OK;
}

//=============================================================================
// MCP251XFD_Ext1 INT0 interrupt handler
//=============================================================================
void MCP251XFD_INT0_Handler(uint32_t id, uint32_t mask) {
    // OMG an interrupt occur !!!!
    UNUSED(id);
    UNUSED(mask);
    asm("nop");
}

//**********************************************************************************
//=============================================================================
// MCP251XFD_Ext1 INT1 (GPIO1) pin configuration for the Flipper Zero
//=============================================================================
eERRORRESULT MCP251XFD_Int1Gpio1PinInit(MCP251XFD* pComp) {
    if(pComp == NULL) return ERR__PARAMETER_ERROR;
    if(pComp->UserDriverData == NULL) return ERR__PARAMETER_ERROR;

    MCP251XFD_Config* Conf = (MCP251XFD_Config*)(pComp->UserDriverData);
    switch(Conf->GPIO1PinMode) {
    case MCP251XFD_PIN_AS_GPIO1_IN: // As Input on the device then as output on the microcontroller
    {
        // ioport_set_pin_dir(EXT1_PIN_14, IOPORT_DIR_OUTPUT);
        // ioport_set_pin_level(EXT1_PIN_14, IOPORT_PIN_LEVEL_LOW);
    } break;
    case MCP251XFD_PIN_AS_GPIO1_OUT: // As output on the device then as input on the microcontroller
    {
        // ioport_set_pin_dir(EXT1_PIN_14, IOPORT_DIR_INPUT);
        // ioport_set_pin_mode(EXT1_PIN_14, IOPORT_MODE_PULLUP);
        // ioport_set_pin_sense_mode(EXT1_PIN_14, IOPORT_SENSE_FALLING);
    } break;
    case MCP251XFD_PIN_AS_INT1_RX: // As interrupt on the device then as input with interruption on the microcontroller
    {
        // ioport_set_pin_dir(EXT1_PIN_14, IOPORT_DIR_INPUT);
        // ioport_set_pin_mode(EXT1_PIN_14, IOPORT_MODE_PULLUP);
        // ioport_set_pin_sense_mode(EXT1_PIN_14, IOPORT_SENSE_FALLING);
        // // Enable interrupt
        // if(pio_handler_set_pin(EXT1_PIN_14, PIO_IT_FALL_EDGE, &MCP251XFD_INT1_Handler) != 0)
        //     return ERR__CONFIGURATION;
        // pio_enable_pin_interrupt(EXT1_PIN_14);
    } break;
    }
    return ERR_OK;
}

//=============================================================================
// MCP251XFD_Ext1 INT1 interrupt handler
//=============================================================================
void MCP251XFD_INT1_Handler(uint32_t id, uint32_t mask) {
    // OMG an interrupt occur !!!!
    UNUSED(id);
    UNUSED(mask);
    asm("nop");
}

//**********************************************************************************************************************************************************
//=============================================================================
// MCP251XFD SPI driver interface configuration for the Flipper Zero
//=============================================================================
eERRORRESULT MCP251XFD_InterfaceInit(void* pIntDev, uint8_t chipSelect, const uint32_t sckFreq) {
    if(pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;

    UNUSED(chipSelect);

    if(MCP251XFD_Ext2.SPIClockSpeed > sckFreq) {
       furi_hal_spi_bus_handle_deinit(pIntDev); 
       MCP251XFD_Ext2.InterfaceDevice =  &mcp251xfd_1m_spi_bus_handle_external;
       furi_hal_spi_bus_handle_init(MCP251XFD_Ext2.InterfaceDevice); 
    } else {
       furi_hal_spi_bus_handle_deinit(pIntDev); 
       MCP251XFD_Ext2.InterfaceDevice =  &mcp251xfd_spi_bus_handle_external;
       furi_hal_spi_bus_handle_init(MCP251XFD_Ext2.InterfaceDevice); 
    }

    return ERR_OK;
}

//=============================================================================
// MCP251XFD SPI transfer data for the Flipper Zero
//=============================================================================
eERRORRESULT MCP251XFD_InterfaceTransfer(
    void* pIntDev,
    uint8_t chipSelect,
    uint8_t* txData,
    uint8_t* rxData,
    size_t size) {
    if(pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;
    if(txData == NULL) return ERR__SPI_PARAMETER_ERROR;

    UNUSED(chipSelect);

    eERRORRESULT result = ERR_OK;

    furi_hal_spi_acquire(pIntDev);
    if(!furi_hal_spi_bus_trx(pIntDev, txData, rxData, size, MCP252XFD_TIMEOUT_SPI_INTERFACE))
        result = ERR__SPI_TIMEOUT;
    furi_hal_spi_release(pIntDev);

    return result;
}

//**********************************************************************************************************************************************************
//=============================================================================
// MCP251XFD_X get millisecond
//=============================================================================
uint32_t GetCurrentms(void) {
    return furi_get_tick();
}

//=============================================================================
// MCP251XFD_X compute CRC16-CMS
//=============================================================================
uint16_t ComputeCRC16(const uint8_t* data, size_t size) {
    return ComputeCRC16CMS(data, size);
}
