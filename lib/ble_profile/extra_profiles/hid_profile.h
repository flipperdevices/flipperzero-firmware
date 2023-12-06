#pragma once

#include <furi_ble/profile_interface.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const FuriHalBleProfileConfig* ble_profile_hid;

bool ble_profile_hid_kb_press(FuriHalBleProfileBase* profile, uint16_t button);

bool ble_profile_hid_kb_release(FuriHalBleProfileBase* profile, uint16_t button);

bool ble_profile_hid_kb_release_all(FuriHalBleProfileBase* profile);

bool ble_profile_hid_consumer_key_press(FuriHalBleProfileBase* profile, uint16_t button);

bool ble_profile_hid_consumer_key_release(FuriHalBleProfileBase* profile, uint16_t button);

bool ble_profile_hid_consumer_key_release_all(FuriHalBleProfileBase* profile);

bool ble_profile_hid_mouse_move(FuriHalBleProfileBase* profile, int8_t dx, int8_t dy);

bool ble_profile_hid_mouse_press(FuriHalBleProfileBase* profile, uint8_t button);

bool ble_profile_hid_mouse_release(FuriHalBleProfileBase* profile, uint8_t button);

bool ble_profile_hid_mouse_release_all(FuriHalBleProfileBase* profile);

bool ble_profile_hid_mouse_scroll(FuriHalBleProfileBase* profile, int8_t delta);

#ifdef __cplusplus
}
#endif
