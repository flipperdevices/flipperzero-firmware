#include "subghz_cli.h"
#include <furi.h>
#include <api-hal.h>

void subghz_cli_init() {
    Cli* cli = furi_record_open("cli");

    cli_add_command(cli, "subghz_tx_carrier", subghz_cli_command_tx, NULL)
    cli_add_command(cli, "subghz_rx_carrier", subghz_cli_command_tx, NULL)

    furi_record_close("cli");
}

void subghz_cli_command_tx_carrier(Cli* cli, string_t args, void* context) {
    printf("Use `subghz_tx_carrier <Frequency in HZ> <Path in 315/433/868>`");

    api_hal_subghz_reset();
    api_hal_subghz_load_preset(ApiHalSubGhzPresetOokAsync);
    api_hal_subghz_set_frequency(433920000);

    hal_gpio_init(&gpio_cc1101_g0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_cc1101_g0, false);

    api_hal_subghz_tx();

    while(cli_cmd_interrupt_received(cli)) {
        osDelay(25);
    }

    api_hal_subghz_reset();
    hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

void subghz_cli_command_rx_carrier(Cli* cli, string_t args, void* context) {

}
