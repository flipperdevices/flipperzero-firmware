#pragma once

#include <flipper_application/api_hashtable/api_hashtable.h>

/* 
 * Resolver interface with private application's symbols. 
 * Implementation is contained in app_api_table.c
 */
#ifdef __cplusplus
extern "C" const ElfApiInterface* const application_api_interface;
#else
extern const ElfApiInterface* const application_api_interface;
#endif