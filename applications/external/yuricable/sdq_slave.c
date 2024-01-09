#include <sdq_slave.h>
#include <yuricable_pro_max_structs.c>

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

uint8_t RESET_DEVICE[7] = {0x75, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t DFU[7] = {0x75, 0x20, 0x00, 0x02, 0x00, 0x00, 0x00};

const SDQTimings sdq_timings = { // microseconds
    .BREAK_meaningful_min = 12,
    .BREAK_meaningful_max = 16,
    .BREAK_meaningful = 14,
    .BREAK_recovery = 5,
    .WAKE_meaningful_min = 22,
    .WAKE_meaningful_max = 27,
    .WAKE_meaningful = 24,
    .WAKE_recovery = 1100,
    .ZERO_meaningful_min = 6,
    .ZERO_meaningful_max = 8,
    .ZERO_meaningful = 7,
    .ZERO_recovery = 4,
    .ONE_meaningful_min = 1,
    .ONE_meaningful_max = 3,
    .ONE_meaningful = 2,
    .ONE_recovery = 9,
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
    uint8_t command[4] = {0};
    if(sdq_slave_receive(bus, command, sizeof(command))) {
        switch(command[0]) {
        case TRISTAR_POLL:
            if(sdq_slave_wait_while_gpio_is(bus, bus->timings.BREAK_meaningful_max, false)) {
                if(sdq_slave_wait_while_gpio_is(bus, bus->timings.BREAK_recovery, true)) {
                    if(sdq_slave_send(bus, RESET, sizeof(RESET))) {
                        FURI_LOG_I("SDQ", "SENT COMMAND BYTES");
                    }
                }
            }
            break;
        case TRISTAR_UNKNOWN_76:
            FURI_LOG_I("SDQ", "TRISTAR_UNKNOWN_76");
            break;
        case TRISTAR_POWER:
            FURI_LOG_I("SDQ", "TRISTAR_POWER");
            break;
        default:
            break;
        }
        furi_assert(bus->command_callback);
        if(bus->command_callback(command, bus->command_callback_context)) {
            return true;
        }
    }
    return (bus->error == SDQSlaveErrorResetInProgress);
}

static inline bool sdq_slave_bus_start(SDQSlave* bus) {
    FURI_CRITICAL_ENTER();
    //const uint32_t start = DWT->CYCCNT;
    //furi_hal_gpio_init(bus->gpio_pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    //const uint32_t time_spent =
    //    (DWT->CYCCNT - start) / furi_hal_cortex_instructions_per_microsecond();
    //if(time_spent){
    //    return false;
    //}
    while(sdq_slave_receive_and_process_command(bus))
        ;
    const bool result = (bus->error == SDQSlaveErrorNone);
    //furi_hal_gpio_init(bus->gpio_pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
    FURI_CRITICAL_EXIT();
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
            if(sdq_slave_wait_while_gpio_is(sdq_slave, sdq_slave->timings.BREAK_recovery, true)) {
                sdq_slave_bus_start(sdq_slave);
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

uint8_t sdq_slave_receive_bit(SDQSlave* bus, bool isLastBitofByte) {
    const SDQTimings* timings = &bus->timings;
    // wait while bus is low for one meaningful
    if(sdq_slave_wait_while_gpio_is(bus, timings->ONE_meaningful_max, false)) {
        // wait while bus is high for one recovery
        if(isLastBitofByte) {
            if(sdq_slave_wait_while_gpio_is(bus, timings->ONE_STOP_recovery, true)) {
                bus->error = SDQSlaveErrorNone;
                return true;
            }
        } else {
            if(sdq_slave_wait_while_gpio_is(bus, timings->ONE_recovery, true)) {
                bus->error = SDQSlaveErrorNone;
                return true;
            }
        }
    }
    // wait while bus is low for zero meaningful
    if(sdq_slave_wait_while_gpio_is(
           bus, timings->ZERO_meaningful_max - timings->ONE_meaningful_max, false)) {
        // wait while bus is high for zero recovery
        if(isLastBitofByte) {
            if(sdq_slave_wait_while_gpio_is(bus, timings->ZERO_STOP_recovery, true)) {
                bus->error = SDQSlaveErrorNone;
                return false;
            }
        } else {
            if(sdq_slave_wait_while_gpio_is(bus, timings->ZERO_recovery, true)) {
                bus->error = SDQSlaveErrorNone;
                return false;
            }
        }
    }
    bus->error = SDQSlaveErrorInvalidCommand;
    return false;
}

static bool sdq_slave_send_byte(SDQSlave* bus, uint8_t byte) {
    const SDQTimings* timings = &bus->timings;
    for(uint8_t mask = 0x80; mask != 0; mask >>= 1) {
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

bool sdq_slave_send(SDQSlave* bus, const uint8_t data[], size_t data_size) {
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

bool sdq_slave_receive(SDQSlave* bus, uint8_t data[], size_t data_size) {
    size_t bytes_received = 0;
    for(; bytes_received < data_size; ++bytes_received) {
        uint8_t value = 0;
        for(uint8_t bit_mask = 0x01; bit_mask != 0; bit_mask <<= 1) {
            if(sdq_slave_receive_bit(bus, (bit_mask == 0x80)) && bus->error == SDQSlaveErrorNone) {
                value |= bit_mask;
            }
        }
        data[bytes_received] = value;
    }

    // Check CRC8
    uint8_t reduced_data[data_size - 1];
    for(size_t i = 0; i < data_size - 1; ++i) {
        reduced_data[i] = data[i];
    }
    uint8_t calculated_crc = crc_data(reduced_data, sizeof(reduced_data));
    if(data[data_size - 1] != calculated_crc) {
        bus->error = SDQSlaveErrorInvalidCommand;
        return false;
    }
    return true;
}
