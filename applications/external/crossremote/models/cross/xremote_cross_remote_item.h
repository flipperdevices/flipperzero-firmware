#pragma once

#include "../infrared/xremote_ir_signal.h"
#include "../subghz/xremote_sg_remote.h"
#include "../../xremote_i.h"

bool xremote_cross_remote_item_read(CrossRemoteItem* item, FlipperFormat* ff);

CrossRemoteItem* xremote_cross_remote_item_alloc();
void xremote_cross_remote_item_free(CrossRemoteItem* item);

void xremote_cross_remote_item_set_name(CrossRemoteItem* item, const char* name);
const char* xremote_cross_remote_item_get_name(CrossRemoteItem* item);
void xremote_cross_remote_item_set_filename(CrossRemoteItem* item, const char* filename);
const char* xremote_cross_remote_item_get_filename(CrossRemoteItem* item);

void xremote_cross_remote_item_set_type(CrossRemoteItem* item, int type);
void xremote_cross_remote_item_set_time(CrossRemoteItem* item, uint32_t time);
uint32_t xremote_cross_remote_item_get_time(CrossRemoteItem* item);

InfraredSignal* xremote_cross_remote_item_get_ir_signal(CrossRemoteItem* item);
void xremote_cross_remote_item_set_ir_signal(CrossRemoteItem* item, InfraredSignal* signal);
SubGhzRemote* xremote_cross_remote_item_get_sg_signal(CrossRemoteItem* item);
void xremote_cross_remote_item_set_sg_signal(CrossRemoteItem* item, SubGhzRemote* subghz);

bool xremote_cross_remote_item_pause_save(FlipperFormat* ff, uint32_t time, const char* name);
bool xremote_cross_remote_item_ir_signal_save(
    InfraredSignal* signal,
    FlipperFormat* ff,
    const char* name,
    uint32_t time);
bool xremote_cross_remote_item_sg_signal_save(
    SubGhzRemote* remote,
    FlipperFormat* ff,
    const char* name,
    const char* filename);
