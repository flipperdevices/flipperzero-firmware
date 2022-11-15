/**
 * @file furi_hal_info.h
 * Device info HAL API
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <core/string.h>
#include <toolbox/rpc_helper.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Get device information
 *
 * @param[in]  callback     callback to provide with new data
 * @param[in]  sep          category separator character
 * @param[in]  context      context to pass to callback
 */
void furi_hal_info_get(RpcHelperPropertyCallback callback, char sep, void* context);

#ifdef __cplusplus
}
#endif
