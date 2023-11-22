#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "../../types/automation_kb_layout.h"

#define TOTP_DEFAULT_KB_LAYOUT (0)
#define TOTP_KB_LAYOUT_DATA_LENGTH (36)
#define TOTP_KB_LAYOUT_NAME_MAX_LENGTH (10)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Loads keyboard layout into \c buffer
 * @param kb_layout keyboard layout
 * @param[out] buffer buffer to load keyboard layout into
 * @return \c true if keyboard layout loaded successfully; \c false otherwise
 */
bool totp_kb_layout_provider_get_layout_data(AutomationKeyboardLayout kb_layout, uint16_t* buffer);

/**
 * @brief Gets total keyboard layouts count
 * @return total keyboard layouts count
 */
uint8_t totp_kb_layout_provider_get_layouts_count();

/**
 * @brief Gets keyboard layout name of a given \c kb_layout into \c buffer
 * @param kb_layout keyboard layout
 * @param[out] buffer buffer to get keyboard layout name into
 * @param buffer_length available buffer length
 * @return \c true if keyboard layout name found successfully; \c false otherwise
 */
bool totp_kb_layout_provider_get_layout_name(
    AutomationKeyboardLayout kb_layout,
    char* buffer,
    size_t buffer_length);

/**
 * @brief Gets keyboard layout by \c name
 * @param name keyboard layout name
 * @param[out] kb_layout keyboard layout
 * @return \c true if keyboard layout found successfully; \c false otherwise
 */
bool totp_kb_layout_provider_get_layout_by_name(
    const char* name,
    AutomationKeyboardLayout* kb_layout);

#ifdef __cplusplus
}
#endif
