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
void mag_spoof(Mag* mag);
void set_bit(uint8_t* b, uint32_t blen, uint32_t bitpos, bool val);
bool get_bit(uint8_t* b, uint32_t blen, uint32_t bitpos);


#define PREFIX_NUM_ZEROES 25
#define BITS_TRACK1 7
#define OFFSET_TRACK1 32
#define BITS_TRACK2 5
#define OFFSET_TRACK2 48
uint16_t add_bit(bool value, uint8_t* out, uint16_t count);
uint16_t add_bit_manchester(bool value, uint8_t* out, uint16_t count);
uint16_t msr_encode(char* data, uint8_t* out_manchester, uint8_t* out_raw, uint8_t track_bits, uint8_t track_ascii_offset);
void debug_msr_string(char* data,  uint8_t track_bits, uint8_t track_ascii_offset);
void mag_spoof_bitwise(Mag* mag);