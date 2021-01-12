#include "api-hal-spi.h"
#include "api-hal-spi-config.h"
#include "api-basic\check.h"

osMutexId_t spi_mutex_r;
osMutexId_t spi_mutex_d;

extern SPI_HandleTypeDef SPI_R;
extern SPI_HandleTypeDef SPI_D;
extern void Enable_SPI(SPI_HandleTypeDef* spi);

void api_hal_spi_init() {
    spi_mutex_r = osMutexNew(NULL);
    spi_mutex_d = osMutexNew(NULL);
}

void api_hal_spi_lock(SPI_HandleTypeDef* spi) {
    if(spi == &SPI_D) {
        osMutexAcquire(spi_mutex_d, osWaitForever);
    } else if(spi == &SPI_R) {
        osMutexAcquire(spi_mutex_r, osWaitForever);
    } else {
        Error_Handler();
    }
}

void api_hal_spi_unlock(SPI_HandleTypeDef* spi) {
    if(spi == &SPI_D) {
        osMutexRelease(spi_mutex_d);
    } else if(spi == &SPI_R) {
        osMutexRelease(spi_mutex_r);
    } else {
        Error_Handler();
    }
}

void api_hal_spi_apply_config(const SPI_InitTypeDef* config, SPI_HandleTypeDef* spi_instance) {
    osKernelLock();

    spi_instance->Init.Mode = config->Mode;
    spi_instance->Init.Direction = config->Direction;
    spi_instance->Init.DataSize = config->DataSize;
    spi_instance->Init.CLKPolarity = config->CLKPolarity;
    spi_instance->Init.CLKPhase = config->CLKPhase;
    spi_instance->Init.NSS = config->NSS;
    spi_instance->Init.BaudRatePrescaler = config->BaudRatePrescaler;
    spi_instance->Init.FirstBit = config->FirstBit;
    spi_instance->Init.TIMode = config->TIMode;
    spi_instance->Init.CRCCalculation = config->CRCCalculation;
    spi_instance->Init.CRCPolynomial = config->CRCPolynomial;
    spi_instance->Init.CRCLength = config->CRCLength;
    spi_instance->Init.NSSPMode = config->NSSPMode;

    if(HAL_SPI_Init(spi_instance) != HAL_OK) {
        Error_Handler();
    }

    Enable_SPI(spi_instance);

    osKernelUnlock();
}

bool api_hal_spi_config_are_same(const SPI_InitTypeDef* config_a, const SPI_InitTypeDef* config_b) {
    return (memcmp(config_a, config_b, sizeof(SPI_InitTypeDef)) == 0);
}

void api_hal_spi_config_device(SpiDevice device) {
    // get handle
    SPI_HandleTypeDef* spi_instance;
    const SPI_InitTypeDef* config;

    // get bus
    switch(device) {
    case(SPI_SD_CARD):
    case(SPI_SD_CARD_INIT):
    case(SPI_DISPLAY):
        spi_instance = &SPI_D;
        break;
    default:
        Error_Handler();
    }

    if(spi_instance == &SPI_D) {
        // get applicable config
        switch(device) {
        case(SPI_SD_CARD):
            config = &sd_fast_spi;
            break;
        case(SPI_SD_CARD_INIT):
            config = &sd_slow_spi;
            break;
        case(SPI_DISPLAY):
            config = &sd_display_spi;
            break;
        default:
            Error_Handler();
        }
    }

    if(!api_hal_spi_config_are_same(config, &spi_instance->Init)) {
        api_hal_spi_apply_config(config, spi_instance);
    }
}