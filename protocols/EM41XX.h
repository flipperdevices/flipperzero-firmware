#include "../helpers/t5577.h"

bool add_em41xx_data(LFRFIDT5577Data* data, uint64_t key, uint8_t from_index);

bool set_em41xx_config(LFRFIDT5577Data* data, uint8_t keys_count);

uint64_t bytes2num(const uint8_t* src, uint8_t len);