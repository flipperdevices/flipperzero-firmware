#pragma once

#include "../flipper.h"
#include <furi.h>
#include <furi_hal.h>

#include <flipper_format/flipper_format.h>

#include "../app_state.h"
#include "../item.h"

void action_subghz_tx(void* context, Item* item);
void action_rfid_tx(void* context, Item* item);
void action_ir_tx(void* context, Item* item);