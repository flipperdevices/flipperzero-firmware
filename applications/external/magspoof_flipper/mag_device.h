#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include "mag_icons.h"

#define MAG_DEV_NAME_MAX_LEN 22
#define MAG_DEV_TRACKS 3

#define MAG_APP_FOLDER ANY_PATH("mag")
#define MAG_APP_EXTENSION ".mag"

typedef void (*MagLoadingCallback)(void* context, bool state);

typedef struct {
    FuriString* str;
    size_t len;
} MagTrack;

typedef struct {
    MagTrack track[MAG_DEV_TRACKS];
} MagDeviceData;

typedef struct {
    Storage* storage;
    DialogsApp* dialogs;
    MagDeviceData dev_data;
    char dev_name[MAG_DEV_NAME_MAX_LEN + 1];
    FuriString* load_path;
    MagLoadingCallback loading_cb;
    void* loading_cb_ctx;
} MagDevice;

MagDevice* mag_device_alloc();

void mag_device_free(MagDevice* mag_dev);

void mag_device_set_name(MagDevice* mag_dev, const char* name);

bool mag_device_save(MagDevice* mag_dev, const char* dev_name);

bool mag_file_select(MagDevice* mag_dev);

void mag_device_data_clear(MagDeviceData* dev_data);

void mag_device_clear(MagDevice* mag_dev);

bool mag_device_delete(MagDevice* mag_dev, bool use_load_path);

bool mag_device_parse_card_string(MagDevice* mag_dev, FuriString* card_str);

void mag_device_set_loading_callback(
    MagDevice* mag_dev,
    MagLoadingCallback callback,
    void* context);
