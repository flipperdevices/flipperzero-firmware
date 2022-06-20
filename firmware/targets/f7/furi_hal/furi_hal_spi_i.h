#pragma once

#include <stm32wbxx_ll_spi.h>

struct FuriHalSpiBus {
    SPI_TypeDef* spi;
    FuriHalSpiBusEventCallback callback;
    FuriHalSpiBusHandle* current_handle;
};
