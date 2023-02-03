#include "../mag_i.h"
#include <stdio.h>
#include <string.h>

void play_halfbit(bool value, MagSetting* setting);
void play_track(uint8_t* bits_manchester, uint16_t n_bits, MagSetting* setting, bool reverse);

void tx_init_rf(int hz);
void tx_init_rfid();
void tx_init_piezo();
bool tx_init(MagSetting* setting);
void tx_deinit_piezo();
void tx_deinit_rfid();
bool tx_deinit(MagSetting* setting);

uint16_t add_bit(bool value, uint8_t* out, uint16_t count);
uint16_t add_bit_manchester(bool value, uint8_t* out, uint16_t count);
uint16_t mag_encode(
    char* data,
    uint8_t* out_manchester,
    uint8_t* out_raw,
    uint8_t track_bits,
    uint8_t track_ascii_offset);
void debug_mag_string(char* data, uint8_t track_bits, uint8_t track_ascii_offset);
void mag_spoof(Mag* mag);
