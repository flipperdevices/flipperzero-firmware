#pragma once

#include "subghz_protocol_header.h"

void subghz_princeton_decoder_init(void);
void subghz_princeton_decoder_send_key(uint64_t key, uint8_t bit,uint8_t repeat);
void subghz_princeton_decoder_parse(LevelPair data);
