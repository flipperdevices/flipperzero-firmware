#include "can_lin_tools_app_i.h"

#include <furi.h>

#define TAG "CanLinTools"

void can_lin_tools_app_can_hacker2_start(CanLinToolsApp* app) {
    furi_assert(app);

    // start the field presence rfid detection
    furi_hal_rfid_field_detect_start();

    // start the field presence nfc detection
    furi_hal_nfc_exit_sleep();
    furi_hal_nfc_field_detect_start();
}

void can_lin_tools_app_can_hacker2_stop(CanLinToolsApp* app) {
    furi_assert(app);

    // stop the field presence rfid detection
    furi_hal_rfid_field_detect_stop();

    // stop the field presence nfc detection
    furi_hal_nfc_start_sleep();
}

bool can_lin_tools_app_can_hacker2_is_nfc(CanLinToolsApp* app) {
    furi_assert(app);

    // check if the field presence is nfc
    return furi_hal_nfc_field_is_present();
}

bool can_lin_tools_app_can_hacker2_is_rfid(CanLinToolsApp* app, uint32_t* frequency) {
    furi_assert(app);

    // check if the field presence is rfid
    return furi_hal_rfid_field_is_present(frequency);
}