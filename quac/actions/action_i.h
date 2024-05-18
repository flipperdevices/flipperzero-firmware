#pragma once

#include <furi.h>
#include <furi_hal.h>

#define ACTION_SET_ERROR(_msg_fmt, ...) furi_string_printf(error, _msg_fmt, ##__VA_ARGS__)

void action_subghz_tx(void* context, const FuriString* action_path, FuriString* error);
void action_rfid_tx(void* context, const FuriString* action_path, FuriString* error);
void action_ir_tx(void* context, const FuriString* action_path, FuriString* error);
void action_nfc_tx(void* context, const FuriString* action_path, FuriString* error);
void action_qpl_tx(void* context, const FuriString* action_path, FuriString* error);