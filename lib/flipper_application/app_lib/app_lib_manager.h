#pragma once

#include <flipper_application/flipper_application.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ApplicationLibManager ApplicationLibManager;

typedef enum {
    ApplicationLibManagerErrorNone = 0,
    ApplicationLibManagerErrorLoaderError,
    ApplicationLibManagerErrorApplicationIdMismatch,
    ApplicationLibManagerErrorAPIVersionMismatch,
} ApplicationLibManagerError;

ApplicationLibManager* application_lib_manager_alloc(
    const char* application_id,
    uint32_t api_version,
    const ElfApiInterface* api_interface);

void application_lib_manager_free(ApplicationLibManager* manager);

ApplicationLibManagerError
    application_lib_manager_load_single(ApplicationLibManager* manager, const char* path);

ApplicationLibManagerError
    application_lib_manager_load_all(ApplicationLibManager* manager, const char* path);

uint32_t application_lib_manager_get_count(ApplicationLibManager* manager);

const FlipperApplicationLibraryDescriptor*
    application_lib_manager_get(ApplicationLibManager* manager, uint32_t index);

const void* application_lib_manager_get_ep(ApplicationLibManager* manager, uint32_t index);

#ifdef __cplusplus
}
#endif
