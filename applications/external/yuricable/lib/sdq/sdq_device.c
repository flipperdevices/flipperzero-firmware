#include <lib/sdq/sdq_device.h>

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

const TRISTART_RESPONSES responses = {
    .RESET_DEVICE = {0x75, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00},
    .DFU = {0x75, 0x20, 0x00, 0x02, 0x00, 0x00, 0x00},
    .USB_UART_JTAG = {0x75, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00},
    .USB_SPAM_JTAG = {0x75, 0xa0, 0x08, 0x10, 0x00, 0x00, 0x00},
    .USB_UART = {0x75, 0x20, 0x00, 0x10, 0x00, 0x00, 0x00},
    .USB_A_CHARGING_CABLE = {0x75, 0x10, 0x0c, 0x00, 0x00, 0x00, 0x00},
    .POWER_ANSWER = {0x71, 0x93}};

uint8_t RECOVERY_PLIST[277] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"><plist version=\"1.0\"><dict> <key>Label</key> <string>yuricable</string> <key>Request</key> <string>EnterRecovery</string> </dict></plist>";

uint8_t crc_data(const uint8_t* data, size_t len) {
    crc_t crc = crc_init();
    crc = crc_update(crc, data, len);
    crc = crc_finalize(crc);
    return crc;
}

void sdq_delay_us(uint32_t time_us) {
    furi_delay_us(time_us - 1);
    return;
}

struct SDQDevice* sdq_device_alloc(const GpioPin* gpio_pin, UsbUartBridge* uart_bridge) {
    struct SDQDevice* bus = malloc(sizeof(struct SDQDevice));
    bus->gpio_pin = gpio_pin;
    bus->uart_bridge = uart_bridge;
    bus->timings = sdq_timings;
    bus->error = SDQDeviceErrorNone;
    bus->runCommand = SDQDeviceCommand_NONE;
    return bus;
}

void sdq_device_free(SDQDevice* bus) {
    sdq_device_stop(bus);
    free(bus);
}

static bool sdq_device_wait_while_gpio_is(SDQDevice* bus, uint32_t time_us, const bool pin_value) {
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

static inline bool sdq_device_receive_and_process_command(SDQDevice* bus) {
    uint8_t command[4] = {0};
    if(sdq_device_receive(bus, command, sizeof(command))) {
        if(sdq_device_wait_while_gpio_is(bus, bus->timings.BREAK_meaningful_max, false)) {
            switch(command[0]) {
            case TRISTAR_POLL:
                switch(bus->runCommand) {
                case SDQDeviceCommand_NONE:
                    bus->commandExecuted = true;
                    break;
                case SDQDeviceCommand_RESET:
                    if(sdq_device_send(bus, responses.RESET_DEVICE, sizeof(responses.RESET_DEVICE))) {
                        bus->commandExecuted = true;
                    }
                    break;
                case SDQDeviceCommand_DFU:
                    if(bus->resetInProgress) {
                        if(sdq_device_send(bus, responses.DFU, sizeof(responses.DFU))) {
                            bus->resetInProgress = false;
                            bus->commandExecuted = true;
                        }
                    } else {
                        if(sdq_device_send(bus, responses.RESET_DEVICE, sizeof(responses.RESET_DEVICE))) {
                            bus->resetInProgress = true;
                        }
                    }
                    break;
                case SDQDeviceCommand_DCSD:
                    if(bus->resetInProgress) {
                        if(sdq_device_send(bus, responses.USB_UART, sizeof(responses.USB_UART))) {
                            bus->resetInProgress = false;
                            bus->commandExecuted = true;
                        }
                    } else {
                        if(sdq_device_send(bus, responses.RESET_DEVICE, sizeof(responses.RESET_DEVICE))) {
                            bus->resetInProgress = true;
                        }
                    }
                    break;
                case SDQDeviceCommand_CHARGING:
                    if(sdq_device_send(bus, responses.USB_A_CHARGING_CABLE, sizeof(responses.USB_A_CHARGING_CABLE))) {
                        bus->commandExecuted = true;
                    }
                    break;
                case SDQDeviceCommand_JTAG:
                    break;
                case SDQDeviceCommand_RECOVERY:
                    if(sdq_device_send(bus, responses.USB_UART, sizeof(responses.USB_UART))) {
                        usb_uart_send_data(bus->uart_bridge, RECOVERY_PLIST, sizeof(RECOVERY_PLIST));
                        bus->commandExecuted = true;
                    }
                    break;
                default:
                    break;
                }
                break;
            case TRISTAR_UNKNOWN_76:
                FURI_LOG_I("SDQ", "TRISTAR_UNKNOWN_76");
                break;
            case TRISTAR_POWER:
                sdq_device_send(bus, responses.POWER_ANSWER, sizeof(responses.POWER_ANSWER));
                break;
            default:
                break;
            }
        }
    }
    return (bus->error == SDQDeviceErrorNone);
}

static inline bool sdq_device_bus_start(SDQDevice* bus) {
    bus->connected = true;
    while(sdq_device_receive_and_process_command(bus))
        ;
    const bool result = (bus->error == SDQDeviceErrorNone);
    bus->connected = false;
    return result;
}

static void sdq_device_exti_callback(void* context) {
    SDQDevice* bus = context;
    FURI_CRITICAL_ENTER();
    furi_hal_gpio_init(bus->gpio_pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    if(sdq_device_wait_while_gpio_is(bus, bus->timings.BREAK_meaningful_max - 8, false)) {
        if(sdq_device_wait_while_gpio_is(bus, bus->timings.BREAK_recovery + 1, true)) {
            sdq_device_bus_start(bus);
        }
    }
    furi_hal_gpio_init(bus->gpio_pin, GpioModeInterruptFall, GpioPullUp, GpioSpeedLow);
    FURI_CRITICAL_EXIT();
}

void sdq_device_start(SDQDevice* bus) {
    furi_hal_gpio_add_int_callback(bus->gpio_pin, sdq_device_exti_callback, bus);
    furi_hal_gpio_write(bus->gpio_pin, true);
    furi_hal_gpio_init(bus->gpio_pin, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);
    bus->listening = true;
}

void sdq_device_stop(SDQDevice* bus) {
    bus->listening = false;
    furi_hal_gpio_write(bus->gpio_pin, true);
    furi_hal_gpio_init(bus->gpio_pin, GpioModeAnalog, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_remove_int_callback(bus->gpio_pin);
}

uint8_t sdq_device_receive_bit(SDQDevice* bus, bool isLastBitofByte) {
    const SDQTimings* timings = &bus->timings;
    // wait while bus is low for one meaningful
    if(sdq_device_wait_while_gpio_is(bus, timings->ONE_meaningful_max, false)) {
        // wait while bus is high for one recovery
        if(isLastBitofByte) {
            if(sdq_device_wait_while_gpio_is(bus, timings->ONE_STOP_recovery, true)) {
                bus->error = SDQDeviceErrorNone;
                return true;
            }
        } else {
            if(sdq_device_wait_while_gpio_is(bus, timings->ONE_recovery, true)) {
                bus->error = SDQDeviceErrorNone;
                return true;
            }
        }
    }
    // wait while bus is low for zero meaningful
    if(sdq_device_wait_while_gpio_is(bus, timings->ZERO_meaningful_max - timings->ONE_meaningful_max, false)) {
        // wait while bus is high for zero recovery
        if(isLastBitofByte) {
            if(sdq_device_wait_while_gpio_is(bus, timings->ZERO_STOP_recovery, true)) {
                bus->error = SDQDeviceErrorNone;
                return false;
            }
        } else {
            if(sdq_device_wait_while_gpio_is(bus, timings->ZERO_recovery, true)) {
                bus->error = SDQDeviceErrorNone;
                return false;
            }
        }
    }
    bus->error = SDQDeviceErrorInvalidCommand;
    return false;
}

static void sdq_device_send_byte(SDQDevice* bus, uint8_t byte) {
    const SDQTimings* timings = &bus->timings;
    for(uint8_t mask = 0x01; mask != 0; mask <<= 1) {
        uint32_t meaningful_time = (mask & byte) ? timings->ONE_meaningful : timings->ZERO_meaningful;
        uint32_t recovery_time = (mask & byte) ? timings->ONE_recovery : timings->ZERO_recovery;
        furi_hal_gpio_write(bus->gpio_pin, false);
        sdq_delay_us(meaningful_time);
        furi_hal_gpio_write(bus->gpio_pin, true);
        // Add stop recovery time for the last bit in the byte
        if(mask == 0x80) {
            recovery_time = (mask & byte) ? timings->ONE_STOP_recovery : timings->ZERO_STOP_recovery;
        }
        sdq_delay_us(recovery_time);
    }
}

bool sdq_device_send(SDQDevice* bus, const uint8_t data[], size_t data_size) {
    const SDQTimings* timings = &bus->timings;

    if(!bus->connected) {
        bus->error = SDQDeviceErrorNotConnected;
        return false;
    }

    for(size_t i = 0; i < data_size; ++i) {
        sdq_device_send_byte(bus, data[i]);
    }
    // Calculate and send CRC8
    uint8_t crc = crc_data(data, data_size);
    sdq_device_send_byte(bus, crc);

    // Send Break
    furi_hal_gpio_write(bus->gpio_pin, false);
    sdq_delay_us(timings->BREAK_meaningful);
    furi_hal_gpio_write(bus->gpio_pin, true);
    sdq_delay_us(timings->BREAK_recovery);

    return true;
}

bool sdq_device_receive(SDQDevice* bus, uint8_t data[], size_t data_size) {
    size_t bytes_received = 0;
    for(; bytes_received < data_size; ++bytes_received) {
        uint8_t value = 0;
        for(uint8_t bit_mask = 0x01; bit_mask != 0; bit_mask <<= 1) {
            if(sdq_device_receive_bit(bus, (bit_mask == 0x80)) && bus->error == SDQDeviceErrorNone) {
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
        bus->error = SDQDeviceErrorInvalidCommand;
        return false;
    }
    return true;
}