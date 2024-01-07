#include <sdq_slave.h>

#define TIMEOUT_MAX 15000 /* Maximum time before general timeout */

struct SDQSlave {
    const GpioPin* gpio_pin;
    SDQTimings timings;
    SDQSlaveError error;
    SDQSlaveResetCallback reset_callback;
    SDQSlaveCommandCallback command_callback;
    SDQSlaveResultCallback result_callback;
    void* reset_callback_context;
    void* result_callback_context;
    void* command_callback_context;
};

const SDQTimings sdq_timings = { // microseconds
    .BREAK_meaningful_min = 12,
    .BREAK_meaningful_max = 16,
    .BREAK_meaningful = 14,
    .BREAK_recovery = 4,
    .WAKE_meaningful_min = 22,
    .WAKE_meaningful_max = 27,
    .WAKE_meaningful = 24,
    .WAKE_recovery = 1100,
    .ZERO_meaningful_min = 6,
    .ZERO_meaningful_max = 8,
    .ZERO_meaningful = 7,
    .ZERO_recovery = 3,
    .ONE_meaningful_min = 1,
    .ONE_meaningful_max = 3,
    .ONE_meaningful = 2,
    .ONE_recovery = 8,
    .ZERO_STOP_recovery = 16,
    .ONE_STOP_recovery = 21};

uint8_t crc_data(const uint8_t* data, size_t len) {
    crc_t crc = crc_init();
    crc = crc_update(crc, data, len);
    crc = crc_finalize(crc);
    return crc;
}

struct SDQSlave* sdq_slave_alloc(const GpioPin* gpio_pin) {
    struct SDQSlave* bus = malloc(sizeof(struct SDQSlave));
    bus->gpio_pin = gpio_pin;
    bus->timings = sdq_timings;
    bus->error = SDQSlaveErrorNone;
    return bus;
}

void sdq_slave_free(SDQSlave* bus) {
    sdq_slave_stop(bus);
    free(bus);
}

static bool sdq_slave_wait_while_gpio_is(SDQSlave* bus, uint32_t time_us, const bool pin_value) {
    const uint32_t time_start = DWT->CYCCNT;
    const uint32_t time_ticks = time_us * furi_hal_cortex_instructions_per_microsecond();
    uint32_t time_elapsed;
    do { //-V1044
        time_elapsed = DWT->CYCCNT - time_start;
        if(furi_hal_gpio_read(bus->gpio_pin) != pin_value) {
            return time_ticks >= time_elapsed;
        }
    } while(time_elapsed < time_ticks);
    return false;
}

static inline bool sdq_slave_receive_and_process_command(SDQSlave* bus) {
    if(bus->error == SDQSlaveErrorResetInProgress) {
        uint8_t command;
        if(sdq_slave_receive(bus, &command, sizeof(command))) {
            furi_assert(bus->command_callback);
            if(bus->command_callback(command, bus->command_callback_context)) {
                return true;
            }
        }
        return (bus->error == SDQSlaveErrorResetInProgress);
    }
    return false;
}

static inline bool sdq_slave_bus_start(SDQSlave* bus) {
    furi_hal_gpio_init(bus->gpio_pin, GpioModeOutputOpenDrain, GpioPullUp, GpioSpeedLow);
    while(sdq_slave_receive_and_process_command(bus))
        ;
    const bool result = (bus->error == SDQSlaveErrorNone);
    furi_hal_gpio_init(bus->gpio_pin, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedLow);
    return result;
}

static void sdq_slave_exti_callback(void* context) {
    SDQSlave* sdq_slave = context;
    const volatile bool input_state = furi_hal_gpio_read(sdq_slave->gpio_pin);
    static uint32_t pulse_start = 0;
    if(input_state) {
        const uint32_t pulse_length =
            (DWT->CYCCNT - pulse_start) / furi_hal_cortex_instructions_per_microsecond();
        if((pulse_length >= sdq_slave->timings.BREAK_meaningful_min) &&
           (pulse_length <= sdq_slave->timings.BREAK_meaningful_max)) {
            sdq_slave->error = SDQSlaveErrorResetInProgress;
            const bool result = sdq_slave_bus_start(sdq_slave);
            if(result && sdq_slave->result_callback != NULL) {
                sdq_slave->result_callback(sdq_slave->result_callback_context);
            }
        }
    } else {
        pulse_start = DWT->CYCCNT;
    }
}

void sdq_slave_start(SDQSlave* bus) {
    furi_hal_gpio_add_int_callback(bus->gpio_pin, sdq_slave_exti_callback, bus);
    furi_hal_gpio_write(bus->gpio_pin, true);
    furi_hal_gpio_init(bus->gpio_pin, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);
}

void sdq_slave_stop(SDQSlave* bus) {
    furi_hal_gpio_write(bus->gpio_pin, true);
    furi_hal_gpio_init(bus->gpio_pin, GpioModeAnalog, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_remove_int_callback(bus->gpio_pin);
}

void sdq_slave_set_reset_callback(SDQSlave* bus, SDQSlaveResetCallback callback, void* context) {
    bus->reset_callback = callback;
    bus->reset_callback_context = context;
}

void sdq_slave_set_command_callback(SDQSlave* bus, SDQSlaveCommandCallback callback, void* context) {
    bus->command_callback = callback;
    bus->command_callback_context = context;
}

void sdq_slave_set_result_callback(SDQSlave* bus, SDQSlaveResultCallback result_cb, void* context) {
    bus->result_callback = result_cb;
    bus->result_callback_context = context;
}

uint8_t sdq_slave_receive_bit(SDQSlave* bus) {
    const SDQTimings* timings = &bus->timings;
    return sdq_slave_wait_while_gpio_is(bus, timings->ONE_meaningful_max, false);
}

static bool sdq_slave_send_byte(SDQSlave* bus, uint8_t byte) {
    const SDQTimings* timings = &bus->timings;
    for(uint8_t mask = 0x80; mask != 0; mask >>= 1) {
        if(!sdq_slave_wait_while_gpio_is(bus, timings->BREAK_meaningful_max, false)) {
            bus->error = SDQSlaveErrorResetInProgress;
            return false;
        }
        if(!sdq_slave_wait_while_gpio_is(bus, timings->WAKE_meaningful_max, true)) {
            bus->error = SDQSlaveErrorTimeout;
            return false;
        }
        uint32_t meaningful_time = (mask & byte) ? timings->ONE_meaningful :
                                                   timings->ZERO_meaningful;
        uint32_t recovery_time = (mask & byte) ? timings->ONE_recovery : timings->ZERO_recovery;
        furi_hal_gpio_write(bus->gpio_pin, false);
        furi_delay_us(meaningful_time);
        furi_hal_gpio_write(bus->gpio_pin, true);
        // Add stop recovery time for the last bit in the byte
        if(mask == 0x01) {
            recovery_time = (mask & byte) ? timings->ONE_STOP_recovery :
                                            timings->ZERO_STOP_recovery;
        }
        furi_delay_us(recovery_time);
    }
    return true;
}

bool sdq_slave_send(SDQSlave* bus, const uint8_t* data, size_t data_size) {
    furi_hal_gpio_write(bus->gpio_pin, true);
    for(size_t i = 0; i < data_size; ++i) {
        if(!sdq_slave_send_byte(bus, data[i])) {
            return false;
        }
    }
    // Calculate and send CRC8
    uint8_t crc = crc_data(data, data_size); // Fix: use crc_data instead of crc8_calculate
    if(!sdq_slave_send_byte(bus, crc)) {
        return false;
    }
    return true;
}

bool sdq_slave_receive(SDQSlave* bus, uint8_t* data, size_t data_size) {
    furi_hal_gpio_write(bus->gpio_pin, true);
    size_t bytes_received = 0;
    for(; bytes_received < data_size; ++bytes_received) {
        uint8_t value = 0;

        for(uint8_t bit_mask = 0x01; bit_mask != 0; bit_mask <<= 1) {
            if(sdq_slave_receive_bit(bus)) {
                value |= bit_mask;
            }
        }

        data[bytes_received] = value;
    }
    // Receive CRC8
    uint8_t received_crc = data[data_size - 1];
    // Check CRC8
    uint8_t calculated_crc = crc_data(data, data_size);
    if(received_crc != calculated_crc) {
        bus->error = SDQSlaveErrorInvalidCommand;
        return false;
    }
    return true;
}
