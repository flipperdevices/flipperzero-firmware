#pragma once

#include <flipper_application/flipper_application.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Object that manages application libraries
 * Implements mass loading of libraries and provides access to their descriptors
 */
typedef struct ApplicationLibManager ApplicationLibManager;

typedef enum {
    ApplicationLibManagerErrorNone = 0,
    ApplicationLibManagerErrorLoaderError,
    ApplicationLibManagerErrorApplicationIdMismatch,
    ApplicationLibManagerErrorAPIVersionMismatch,
} ApplicationLibManagerError;

/**
 * @brief Allocates new ApplicationLibManager
 * @param application_id Application ID filter - only libraries with matching ID will be loaded
 * @param api_version Application API version filter - only libraries with matching API version
 * @param api_interface Application API interface - used to load libraries. 
 * If library uses private application's API, use CompoundApiInterface
 * @return 
 */
ApplicationLibManager* application_lib_manager_alloc(
    const char* application_id,
    uint32_t api_version,
    const ElfApiInterface* api_interface);

/**
 * @brief Frees ApplicationLibManager
 * @param manager ApplicationLibManager instance
 */
void application_lib_manager_free(ApplicationLibManager* manager);

/**
 * @brief Loads single library by full path
 * @param manager ApplicationLibManager instance
 * @param path Path to library
 * @return Error code
 */
ApplicationLibManagerError
    application_lib_manager_load_single(ApplicationLibManager* manager, const char* path);

/**
 * @brief Loads all libraries from specified directory
 * @param manager ApplicationLibManager instance
 * @param path Path to directory
 * @return Error code
 */
ApplicationLibManagerError
    application_lib_manager_load_all(ApplicationLibManager* manager, const char* path);

/**
 * @brief Returns number of loaded libraries
 * @param manager ApplicationLibManager instance
 * @return Number of loaded libraries
 */
uint32_t application_lib_manager_get_count(ApplicationLibManager* manager);

/**
 * @brief Returns library descriptor by index
 * @param manager ApplicationLibManager instance
 * @param index Library index
 * @return Library descriptor
 */
const FlipperApplicationLibraryDescriptor*
    application_lib_manager_get(ApplicationLibManager* manager, uint32_t index);

/**
 * @brief Returns library entry point by index
 * @param manager ApplicationLibManager instance
 * @param index Library index
 * @return Library entry point
 */
const void* application_lib_manager_get_ep(ApplicationLibManager* manager, uint32_t index);

#ifdef __cplusplus
}
#endif
