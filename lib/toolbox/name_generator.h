#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Generates detailed/random name based on furi_hal flags
 * @param name buffer to write random name
 * @param max_name_size length of given buffer
 * @param app_name name of the application
 */
void name_generator_set(char* name, uint8_t max_name_size, const char* app_name);

/** Converts application extension to app_name
 * @param app_name name of the application
 * @return converted name
 */
const char* convert_app_extension_to_name(char* app_name);

/** Generates random name
 * @param name buffer to write random name
 * @param max_name_size length of given buffer
 */
void set_random_name(char* name, uint8_t max_name_size);

#ifdef __cplusplus
}
#endif