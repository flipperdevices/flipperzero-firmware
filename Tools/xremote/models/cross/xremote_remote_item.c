#include "../infrared/xremote_ir_signal.h"
#include "xremote_remote_item.h"
#include <infrared_worker.h>
#include <infrared_transmit.h>
#include "../../xremote_i.h"

CrossRemoteItem* xremote_remote_item_alloc() {
    CrossRemoteItem* item = malloc(sizeof(CrossRemoteItem));
    item->name = furi_string_alloc();
    item->time = 0;
    item->type = 0;
    item->ir_signal = xremote_ir_signal_alloc();
    
    return item;
}

static inline bool xremote_ir_signal_save_message(InfraredMessage* message, FlipperFormat* ff) {
    const char* protocol_name = infrared_get_protocol_name(message->protocol);
    return flipper_format_write_string_cstr(ff, "type", "parsed") &&
           flipper_format_write_string_cstr(ff, "protocol", protocol_name) &&
           flipper_format_write_hex(ff, "address", (uint8_t*)&message->address, 4) &&
           flipper_format_write_hex(ff, "command", (uint8_t*)&message->command, 4);
}

static inline bool xremote_ir_signal_save_raw(InfraredRawSignal* raw, FlipperFormat* ff) {
    furi_assert(raw->timings_size <= MAX_TIMINGS_AMOUNT);
    return flipper_format_write_string_cstr(ff, "type", "raw") &&
           flipper_format_write_uint32(ff, "frequency", &raw->frequency, 1) &&
           flipper_format_write_float(ff, "duty_cycle", &raw->duty_cycle, 1) &&
           flipper_format_write_uint32(ff, "data", raw->timings, raw->timings_size);
}

void xremote_remote_item_free(CrossRemoteItem* item) {
    furi_string_free(item->name);
    free(item);
}

void xremote_remote_item_set_type(CrossRemoteItem* item, int type) {
    item->type = type;
}

void xremote_remote_item_set_name(CrossRemoteItem* item, const char* name) {
    furi_string_set(item->name, name);
}

void xremote_remote_item_set_ir_signal(CrossRemoteItem* item, InfraredSignal* signal) {
    xremote_ir_signal_set_signal(item->ir_signal, signal);
}

const char* xremote_remote_item_get_name(CrossRemoteItem* item) {
    return furi_string_get_cstr(item->name);
}

InfraredSignal* xremote_remote_item_get_ir_signal(CrossRemoteItem* item) {
    return item->ir_signal;
}

bool xremote_ir_signal_save(InfraredSignal* signal, FlipperFormat* ff, const char* name) {
    if(!flipper_format_write_comment_cstr(ff, "") || 
       !flipper_format_write_string_cstr(ff, "remote_type", "IR") || 
       !flipper_format_write_string_cstr(ff, "name", name)) {
        return false;
    } else if(signal->is_raw) {
        return xremote_ir_signal_save_raw(&signal->payload.raw, ff);
    } else {
        return xremote_ir_signal_save_message(&signal->payload.message, ff);
    }
}