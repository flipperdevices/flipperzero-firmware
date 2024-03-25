#include "nfc_cli_i.h"

#include <furi_hal.h>

void nfc_cli_field(NfcCli* instance, FuriString* args) {
    UNUSED(args);

    furi_hal_nfc_low_power_mode_stop();
    furi_hal_nfc_poller_field_on();

    printf("Field is on. Don't leave device in this mode for too long.\r\n");
    printf("Press Ctrl+C to abort\r\n");

    while(!nfc_cli_abort_received(instance)) {
        furi_delay_ms(50);
    }

    furi_hal_nfc_low_power_mode_start();
}
