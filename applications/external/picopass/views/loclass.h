#pragma once
#include <stdint.h>
#include <gui/view.h>
#include <gui/modules/widget.h>

typedef struct Loclass Loclass;

typedef void (*LoclassCallback)(void* context);

Loclass* loclass_alloc();

void loclass_free(Loclass* loclass);

void loclass_reset(Loclass* loclass);

View* loclass_get_view(Loclass* loclass);

void loclass_set_callback(Loclass* loclass, LoclassCallback callback, void* context);

void loclass_set_header(Loclass* loclass, const char* header);

void loclass_set_num_macs(Loclass* loclass, uint16_t num_macs);
