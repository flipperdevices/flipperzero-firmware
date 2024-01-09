#ifndef FLIP_GRAV_SYNC_H
#define FLIP_GRAV_SYNC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_flip_struct.h"
#include "gravity_app_i.h"

bool syncNextToken(uint8_t** syncString, GravitySyncItem* tokenItem, char* tokenValue);
bool syncProcessResponse(GravityApp* app, bool syncFromFile);
void gravity_sync_rx_data_cb(uint8_t* buf, size_t len, void* context);
void syncCleanup();
void do_sync(GravityApp* app);

#endif