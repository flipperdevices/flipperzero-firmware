#include "one_wire_slave.h"

#include <furi.h>
#include <furi_hal.h>

typedef enum {
    OneWireSlaveErrorNone = 0,
    OneWireSlaveErrorResetInProgress,
    OneWireSlaveErrorPresenceConflict,
    OneWireSlaveErrorInvalidCommand,
    OneWireSlaveErrorTimeout,
} OneWireSlaveError;

typedef struct {
    uint16_t trstl_min; /* Minimum Reset Low time */
    uint16_t trstl_max; /* Maximum Reset Low time */

    uint16_t tpdh_typ; /* Typical Presence Detect High time */
    uint16_t tpdl_min; /* Minimum Presence Detect Low time */
    uint16_t tpdl_max; /* Maximum Presence Detect Low time */

    uint16_t tslot_min; /* Minimum Read/Write Slot time */
    uint16_t tslot_max; /* Maximum Read/Write Slot time */

    uint16_t tw1l_max; /* Maximum Master Write 1 time */
    uint16_t trl_tmsr_max; /* Maximum Master Read Low + Read Sample time */

    uint16_t th_timeout; /* Maximum time before general timeout */
} OneWireSlaveTimings;

struct OneWireSlave {
    const GpioPin* gpio_pin;
    const OneWireSlaveTimings* timings;
    OneWireSlaveError error;

    OneWireSlaveResetCallback reset_callback;
    OneWireSlaveCommandCallback command_callback;
    OneWireSlaveResultCallback result_callback;

    void* reset_callback_context;
    void* result_callback_context;
    void* command_callback_context;
};

static const OneWireSlaveTimings onewire_slave_timings_normal = {
    .trstl_min = 270,
    .trstl_max = 1200,

    .tpdh_typ = 20,
    .tpdl_min = 100,
    .tpdl_max = 480,

    .tslot_min = 60,
    .tslot_max = 135,

    .tw1l_max = 20,
    .trl_tmsr_max = 30,

    .th_timeout = 15000,
};

static const OneWireSlaveTimings onewire_slave_timings_overdrive = {
    .trstl_min = 48,
    .trstl_max = 80,

    .tpdh_typ = 2,
    .tpdl_min = 7,
    .tpdl_max = 24,

    .tslot_min = 6,
    .tslot_max = 16,

    .tw1l_max = 1,
    .trl_tmsr_max = 2,

    .th_timeout = 15000,
};

/*********************** PRIVATE ***********************/

static uint32_t
    onewire_slave_wait_while_gpio_is(OneWireSlave* bus, uint32_t time, const bool pin_value) {
    uint32_t start = DWT->CYCCNT;
    uint32_t time_ticks = time * furi_hal_cortex_instructions_per_microsecond();
    uint32_t time_captured;

    do { //-V1044
        time_captured = DWT->CYCCNT;
        if(furi_hal_gpio_read(bus->gpio_pin) != pin_value) {
            uint32_t remaining_time = time_ticks - (time_captured - start);
            remaining_time /= furi_hal_cortex_instructions_per_microsecond();
            return remaining_time;
        }
    } while((time_captured - start) < time_ticks);

    return 0;
}

static bool onewire_slave_show_presence(OneWireSlave* bus) {
    const OneWireSlaveTimings* timings = bus->timings;
    // wait until the bus is high (might return immediately)
    onewire_slave_wait_while_gpio_is(bus, timings->trstl_max, false);
    // wait while master delay presence check
    furi_delay_us(timings->tpdh_typ);

    // show presence
    furi_hal_gpio_write(bus->gpio_pin, false);
    furi_delay_us(timings->tpdl_min);
    furi_hal_gpio_write(bus->gpio_pin, true);

    // somebody also can show presence
    const uint32_t wait_low_time = timings->tpdl_max - timings->tpdl_min;

    // so we will wait
    if(onewire_slave_wait_while_gpio_is(bus, wait_low_time, false) == 0) {
        bus->error = OneWireSlaveErrorPresenceConflict;
        return false;
    }

    return true;
}

static inline bool onewire_slave_receive_and_process_command(OneWireSlave* bus) {
    /* Reset condition detected, send a presence pulse and reset protocol state */
    if(bus->error == OneWireSlaveErrorResetInProgress) {
        if(onewire_slave_show_presence(bus)) {
            bus->error = OneWireSlaveErrorNone;

            if(bus->reset_callback != NULL) {
                bus->reset_callback(bus->reset_callback_context);
            }

            return true;
        }

    } else if(bus->error == OneWireSlaveErrorNone) {
        uint8_t command;
        if(!onewire_slave_receive(bus, &command, 1)) {
            /* Upon failure, request an additional iteration to
               choose the appropriate action by checking bus->error */
            return true;
        } else if(bus->command_callback) {
            return bus->command_callback(command, bus->command_callback_context);
        } else {
            bus->error = OneWireSlaveErrorInvalidCommand;
        }
    }

    return false;
}

static inline bool onewire_slave_bus_start(OneWireSlave* bus) {
    FURI_CRITICAL_ENTER();
    furi_hal_gpio_init(bus->gpio_pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);

    /* Start in Reset state in order to send a presence pulse immediately */
    bus->error = OneWireSlaveErrorResetInProgress;

    while(onewire_slave_receive_and_process_command(bus))
        ;

    const bool result = (bus->error == OneWireSlaveErrorNone);

    furi_hal_gpio_init(bus->gpio_pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
    FURI_CRITICAL_EXIT();

    return result;
}

static void onewire_slave_exti_callback(void* context) {
    OneWireSlave* bus = context;
    const OneWireSlaveTimings* timings = bus->timings;

    const volatile bool input_state = furi_hal_gpio_read(bus->gpio_pin);
    static uint32_t pulse_start = 0;

    if(input_state) {
        const uint32_t pulse_length =
            (DWT->CYCCNT - pulse_start) / furi_hal_cortex_instructions_per_microsecond();

        if((pulse_length >= timings->trstl_min) && pulse_length <= (timings->trstl_max)) {
            const bool result = onewire_slave_bus_start(bus);

            if(result && bus->result_callback != NULL) {
                bus->result_callback(bus->result_callback_context);
            }
        }

    } else {
        pulse_start = DWT->CYCCNT;
    }
};

/*********************** PUBLIC ***********************/

OneWireSlave* onewire_slave_alloc(const GpioPin* gpio_pin) {
    OneWireSlave* bus = malloc(sizeof(OneWireSlave));

    bus->gpio_pin = gpio_pin;
    bus->timings = &onewire_slave_timings_normal;
    bus->error = OneWireSlaveErrorNone;

    return bus;
}

void onewire_slave_free(OneWireSlave* bus) {
    onewire_slave_stop(bus);
    free(bus);
}

void onewire_slave_start(OneWireSlave* bus) {
    furi_hal_gpio_add_int_callback(bus->gpio_pin, onewire_slave_exti_callback, bus);
    furi_hal_gpio_write(bus->gpio_pin, true);
    furi_hal_gpio_init(bus->gpio_pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
}

void onewire_slave_stop(OneWireSlave* bus) {
    furi_hal_gpio_write(bus->gpio_pin, true);
    furi_hal_gpio_init(bus->gpio_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_remove_int_callback(bus->gpio_pin);
}

void onewire_slave_set_reset_callback(
    OneWireSlave* bus,
    OneWireSlaveResetCallback callback,
    void* context) {
    bus->reset_callback = callback;
    bus->reset_callback_context = context;
}

void onewire_slave_set_command_callback(
    OneWireSlave* bus,
    OneWireSlaveCommandCallback callback,
    void* context) {
    bus->command_callback = callback;
    bus->command_callback_context = context;
}

void onewire_slave_set_result_callback(
    OneWireSlave* bus,
    OneWireSlaveResultCallback result_cb,
    void* context) {
    bus->result_callback = result_cb;
    bus->result_callback_context = context;
}

bool onewire_slave_receive_bit(OneWireSlave* bus) {
    const OneWireSlaveTimings* timings = bus->timings;
    // wait while bus is low
    uint32_t time = timings->tslot_max;
    time = onewire_slave_wait_while_gpio_is(bus, time, false);
    if(time == 0) {
        bus->error = OneWireSlaveErrorResetInProgress;
        return false;
    }

    // wait while bus is high
    time = timings->th_timeout;
    time = onewire_slave_wait_while_gpio_is(bus, time, true);
    if(time == 0) {
        bus->error = OneWireSlaveErrorTimeout;
        return false;
    }

    // wait a time of zero
    time = timings->tw1l_max;
    time = onewire_slave_wait_while_gpio_is(bus, time, false);

    return (time > 0);
}

bool onewire_slave_send_bit(OneWireSlave* bus, bool value) {
    const OneWireSlaveTimings* timings = bus->timings;
    // wait while bus is low
    uint32_t time = timings->tslot_max;
    time = onewire_slave_wait_while_gpio_is(bus, time, false);
    if(time == 0) {
        bus->error = OneWireSlaveErrorResetInProgress;
        return false;
    }

    // wait while bus is high
    time = timings->th_timeout;
    time = onewire_slave_wait_while_gpio_is(bus, time, true);
    if(time == 0) {
        bus->error = OneWireSlaveErrorTimeout;
        return false;
    }

    // choose write time
    if(!value) {
        furi_hal_gpio_write(bus->gpio_pin, false);
        time = timings->trl_tmsr_max;
    } else {
        time = timings->tslot_min;
    }

    // hold line for ZERO or ONE time
    furi_delay_us(time);
    furi_hal_gpio_write(bus->gpio_pin, true);

    return true;
}

bool onewire_slave_send(OneWireSlave* bus, const uint8_t* data, size_t data_size) {
    furi_hal_gpio_write(bus->gpio_pin, true);

    size_t bytes_sent = 0;

    // bytes loop
    for(; bytes_sent < data_size; ++bytes_sent) {
        const uint8_t data_byte = data[bytes_sent];

        // bit loop
        for(uint8_t bit_mask = 0x01; bit_mask != 0; bit_mask <<= 1) {
            if(!onewire_slave_send_bit(bus, bit_mask & data_byte)) {
                return false;
            }
        }
    }
    return true;
}

bool onewire_slave_receive(OneWireSlave* bus, uint8_t* data, size_t data_size) {
    furi_hal_gpio_write(bus->gpio_pin, true);

    size_t bytes_received = 0;

    for(; bytes_received < data_size; ++bytes_received) {
        uint8_t value = 0;

        for(uint8_t bit_mask = 0x01; bit_mask != 0; bit_mask <<= 1) {
            if(onewire_slave_receive_bit(bus)) {
                value |= bit_mask;
            }

            if(bus->error != OneWireSlaveErrorNone) {
                return false;
            }
        }

        data[bytes_received] = value;
    }
    return true;
}

void onewire_slave_set_overdrive(OneWireSlave* bus, bool set) {
    bus->timings = set ? &onewire_slave_timings_overdrive : &onewire_slave_timings_normal;
}
