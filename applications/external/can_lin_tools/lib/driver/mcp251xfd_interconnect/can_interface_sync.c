//-----------------------------------------------------------------------------
#include "Conf_MCP251XFD.h"
#include "../mcp251xfd/MCP251XFD.h"
#include "can_interface_sync.h"

#include "mcp251xfd_spi.h"
#include <furi_hal_spi.h>
#include "can0_function.h"
#include "mpc251xfd_user_driver_data.h"

//-----------------------------------------------------------------------------

//=============================================================================
// CAN INT pin configuration for the Flipper Zero
//=============================================================================
eERRORRESULT can_interface_sync_pin_init(MCP251XFD* pComp) {
    if(pComp == NULL) return ERR__PARAMETER_ERROR;
    if(pComp->UserDriverData == NULL) return ERR__PARAMETER_ERROR;
    MCP251XFD_UserDriverData* user_driver_data =
        (MCP251XFD_UserDriverData*)(pComp->UserDriverData);
    if(user_driver_data->can_irq != NULL) {
        furi_hal_gpio_init(
            user_driver_data->can_irq, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);
        furi_hal_gpio_remove_int_callback(user_driver_data->can_irq);
        //todo: add callback if enabled irq
        //furi_hal_gpio_add_int_callback(MCP252XFD_PIN_IRQ_EXT1, MCP251XFD_INT_Handler, NULL);
    }

    return ERR_OK;
}

// //=============================================================================
// // CAN INT interrupt handler
// //=============================================================================
// void MCP251XFD_INT_Handler(uint32_t id, uint32_t mask) {
//     UNUSED(id);
//     UNUSED(mask);
//     // OMG an interrupt occur !!!!
//     asm("nop");
// }

//**********************************************************************************
//=============================================================================
// CAN INT0 (GPIO0) pin configuration for the Flipper Zero
//=============================================================================
eERRORRESULT can_interface_sync_int0_gpio0_pin_init(MCP251XFD* pComp) {
    if(pComp == NULL) return ERR__PARAMETER_ERROR;
    if(pComp->UserDriverData == NULL) return ERR__PARAMETER_ERROR;

    MCP251XFD_UserDriverData* user_driver_data =
        (MCP251XFD_UserDriverData*)(pComp->UserDriverData);
    if(user_driver_data->can_gpio0 != NULL) {
        MCP251XFD_Config* Conf = (MCP251XFD_Config*)(user_driver_data->config);
        switch(Conf->GPIO0PinMode) {
        case MCP251XFD_PIN_AS_GPIO0_IN: // As Input on the device then as output on the microcontroller
        {
            furi_hal_gpio_init(
                user_driver_data->can_gpio0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
            furi_hal_gpio_write(user_driver_data->can_gpio0, false);
        } break;
        case MCP251XFD_PIN_AS_GPIO0_OUT: // As output on the device then as input on the microcontroller
        {
            furi_hal_gpio_init(
                user_driver_data->can_gpio0, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
        } break;
        case MCP251XFD_PIN_AS_INT0_TX: // As interrupt on the device then as input with interruption on the microcontroller
        {
            // Enable interrupt
            furi_hal_gpio_init(
                user_driver_data->can_gpio0,
                GpioModeInterruptRiseFall,
                GpioPullUp,
                GpioSpeedVeryHigh);
            furi_hal_gpio_remove_int_callback(user_driver_data->can_gpio0);
            //ToDo: add callback if enabled irq
            //furi_hal_gpio_add_int_callback(MCP252XFD_PIN_IRQ_EXT1, MCP251XFD_INT_Handler, NULL);
        } break;
        case MCP251XFD_PIN_AS_XSTBY: {
        } break;
        }
    }
    return ERR_OK;
}

// //=============================================================================
// // CAN INT0 interrupt handler
// //=============================================================================
// void MCP251XFD_INT0_Handler(uint32_t id, uint32_t mask) {
//     // OMG an interrupt occur !!!!
//     UNUSED(id);
//     UNUSED(mask);
//     asm("nop");
// }

//**********************************************************************************
//=============================================================================
// CAN INT1 (GPIO1) pin configuration for the Flipper Zero
//=============================================================================
eERRORRESULT can_interface_sync_int1_gpio1_pin_init(MCP251XFD* pComp) {
    if(pComp == NULL) return ERR__PARAMETER_ERROR;
    if(pComp->UserDriverData == NULL) return ERR__PARAMETER_ERROR;

    MCP251XFD_UserDriverData* user_driver_data =
        (MCP251XFD_UserDriverData*)(pComp->UserDriverData);
    if(user_driver_data->can_gpio1 != NULL) {
        MCP251XFD_Config* Conf = (MCP251XFD_Config*)(user_driver_data->config);
        switch(Conf->GPIO1PinMode) {
        case MCP251XFD_PIN_AS_GPIO1_IN: // As Input on the device then as output on the microcontroller
        {
            furi_hal_gpio_init(
                user_driver_data->can_gpio1, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
            furi_hal_gpio_write(user_driver_data->can_gpio1, false);
        } break;
        case MCP251XFD_PIN_AS_GPIO1_OUT: // As output on the device then as input on the microcontroller
        {
            furi_hal_gpio_init(
                user_driver_data->can_gpio1, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
        } break;
        case MCP251XFD_PIN_AS_INT1_RX: // As interrupt on the device then as input with interruption on the microcontroller
        {
            // Enable interrupt
            furi_hal_gpio_init(
                user_driver_data->can_gpio1,
                GpioModeInterruptRiseFall,
                GpioPullUp,
                GpioSpeedVeryHigh);
            furi_hal_gpio_remove_int_callback(user_driver_data->can_gpio1);
            //ToDo: add callback if enabled irq
            //furi_hal_gpio_add_int_callback(MCP252XFD_PIN_IRQ_EXT1, MCP251XFD_INT_Handler, NULL);
        } break;
        }
    }

    return ERR_OK;
}

// //=============================================================================
// // CAN INT1 interrupt handler
// //=============================================================================
// void MCP251XFD_INT1_Handler(uint32_t id, uint32_t mask) {
//     // OMG an interrupt occur !!!!
//     UNUSED(id);
//     UNUSED(mask);
//     asm("nop");
// }

//**********************************************************************************************************************************************************
//=============================================================================
// MCP251XFD SPI driver interface configuration for the Flipper Zero
//=============================================================================
eERRORRESULT can_interface_sync_init(void* pIntDev, uint8_t chipSelect, const uint32_t sckFreq) {
    if(pIntDev == NULL) return ERR__SPI_PARAMETER_ERROR;

    UNUSED(chipSelect);

    if(can0.SPIClockSpeed > sckFreq) {
        furi_hal_spi_bus_handle_deinit(pIntDev);
        can0.InterfaceDevice = &mcp251xfd_1m_spi_bus_handle_external;
        furi_hal_spi_bus_handle_init(can0.InterfaceDevice);
    } else {
        furi_hal_spi_bus_handle_deinit(pIntDev);
        can0.InterfaceDevice = &mcp251xfd_spi_bus_handle_external;
        furi_hal_spi_bus_handle_init(can0.InterfaceDevice);
    }

    return ERR_OK;
}

//=============================================================================
// MCP251XFD SPI transfer data for the Flipper Zero
//=============================================================================
eERRORRESULT can_interface_sync_transfer(
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
    if(!furi_hal_spi_bus_trx(pIntDev, txData, rxData, size, CAN_TIMEOUT_SPI_INTERFACE))
        result = ERR__SPI_TIMEOUT;
    furi_hal_spi_release(pIntDev);

    return result;
}

//=============================================================================
// MCP251XFD  deinit for the Flipper Zero
//=============================================================================
eERRORRESULT can_interface_sync_deinit(MCP251XFD* pComp) {
    if(pComp == NULL) return ERR__PARAMETER_ERROR;
    if(pComp->UserDriverData == NULL) return ERR__PARAMETER_ERROR;

    MCP251XFD_UserDriverData* user_driver_data =
        (MCP251XFD_UserDriverData*)(pComp->UserDriverData);
    MCP251XFD_Config* conf = (MCP251XFD_Config*)(user_driver_data->config);

    if(user_driver_data->can_irq != NULL) {
        furi_hal_gpio_remove_int_callback(user_driver_data->can_irq);
        furi_hal_gpio_init(user_driver_data->can_irq, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }

    if(user_driver_data->can_gpio0 != NULL) {
        if(conf->GPIO0PinMode == MCP251XFD_PIN_AS_INT0_TX) {
            furi_hal_gpio_remove_int_callback(user_driver_data->can_gpio0);
        }
        furi_hal_gpio_init(user_driver_data->can_gpio0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }

    if(user_driver_data->can_gpio1 != NULL) {
        if(conf->GPIO1PinMode == MCP251XFD_PIN_AS_INT1_RX) {
            furi_hal_gpio_remove_int_callback(user_driver_data->can_gpio1);
        }
        furi_hal_gpio_init(user_driver_data->can_gpio1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }

    furi_hal_spi_bus_handle_deinit(pComp->InterfaceDevice);

    return ERR_OK;
}

//**********************************************************************************************************************************************************
//=============================================================================
// MCP251XFD_X get millisecond
//=============================================================================
uint32_t can_interface_sync_get_currentms(void) {
    return furi_get_tick();
}

//=============================================================================
// MCP251XFD_X compute CRC16-CMS
//=============================================================================
uint16_t can_interface_sync_compute_crc16(const uint8_t* data, size_t size) {
    return ComputeCRC16CMS(data, size);
}
