#pragma once

#include <furi.h>
#include <furi_hal.h>
#include "rpc/rpc_app_error_codes.h"

typedef enum {
    SubGhzRpcErrorTypeOnlyRX =
        RpcAppSystemErrorCodesReserved, /** Transmission on this frequency is blocked by regional settings */
    SubGhzRpcErrorTypeParserOthers, /** Error in protocol parameters description */
} SubGhzRpcErrorType;
