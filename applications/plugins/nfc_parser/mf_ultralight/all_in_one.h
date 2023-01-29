#pragma once

#include <furi/furi.h>
#include <lib/nfc/protocols/mifare_ultralight.h>

bool all_in_one_verify(MfUltralightData* data);

bool all_in_one_parse(MfUltralightData* data, FuriString* parsed_data);
