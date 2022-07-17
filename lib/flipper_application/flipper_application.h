#pragma once

#include "application_manifest.h"

#include <furi.h>
#include <storage/storage.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FlipperApplicationPreloadStatusSuccess = 0,
    FlipperApplicationPreloadStatusUnspecifiedError,
    FlipperApplicationPreloadStatusInvalidFile,
    FlipperApplicationPreloadStatusInvalidManifest,
    FlipperApplicationPreloadStatusApiMismatch,
    FlipperApplicationPreloadStatusTargetMismatch,
} FlipperApplicationPreloadStatus;

typedef enum {
    FlipperApplicationLoadStatusSuccess,
    FlipperApplicationLoadStatusUnspecifiedError,
    FlipperApplicationLoadStatusNoFreeMemory,
    FlipperApplicationLoadStatusMissingImports,
} FlipperApplicationLoadStatus;

typedef struct FlipperApplication FlipperApplication;

typedef struct {
    const char* name;
    uint32_t address;
} FlipperApplicationMemoryMapEntry;

typedef struct {
    const char name[FAP_MANIFEST_MAX_APP_NAME_LENGTH];
    uint32_t version;
    FlipperApplicationMemoryMapEntry* entries;
    uint32_t mmap_entry_count;
} FlipperApplicationState;

typedef bool (*SymbolResolver)(const char* name, uint32_t* address);

FlipperApplication* flipper_application_alloc(Storage* storage, SymbolResolver resolver);

void flipper_application_free(FlipperApplication* app);

/* Parse headers, load manifest */
FlipperApplicationPreloadStatus
    flipper_application_preload(FlipperApplication* app, const char* path);

const FlipperApplicationManifest* flipper_application_get_manifest(FlipperApplication* app);

FlipperApplicationLoadStatus flipper_application_map_to_memory(FlipperApplication* app);

const FlipperApplicationState* flipper_application_get_state(FlipperApplication* app);

FuriThread* flipper_application_spawn(FlipperApplication* app, void* args);

#ifdef __cplusplus
}
#endif