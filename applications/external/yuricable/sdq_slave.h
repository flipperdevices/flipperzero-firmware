#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <crc.h>
#include <furi_hal_gpio.h>
#include <furi_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

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

extern const SDQTimings sdq_timings;
typedef enum {
    SDQSlaveErrorNone = 0,
    SDQSlaveErrorResetInProgress,
    SDQSlaveErrorPresenceConflict,
    SDQSlaveErrorInvalidCommand,
    SDQSlaveErrorTimeout,
} SDQSlaveError;

typedef struct SDQDevice SDQDevice;
typedef struct SDQSlave SDQSlave;

typedef bool (*SDQSlaveResetCallback)(bool is_short, void* context);
typedef bool (*SDQSlaveCommandCallback)(uint8_t command, void* context);
typedef void (*SDQSlaveResultCallback)(void* context);

struct SDQSlave* sdq_slave_alloc(const GpioPin* gpio_pin);
void sdq_slave_free(SDQSlave* bus);

void sdq_slave_start(SDQSlave* bus);
void sdq_slave_stop(SDQSlave* bus);

void sdq_slave_set_reset_callback(SDQSlave* bus, SDQSlaveResetCallback callback, void* context);
void sdq_slave_set_command_callback(SDQSlave* bus, SDQSlaveCommandCallback callback, void* context);
void sdq_slave_set_result_callback(SDQSlave* bus, SDQSlaveResultCallback result_cb, void* context);

bool sdq_slave_send(SDQSlave* bus, const uint8_t* data, size_t data_size);
bool sdq_slave_receive(SDQSlave* bus, uint8_t* data, size_t data_size);

#ifdef __cplusplus
}
#endif
