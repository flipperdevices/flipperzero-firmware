#pragma once

#include "ibutton_key.h"

#include "protocols/ibutton_protocol_base.h"

iButtonProtocolData* ibutton_key_get_protocol_data(const iButtonKey* key);

size_t ibutton_key_get_protocol_data_size(const iButtonKey* key);
