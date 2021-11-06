#include "one_wire_slave.h"
#include "one_wire_slave_i.h"
#include "one_wire_device.h"
#include <furi.h>
#include <furi-hal-delay.h>

#define OWS_RESET_MIN 270
#define OWS_RESET_MAX 960
#define OWS_PRESENCE_TIMEOUT 20
#define OWS_PRESENCE_MIN 100
#define OWS_PRESENCE_MAX 480
#define OWS_MSG_HIGH_TIMEOUT 15000
#define OWS_SLOT_MAX 135
#define OWS_READ_MIN 20
#define OWS_READ_MAX 60
#define OWS_WRITE_ZERO 30

typedef enum {
    NO_ERROR = 0,
    READ_TIMESLOT_TIMEOUT,
    WRITE_TIMESLOT_TIMEOUT,
    WAIT_RESET_TIMEOUT,
    VERY_LONG_RESET,
    VERY_SHORT_RESET,
    PRESENCE_LOW_ON_LINE,
    READ_TIMESLOT_TIMEOUT_LOW,
    AWAIT_TIMESLOT_TIMEOUT_HIGH,
    PRESENCE_HIGH_ON_LINE,
    INCORRECT_ONEWIRE_CMD,
    INCORRECT_SLAVE_USAGE,
    TRIED_INCORRECT_WRITE,
    FIRST_TIMESLOT_TIMEOUT,
    FIRST_BIT_OF_BYTE_TIMEOUT,
    RESET_IN_PROGRESS
} OneWireSlaveError;

struct OneWireSlave {
    OneWireSlaveError error;
    const GpioPin* pin;
    OneWireDevice* device;
    OneWireSlaveResultCallback result_cb;
    void* result_cb_ctx;
};

/*********************** PRIVATE ***********************/
void onewire_slave_pin_set_float(OneWireSlave* bus) {
    hal_gpio_write(bus->pin, true);
}

void onewire_slave_pin_set_low(OneWireSlave* bus) {
    hal_gpio_write(bus->pin, false);
}

void onewire_slave_pin_init_interrupt_in_isr_ctx(OneWireSlave* bus) {
    hal_gpio_init(bus->pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
    __HAL_GPIO_EXTI_CLEAR_IT(bus->pin->pin);
}

void onewire_slave_pin_init_opendrain_in_isr_ctx(OneWireSlave* bus) {
    hal_gpio_init(bus->pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    __HAL_GPIO_EXTI_CLEAR_IT(bus->pin->pin);
}

uint32_t onewire_slave_wait_while_gpio_is(OneWireSlave* bus, uint32_t time, const bool pin_value) {
    uint32_t start = DWT->CYCCNT;
    uint32_t time_ticks = time * instructions_per_us;
    uint32_t time_captured;

    do {
        time_captured = DWT->CYCCNT;
        if(hal_gpio_read(bus->pin) != pin_value) {
            uint32_t remaining_time = time_ticks - (time_captured - start);
            remaining_time /= instructions_per_us;
            return remaining_time;
        }
    } while((time_captured - start) < time_ticks);

    return 0;
}

bool onewire_slave_show_presence(OneWireSlave* bus) {
    // wait while master delay presence check
    onewire_slave_wait_while_gpio_is(bus, OWS_PRESENCE_TIMEOUT, true);

    // show presence
    onewire_slave_pin_set_low(bus);
    delay_us(OWS_PRESENCE_MIN);
    onewire_slave_pin_set_float(bus);

    // somebody also can show presence
    const uint32_t wait_low_time = OWS_PRESENCE_MAX - OWS_PRESENCE_MIN;

    // so we will wait
    if(onewire_slave_wait_while_gpio_is(bus, wait_low_time, false) == 0) {
        bus->error = PRESENCE_LOW_ON_LINE;
        return false;
    }

    return true;
}

bool onewire_slave_receive_bit(OneWireSlave* bus) {
    // wait while bus is low
    uint32_t time = OWS_SLOT_MAX;
    time = onewire_slave_wait_while_gpio_is(bus, time, false);
    if(time == 0) {
        bus->error = RESET_IN_PROGRESS;
        return false;
    }

    // wait while bus is high
    time = OWS_MSG_HIGH_TIMEOUT;
    time = onewire_slave_wait_while_gpio_is(bus, time, true);
    if(time == 0) {
        bus->error = AWAIT_TIMESLOT_TIMEOUT_HIGH;
        return false;
    }

    // wait a time of zero
    time = OWS_READ_MIN;
    time = onewire_slave_wait_while_gpio_is(bus, time, false);

    return (time > 0);
}

bool onewire_slave_send_bit(OneWireSlave* bus, bool value) {
    const bool write_zero = !value;

    // wait while bus is low
    uint32_t time = OWS_SLOT_MAX;
    time = onewire_slave_wait_while_gpio_is(bus, time, false);
    if(time == 0) {
        bus->error = RESET_IN_PROGRESS;
        return false;
    }

    // wait while bus is high
    time = OWS_MSG_HIGH_TIMEOUT;
    time = onewire_slave_wait_while_gpio_is(bus, time, true);
    if(time == 0) {
        bus->error = AWAIT_TIMESLOT_TIMEOUT_HIGH;
        return false;
    }

    // choose write time
    if(write_zero) {
        onewire_slave_pin_set_low(bus);
        time = OWS_WRITE_ZERO;
    } else {
        time = OWS_READ_MAX;
    }

    // hold line for ZERO or ONE time
    delay_us(time);
    onewire_slave_pin_set_float(bus);

    return true;
}

void onewire_slave_cmd_search_rom(OneWireSlave* bus) {
    const uint8_t key_bytes = 8;
    uint8_t* key = onewire_device_get_id_p(bus->device);

    for(uint8_t i = 0; i < key_bytes; i++) {
        uint8_t key_byte = key[i];

        for(uint8_t j = 0; j < 8; j++) {
            bool bit = (key_byte >> j) & 0x01;

            if(!onewire_slave_send_bit(bus, bit)) return;
            if(!onewire_slave_send_bit(bus, !bit)) return;

            onewire_slave_receive_bit(bus);
            if(bus->error != NO_ERROR) return;
        }
    }
}

bool onewire_slave_receive_and_process_cmd(OneWireSlave* bus) {
    uint8_t cmd;
    onewire_slave_receive(bus, &cmd, 1);

    if(bus->error == RESET_IN_PROGRESS) return true;
    if(bus->error != NO_ERROR) return false;

    switch(cmd) {
    case 0xF0:
        // SEARCH ROM
        onewire_slave_cmd_search_rom(bus);
        return true;

    case 0x0F:
    case 0x33:
        // READ ROM
        onewire_device_send_id(bus->device);
        return true;

    default: // Unknown command
        bus->error = INCORRECT_ONEWIRE_CMD;
    }

    if(bus->error == RESET_IN_PROGRESS) return true;
    return (bus->error == NO_ERROR);
}

bool onewire_slave_bus_start(OneWireSlave* bus) {
    bool result = true;

    if(bus->device == NULL) {
        result = false;
    } else {
        __disable_irq();
        onewire_slave_pin_init_opendrain_in_isr_ctx(bus);
        bus->error = NO_ERROR;

        if(onewire_slave_show_presence(bus)) {
            // TODO think about multiple command cycles
            onewire_slave_receive_and_process_cmd(bus);
            result = (bus->error == NO_ERROR || bus->error == INCORRECT_ONEWIRE_CMD);

        } else {
            result = false;
        }

        onewire_slave_pin_init_interrupt_in_isr_ctx(bus);
        __enable_irq();
    }

    return result;
}

static void exti_cb(void* context) {
    OneWireSlave* bus = context;

    volatile bool input_state = hal_gpio_read(bus->pin);
    static uint32_t pulse_start = 0;

    if(input_state) {
        uint32_t pulse_length = (DWT->CYCCNT - pulse_start) / instructions_per_us;
        if(pulse_length >= OWS_RESET_MIN) {
            if(pulse_length <= OWS_RESET_MAX) {
                // reset cycle ok
                bool result = onewire_slave_bus_start(bus);
                if(result && bus->result_cb != NULL) {
                    bus->result_cb(bus->result_cb_ctx);
                }
            } else {
                bus->error = VERY_LONG_RESET;
            }
        } else {
            bus->error = VERY_SHORT_RESET;
        }
    } else {
        //FALL event
        pulse_start = DWT->CYCCNT;
    }
};

/*********************** PUBLIC ***********************/

OneWireSlave* onewire_slave_alloc(const GpioPin* pin) {
    OneWireSlave* bus = malloc(sizeof(OneWireSlave));
    bus->error = NO_ERROR;
    bus->pin = pin;
    bus->device = NULL;
    bus->result_cb = NULL;
    bus->result_cb_ctx = NULL;
    return bus;
}

void onewire_slave_free(OneWireSlave* bus) {
    onewire_slave_stop(bus);
    free(bus);
}

void onewire_slave_start(OneWireSlave* bus) {
    hal_gpio_add_int_callback(bus->pin, exti_cb, bus);
    hal_gpio_init(bus->pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
    onewire_slave_pin_set_float(bus);
}

void onewire_slave_stop(OneWireSlave* bus) {
    hal_gpio_init(bus->pin, GpioModeInput, GpioPullNo, GpioSpeedLow);
    hal_gpio_remove_int_callback(bus->pin);
    onewire_slave_detach(bus);
}

void onewire_slave_attach(OneWireSlave* bus, OneWireDevice* device) {
    bus->device = device;
    onewire_device_attach(device, bus);
}

void onewire_slave_detach(OneWireSlave* bus) {
    if(bus->device != NULL) {
        onewire_device_detach(bus->device);
    }
    bus->device = NULL;
}

bool onewire_slave_send(OneWireSlave* bus, const uint8_t* address, const uint8_t data_length) {
    uint8_t bytes_sent = 0;

    onewire_slave_pin_set_float(bus);

    // bytes loop
    for(; bytes_sent < data_length; ++bytes_sent) {
        const uint8_t data_byte = address[bytes_sent];

        // bit loop
        for(uint8_t bit_mask = 0x01; bit_mask != 0; bit_mask <<= 1) {
            if(!onewire_slave_send_bit(bus, bit_mask & data_byte)) {
                // if we cannot send first bit
                if((bit_mask == 0x01) && (bus->error == AWAIT_TIMESLOT_TIMEOUT_HIGH))
                    bus->error = FIRST_BIT_OF_BYTE_TIMEOUT;
                return false;
            }
        }
    }
    return true;
}

bool onewire_slave_receive(OneWireSlave* bus, uint8_t* data, const uint8_t data_length) {
    uint8_t bytes_received = 0;

    onewire_slave_pin_set_float(bus);

    for(; bytes_received < data_length; ++bytes_received) {
        uint8_t value = 0;

        for(uint8_t bit_mask = 0x01; bit_mask != 0; bit_mask <<= 1) {
            if(onewire_slave_receive_bit(bus)) value |= bit_mask;
        }

        data[bytes_received] = value;
    }
    return (bytes_received != data_length);
}

void onewire_slave_set_result_callback(
    OneWireSlave* bus,
    OneWireSlaveResultCallback result_cb,
    void* context) {
    bus->result_cb = result_cb;
    bus->result_cb_ctx = context;
}
