#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <lib/crc/crc.c>
#include <furi_hal_gpio.h>
#include <furi_hal.h>
#include <lib/uart/usb_uart_bridge.c>

#ifdef __cplusplus
extern "C" {
#endif

#define RESPONSE_BUFFER_SIZE 8

enum TRISTAR_REQUESTS {
    TRISTAR_POWER = 0x70,
    TRISTAR_POWER_ANSWER = 0x72,
    TRISTAR_POLL = 0x74,
    TRISTAR_UNKNOWN_76 = 0x76,
    TRISTAR_SERVICEMODE_ANSWER = 0x7E
};

typedef struct {
    uint8_t DFU[7];
    uint8_t RESET_DEVICE[7];
    uint8_t USB_UART_JTAG[7];
    uint8_t USB_SPAM_JTAG[7];
    uint8_t USB_UART[7];
    uint8_t USB_A_CHARGING_CABLE[7];
    uint8_t POWER_ANSWER[2];
    uint8_t SN[7];
    uint8_t KEYSET[10];
    uint8_t UNKNOWN_76_ANSWER[12];
} TRISTART_RESPONSES;

typedef enum {
    SDQDeviceCommand_NONE = 0,
    SDQDeviceCommand_DCSD,
    SDQDeviceCommand_RESET,
    SDQDeviceCommand_DFU,
    SDQDeviceCommand_CHARGING,
    SDQDeviceCommand_SN,
    SDQDeviceCommand_JTAG,
    SDQDeviceCommand_RECOVERY,
} SDQDeviceCommand;

typedef struct {
    uint32_t BREAK_meaningful_min;
    uint32_t BREAK_meaningful_max;
    uint32_t BREAK_meaningful;
    uint32_t BREAK_recovery;
    uint32_t WAKE_meaningful_min;
    uint32_t WAKE_meaningful_max;
    uint32_t WAKE_meaningful;
    uint32_t WAKE_recovery;
    uint32_t ZERO_meaningful_min;
    uint32_t ZERO_meaningful_max;
    uint32_t ZERO_meaningful;
    uint32_t ZERO_recovery;
    uint32_t ONE_meaningful_min;
    uint32_t ONE_meaningful_max;
    uint32_t ONE_meaningful;
    uint32_t ONE_recovery;
    uint32_t ZERO_STOP_recovery;
    uint32_t ONE_STOP_recovery;
} SDQTimings;

typedef enum {
    SDQDeviceErrorNone = 0,
    SDQDeviceErrorNotConnected,
    SDQDeviceErrorInvalidCommand,
    SDQDeviceErrorBitReadTiming,
    SDQDeviceErrorTimeout,
    SDQDeviceErrorInvalidCRC,
} SDQDeviceError;

typedef struct SDQDevice SDQDevice;

struct SDQDevice {
    const GpioPin* gpio_pin;
    UsbUartBridge* uart_bridge;
    SDQTimings timings;
    SDQDeviceError error;
    SDQDeviceCommand runCommand;
    bool listening;
    bool connected;
    bool resetInProgress;
    bool commandExecuted;
};

struct SDQDevice* sdq_device_alloc(const GpioPin* gpio_pin, UsbUartBridge* uart_bridge);
void sdq_device_free(SDQDevice* bus);

void sdq_device_start(SDQDevice* bus);
void sdq_device_stop(SDQDevice* bus);

bool sdq_device_send(SDQDevice* bus, const uint8_t data[], size_t data_size);
bool sdq_device_receive(SDQDevice* bus, uint8_t data[], size_t data_size);

#ifdef __cplusplus
}
#endif
