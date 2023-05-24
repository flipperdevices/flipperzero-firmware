#pragma once

#include "../infrared/xremote_ir_signal.h"
#include "../subghz/xremote_sg_remote.h"
#include "../subghz/subghz_i.h"
#include "../../xremote_i.h"

bool xremote_remote_item_read(CrossRemoteItem* item, FlipperFormat* ff);
bool xremote_remote_item_read_ir(CrossRemoteItem* item, FlipperFormat* ff);
bool xremote_remote_item_read_ir_signal_raw(CrossRemoteItem* item, FlipperFormat* ff);
bool xremote_remote_item_read_message(CrossRemoteItem* item, FlipperFormat* ff);

bool xremote_remote_item_read_pause(CrossRemoteItem* item, FlipperFormat* ff);

CrossRemoteItem* xremote_remote_item_alloc();
void xremote_remote_item_free(CrossRemoteItem* item);

void xremote_remote_item_set_name(CrossRemoteItem* item, const char* name);
const char* xremote_remote_item_get_name(CrossRemoteItem* item);

void xremote_remote_item_set_type(CrossRemoteItem* item, int type);
void xremote_remote_item_set_time(CrossRemoteItem* item, int32_t time);

InfraredSignal* xremote_remote_item_get_ir_signal(CrossRemoteItem* item);
void xremote_remote_item_set_ir_signal(CrossRemoteItem* item, InfraredSignal* signal);
SubGhzRemote* xremote_remote_item_get_sg_signal(CrossRemoteItem* item);
void xremote_remote_item_set_sg_signal(CrossRemoteItem* item, SubGhzRemote* subghz);

bool xremote_pause_save(FlipperFormat* ff, int32_t time, const char* name);
bool xremote_ir_signal_save(InfraredSignal* signal, FlipperFormat* ff, const char* name);
bool xremote_sg_signal_save(SubGhzRemote* remote, FlipperFormat* ff, const char* name);
