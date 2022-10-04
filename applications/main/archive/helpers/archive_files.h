#pragma once

#include <m-array.h>
#include <m-string.h>
#include <storage/storage.h>

#define CUSTOM_ICON_MAX_SIZE 32

typedef enum {
    ArchiveFileTypeIButton,
    ArchiveFileTypeNFC,
    ArchiveFileTypeSubGhz,
    ArchiveFileTypeLFRFID,
    ArchiveFileTypeInfrared,
    ArchiveFileTypeBadUsb,
    ArchiveFileTypeU2f,
    ArchiveFileTypeUpdateManifest,
    ArchiveFileTypeApplication,
    ArchiveFileTypeFolder,
    ArchiveFileTypeUnknown,
    ArchiveFileTypeLoading,
} ArchiveFileTypeEnum;

typedef struct {
    string_t path;
    ArchiveFileTypeEnum type;
    uint8_t* custom_icon_data;
    string_t display_name;
    bool fav;
    bool is_app;
} ArchiveFile_t;

static void ArchiveFile_t_init(ArchiveFile_t* obj) {
    string_init(obj->path);
    obj->type = ArchiveFileTypeUnknown;
    obj->custom_icon_data = NULL;
    string_init(obj->display_name);
    obj->fav = false;
    obj->is_app = false;
}

static void ArchiveFile_t_init_set(ArchiveFile_t* obj, const ArchiveFile_t* src) {
    string_init_set(obj->path, src->path);
    obj->type = src->type;
    if(src->custom_icon_data) {
        obj->custom_icon_data = malloc(CUSTOM_ICON_MAX_SIZE);
        memcpy(obj->custom_icon_data, src->custom_icon_data, CUSTOM_ICON_MAX_SIZE);
    } else {
        obj->custom_icon_data = NULL;
    }
    string_init_set(obj->display_name, src->display_name);
    obj->fav = src->fav;
    obj->is_app = src->is_app;
}

static void ArchiveFile_t_set(ArchiveFile_t* obj, const ArchiveFile_t* src) {
    string_set(obj->path, src->path);
    obj->type = src->type;
    if(src->custom_icon_data) {
        obj->custom_icon_data = malloc(CUSTOM_ICON_MAX_SIZE);
        memcpy(obj->custom_icon_data, src->custom_icon_data, CUSTOM_ICON_MAX_SIZE);
    } else {
        obj->custom_icon_data = NULL;
    }
    string_set(obj->display_name, src->display_name);
    obj->fav = src->fav;
    obj->is_app = src->is_app;
}

static void ArchiveFile_t_clear(ArchiveFile_t* obj) {
    string_clear(obj->path);
    if(obj->custom_icon_data) {
        free(obj->custom_icon_data);
        obj->custom_icon_data = NULL;
    }
    string_clear(obj->display_name);
}

ARRAY_DEF(
    files_array,
    ArchiveFile_t,
    (INIT(API_2(ArchiveFile_t_init)),
     SET(API_6(ArchiveFile_t_set)),
     INIT_SET(API_6(ArchiveFile_t_init_set)),
     CLEAR(API_2(ArchiveFile_t_clear))))

void archive_set_file_type(ArchiveFile_t* file, const char* path, bool is_folder, bool is_app);
bool archive_get_items(void* context, const char* path);
void archive_file_append(const char* path, const char* format, ...);
void archive_delete_file(void* context, const char* format, ...);
