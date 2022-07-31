#pragma once

typedef enum {
    GpioStartEventOtgOff = 0,
    GpioStartEventOtgOn,
    GpioStartEventManualControl,
    GpioStartEventUsbUart,
    GpioStartEventI2CScanner,

    GpioCustomEventErrorBack,

    GpioUsbUartEventConfig,
} GpioCustomEvent;
