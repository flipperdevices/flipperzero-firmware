#include "../wiegand.h"

void single_vibro() {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_single_vibro);
    furi_record_close(RECORD_NOTIFICATION);
}

void wiegand_play() {
    uint32_t* delays = malloc(sizeof(uint32_t) * bit_count * 2);
    for(int i = 0; i < bit_count - 1; i++) {
        delays[i * 2] = (data_rise[i] - data_fall[i]) / 64;
        delays[i * 2 + 1] = (data_fall[i + 1] - data_rise[i]) / 64;
    }
    delays[(bit_count - 1) * 2] = (data_rise[bit_count - 1] - data_fall[bit_count - 1]) / 64;
    delays[(bit_count - 1) * 2 + 1] = 1;

    for(int i = 0; i < bit_count; i++) {
        // Delays are always at least 1 tick.
        if(delays[i * 2] == 0) delays[i * 2] = 1;
        if(delays[i * 2 + 1] == 0) delays[i * 2 + 1] = 1;
        if(delays[i * 2 + 1] > 5) delays[i * 2 + 1] -= 1;
    }

    furi_hal_gpio_write(pinD0, true);
    furi_hal_gpio_init(pinD0, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(pinD0mosfet, false);
    furi_hal_gpio_init(pinD0mosfet, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(pinD1, true);
    furi_hal_gpio_init(pinD1, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(pinD1mosfet, false);
    furi_hal_gpio_init(pinD1mosfet, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    single_vibro();
    furi_delay_ms(500);

    int j = 0;
    for(int i = 0; i < bit_count; i++) {
        if(data[i]) {
            furi_hal_gpio_write(pinD1mosfet, true); // Activate the mosfet to ground wire
            furi_hal_gpio_write(pinD1, false); // Ground the open-drain wire
            furi_delay_us(delays[j++]);
            furi_hal_gpio_write(pinD1, true); // Float the wire
            furi_hal_gpio_write(pinD1mosfet, false); // Deactivate the mosfet
            furi_delay_us(delays[j++]);
        } else {
            furi_hal_gpio_write(pinD0mosfet, true); // Activate the mosfet to ground wire
            furi_hal_gpio_write(pinD0, false); // Ground the open-drain wire
            furi_delay_us(delays[j++]);
            furi_hal_gpio_write(pinD0, true); // Float the wire
            furi_hal_gpio_write(pinD0mosfet, false); // Deactivate the mosfet
            furi_delay_us(delays[j++]);
        }
    }

    furi_hal_gpio_init_simple(pinD0, GpioModeAnalog);
    furi_hal_gpio_init_simple(pinD1, GpioModeAnalog);
    furi_hal_gpio_init_simple(pinD0mosfet, GpioModeAnalog);
    furi_hal_gpio_init_simple(pinD1mosfet, GpioModeAnalog);
}