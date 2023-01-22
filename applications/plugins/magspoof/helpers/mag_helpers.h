#include "../mag_i.h"
#include <stdio.h>
#include <string.h>

void play_bit_rfid(uint8_t send_bit, MagSetting* setting);
void play_bit_gpio(uint8_t send_bit, MagSetting* setting);
bool play_bit(uint8_t send_bit, MagSetting* setting);
void tx_init_rfid();
void tx_init_gpio();
void tx_reset_rfid();
void tx_reset_gpio();
bool tx_init(MagSetting* setting);
bool tx_reset(MagSetting* setting);
void track_to_bits(uint8_t* bit_array, const char* track_data, uint8_t track_index);
//void mag_spoof_single_track_rfid(FuriString* track_str, uint8_t track_index);
//void mag_spoof_two_track_rfid(FuriString* track1, FuriString* track2);
void mag_spoof(Mag* mag);
void set_bit(uint8_t* b, uint32_t blen, uint32_t bitpos, bool val);
bool get_bit(uint8_t* b, uint32_t blen, uint32_t bitpos);
