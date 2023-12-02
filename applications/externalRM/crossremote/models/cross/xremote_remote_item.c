#include "xremote_remote_item.h"

CrossRemoteItem* xremote_remote_item_alloc() {
    CrossRemoteItem* item = malloc(sizeof(CrossRemoteItem));
    item->name = furi_string_alloc();
    item->time = 0;
    item->type = 0;
    item->ir_signal = xremote_ir_signal_alloc();
    item->sg_signal = xremote_sg_remote_alloc();

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

static inline bool xremote_sg_signal_save_data(SubGhzRemote* remote, FlipperFormat* ff) {
    UNUSED(remote);
    UNUSED(ff);
    return true;
    /*return flipper_format_write_uint32(ff, "frequency", xremote_sg_remote_get_frequency(remote)) &&
           flipper_format_write_string_cstr(ff, "preset", xremote_sg_remote_get_preset(remote));*/
}

static bool xremote_ir_signal_is_message_valid(InfraredMessage* message) {
    if(!infrared_is_protocol_valid(message->protocol)) {
        FURI_LOG_E(TAG, "Unknown protocol");
        return false;
    }

    uint32_t address_length = infrared_get_protocol_address_length(message->protocol);
    uint32_t address_mask = (1UL << address_length) - 1;

    if(message->address != (message->address & address_mask)) {
        FURI_LOG_E(
            TAG,
            "Address is out of range (mask 0x%08lX): 0x%lX\r\n",
            address_mask,
            message->address);
        return false;
    }

    uint32_t command_length = infrared_get_protocol_command_length(message->protocol);
    uint32_t command_mask = (1UL << command_length) - 1;

    if(message->command != (message->command & command_mask)) {
        FURI_LOG_E(
            TAG,
            "Command is out of range (mask 0x%08lX): 0x%lX\r\n",
            command_mask,
            message->command);
        return false;
    }

    return true;
}

bool xremote_remote_item_read(CrossRemoteItem* item, FlipperFormat* ff) {
    FuriString* type = furi_string_alloc();
    bool success = false;
    do {
        if(!flipper_format_read_string(ff, "remote_type", type)) break;
        if(furi_string_equal(type, "IR")) {
            success = xremote_remote_item_read_ir(item, ff);
        } else if(furi_string_equal(type, "PAUSE")) {
            success = xremote_remote_item_read_pause(item, ff);
        } else {
            break;
        }
        success = true;
    } while(false);
    return success;
}

bool xremote_remote_item_read_ir(CrossRemoteItem* item, FlipperFormat* ff) {
    FuriString* buf;
    bool success = false;
    buf = furi_string_alloc();
    item->type = XRemoteRemoteItemTypeInfrared;
    item->time = 0;

    do {
        if(!flipper_format_read_string(ff, "name", item->name)) break;
        if(!flipper_format_read_string(ff, "type", buf)) break;
        if(furi_string_equal(buf, "raw")) {
            if(!xremote_remote_item_read_ir_signal_raw(item, ff)) break;
        } else if(furi_string_equal(buf, "parsed")) {
            if(!xremote_remote_item_read_message(item, ff)) break;
        } else {
            break;
        }
        success = true;
    } while(false);
    furi_string_free(buf);

    return success;
}

bool xremote_remote_item_read_pause(CrossRemoteItem* item, FlipperFormat* ff) {
    bool success = false;
    item->type = XRemoteRemoteItemTypePause;

    do {
        if(!flipper_format_read_string(ff, "name", item->name)) break;
        if(!flipper_format_read_int32(ff, "time", &item->time, 1)) break;
        success = true;
    } while(false);

    return success;
}

bool xremote_remote_item_read_ir_signal_raw(CrossRemoteItem* item, FlipperFormat* ff) {
    uint32_t timings_size, frequency;
    float duty_cycle;

    bool success = flipper_format_read_uint32(ff, "frequency", &frequency, 1) &&
                   flipper_format_read_float(ff, "duty_cycle", &duty_cycle, 1) &&
                   flipper_format_get_value_count(ff, "data", &timings_size);

    if(!success || timings_size > MAX_TIMINGS_AMOUNT) {
        return false;
    }

    uint32_t* timings = malloc(sizeof(uint32_t) * timings_size);
    success = flipper_format_read_uint32(ff, "data", timings, timings_size);

    if(success) {
        xremote_ir_signal_set_raw_signal(
            item->ir_signal, timings, timings_size, frequency, duty_cycle);
    }

    free(timings);

    return success;
}

bool xremote_remote_item_read_message(CrossRemoteItem* item, FlipperFormat* ff) {
    FuriString* buf = furi_string_alloc();
    bool success = false;

    do {
        if(!flipper_format_read_string(ff, "protocol", buf)) break;
        InfraredMessage message;
        message.protocol = infrared_get_protocol_by_name(furi_string_get_cstr(buf));
        success = flipper_format_read_hex(ff, "address", (uint8_t*)&message.address, 4) &&
                  flipper_format_read_hex(ff, "command", (uint8_t*)&message.command, 4) &&
                  xremote_ir_signal_is_message_valid(&message);

        if(!success) break;

        xremote_ir_signal_set_message(item->ir_signal, &message);
    } while(false);

    furi_string_free(buf);
    return success;
}

void xremote_remote_item_free(CrossRemoteItem* item) {
    furi_string_free(item->name);
    //Determine type before free
    //xremote_ir_signal_free(item->ir_signal);
    //xremote_sg_remote_free(item->sg_signal);
    free(item);
}

void xremote_remote_item_set_type(CrossRemoteItem* item, int type) {
    item->type = type;
}

void xremote_remote_item_set_name(CrossRemoteItem* item, const char* name) {
    furi_string_set(item->name, name);
}

void xremote_remote_item_set_time(CrossRemoteItem* item, int32_t time) {
    item->time = time;
}

void xremote_remote_item_set_ir_signal(CrossRemoteItem* item, InfraredSignal* signal) {
    xremote_ir_signal_set_signal(item->ir_signal, signal);
}

void xremote_remote_item_set_sg_signal(CrossRemoteItem* item, SubGhzRemote* subghz) {
    item->sg_signal = subghz;
}

const char* xremote_remote_item_get_name(CrossRemoteItem* item) {
    return furi_string_get_cstr(item->name);
}

InfraredSignal* xremote_remote_item_get_ir_signal(CrossRemoteItem* item) {
    return item->ir_signal;
}

SubGhzRemote* xremote_remote_item_get_sg_signal(CrossRemoteItem* item) {
    return item->sg_signal;
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

bool xremote_pause_save(FlipperFormat* ff, int32_t time, const char* name) {
    if(!flipper_format_write_comment_cstr(ff, "") ||
       !flipper_format_write_string_cstr(ff, "remote_type", "PAUSE") ||
       !flipper_format_write_string_cstr(ff, "name", name) ||
       !flipper_format_write_int32(ff, "time", &time, 1)) {
        return false;
    }
    return true;
}

bool xremote_sg_signal_save(SubGhzRemote* remote, FlipperFormat* ff, const char* name) {
    if(!flipper_format_write_comment_cstr(ff, "") ||
       !flipper_format_write_string_cstr(ff, "remote_type", "SG") ||
       !flipper_format_write_string_cstr(ff, "name", name)) {
        return false;
    }
    return xremote_sg_signal_save_data(remote, ff);
}
